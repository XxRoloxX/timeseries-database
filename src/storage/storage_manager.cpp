#include "./storage_manager.h"
#include "../cache/write_back_cache.h"
#include "../datapoints/data_point.h"
#include "../indexing/indexed_ss_table_reader.h"
#include "../utils.h"
#include "ss_table_storage.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

StorageManager::StorageManager(
    std::shared_ptr<Logger> logger, std::shared_ptr<Decoder> decoder,
    std::shared_ptr<Encoder> encoder,
    std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache,
    std::shared_ptr<IndexedSSTableWriter> ss_writer, std::string base_path)
    : base_path(base_path), cache(cache), logger(logger), decoder(decoder),
      encoder(encoder), ss_writer(ss_writer) {

  this->load_tables();
  this->load_cache();
}

void StorageManager::load_tables() {

  if (!std::filesystem::is_directory(this->base_path)) {
    this->logger->info(std::format(
        "No storage path, skipping loading tables: {}", this->base_path));
    return;
  }

  this->tables.clear();

  for (const auto &file :
       std::filesystem::directory_iterator(this->base_path)) {

    std::filesystem::path path = file.path();

    if (path.filename() == CACHE_PATH) {
      continue;
    }

    std::string series = this->get_series_from_table_name(path.filename());

    std::shared_ptr<SSTableStorage> storage = make_shared<SSTableStorage>(
        this->logger, this->base_path, path.filename(), series);

    tables.push_back(storage);
  }
};

void StorageManager::load_cache() {

  if (!std::filesystem::is_directory(this->base_path)) {
    this->logger->info(std::format(
        "No storage path, skipping loading tables: {}", this->base_path));
    return;
  }

  if (!std::filesystem::exists(this->get_storage_path(CACHE_PATH))) {
    return;
  }

  std::ifstream file(this->get_storage_path(CACHE_PATH),
                     std::ios::in | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read cache file at: " +
                        this->get_storage_path(CACHE_PATH));
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  file.seekg(0, std::ios::end);

  auto end_pos = file.tellg();

  file.seekg(0, std::ios::beg);

  std::streamsize size = end_pos;

  std::shared_ptr<EncodedBuffer> file_data = std::make_shared<EncodedBuffer>();
  file_data->resize(size);

  if (!file.read(file_data->data(), size)) {
    this->logger->error("failed to load table from file");
  }

  auto points = this->decoder->decode_many(file_data);

  this->logger->info(
      std::format("reading {} points from cache", points.size()));

  for (auto &point : points) {
    cache->set(point.get_key(), point);
  }

  file.close();
};

void StorageManager::clear_cache() {
  auto size = this->cache->purge();
  this->logger->info(std::format("purged {} elements from cache", size));
  std::filesystem::remove(this->get_storage_path(CACHE_PATH));
}

void StorageManager::reload_tables() {
  this->load_tables();
  this->load_cache();
}

std::vector<std::shared_ptr<SSTableStorage>> StorageManager::get_tables() {
  return this->tables;
}

std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>>
StorageManager::get_cache() {
  return cache;
}

void StorageManager::save_cache() {

  std::shared_ptr<std::vector<DataPoint>> points =
      std::make_shared<std::vector<DataPoint>>(this->cache->get_all());

  this->logger->info(std::format("saving {} points to cache", points->size()));

  auto encoded_points = this->encoder->encode_many(points);

  if (!std::filesystem::is_directory(this->base_path)) {
    std::filesystem::create_directory(this->base_path);
  }

  std::ofstream file(this->get_storage_path(CACHE_PATH),
                     std::ios::out | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read cache file at: " + CACHE_PATH);
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  file.write(encoded_points.data(), encoded_points.size());
  file.close();
};

std::string StorageManager::create_table_name(std::string series_name) {

  auto now = std::chrono::system_clock::now();

  auto duration = now.time_since_epoch();

  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  return std::format("{}-{}", series_name, milliseconds);
}

void StorageManager::add_table(std::string series,
                               std::shared_ptr<std::vector<DataPoint>> points,
                               IndexesMetadataBlock indexes) {

  if (!std::filesystem::is_directory(this->base_path)) {
    std::filesystem::create_directory(this->base_path);
  }

  auto table_name = this->create_table_name(series);

  std::shared_ptr<SSTableStorage> table = std::make_shared<SSTableStorage>(
      this->logger, this->base_path, table_name, series);

  auto encoded_points = this->encoder->encode_many(points);
  auto encoded_index = indexes.encode();

  table->set_data(encoded_points);
  table->set_indexes(encoded_index);

  table->save();
}

void StorageManager::append_to_table(
    std::string series, std::string table_name,
    std::shared_ptr<std::vector<DataPoint>> points,
    IndexesMetadataBlock indexes) {

  if (!std::filesystem::is_directory(this->base_path)) {
    std::filesystem::create_directory(this->base_path);
  }

  std::shared_ptr<SSTableStorage> table = std::make_shared<SSTableStorage>(
      this->logger, this->base_path, table_name, series);

  auto encoded_points = this->encoder->encode_many(points);
  auto encoded_index = indexes.encode();

  // Initialize table to load metadata of the index block
  table->initialize();
  table->append_save(encoded_points, encoded_index);
}

std::string StorageManager::get_storage_path(std::string table_name) {
  return this->base_path + "/" + table_name;
};

void StorageManager::compact_tables(IndexedSSTableReader table_a,
                                    IndexedSSTableReader table_b) {

  int batch_size = 32000;

  auto indexes_a = table_a.get_indexes();
  auto indexes_b = table_b.get_indexes();

  IndexMapping smallest_index;
  IndexMapping biggest_index;
  IndexMapping current_index;

  // // Get all elements from a that are smaller than common ones
  // // a -> 1,2,3,4,5,6,7,8
  // // b -> 4,5,6,7,8,6,5
  if (indexes_a->first_index() > indexes_b->first_index()) {
    smallest_index = indexes_b->first_index();
  } else {
    smallest_index = indexes_a->first_index();
  }

  if (indexes_a->last_index() > indexes_b->last_index()) {
    biggest_index = indexes_b->last_index();
  } else {
    biggest_index = indexes_a->last_index();
  }

  current_index = smallest_index;

  SSTableStorage new_table(this->logger, this->base_path,
                           this->create_table_name(table_a.get_series()),
                           table_a.get_series());

  while (current_index.key <= biggest_index.key) {
    std::vector<DataPoint> batch_points;

    auto points_a =
        table_a.read_range(current_index.key, current_index.key + batch_size);

    auto points_b =
        table_b.read_range(current_index.key, current_index.key + batch_size);

    auto merged_points = std::make_shared<std::vector<DataPoint>>(
        merge_points(&(*points_a), &(*points_b)));

    auto new_indexes = this->ss_writer->create_indexes(merged_points);

    current_index.key += batch_size;

    auto encoded_points = this->encoder->encode_many(merged_points);
    auto encoded_indexes = new_indexes.encode();

    new_table.append_save(encoded_points, encoded_indexes);
  }

  table_a.remove();
  table_b.remove();

  this->reload_tables();
}

StorageManager::~StorageManager() { this->save_cache(); }

std::string StorageManager::get_series_from_table_name(std::string name) {
  auto splitted_path = split(name, "-");
  return splitted_path[0];
}
