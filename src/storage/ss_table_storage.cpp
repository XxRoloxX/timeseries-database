#include "../storage/ss_table_storage.h"
#include "../logger/logger.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>

SSTableStorage::SSTableStorage(std::shared_ptr<Logger> logger,
                               std::string base_path, std::string name,
                               std::string series)
    : logger(logger), name(std::move(name)), series(series),
      base_path(base_path) {

  this->logger->info(std::format("loaded storage for {}", this->name));
}

std::shared_ptr<EncodedBuffer> SSTableStorage::read_range(std::size_t startByte,
                                                          std::size_t endByte) {

  std::ifstream file(this->get_path(), std::ios::in | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  // Skipping the bytes of the metadata block
  file.seekg(startByte + INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES,
             std::ios::beg);

  std::streamsize size = endByte - startByte;

  this->logger->info(
      std::format("reading {} bytes from {} storage", size, this->name));

  std::vector<char> file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  return std::make_shared<EncodedBuffer>(file_data);
};

std::shared_ptr<EncodedBuffer> SSTableStorage::read_all() {
  std::ifstream file(this->get_path(), std::ios::in | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  file.seekg(0, std::ios::end);

  auto end_pos = file.tellg();

  file.seekg(INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES, std::ios::beg);

  std::streamsize size =
      int(end_pos) - INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES;

  this->logger->info(std::format("reading {} bytes from {}", size, this->name));

  std::vector<char> file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  return std::make_shared<EncodedBuffer>(file_data);
}

void SSTableStorage::load(EncodedBuffer *raw) {

  // We start reading the blob with metadata block.
  auto _ = metadata.load(raw);

  std::vector<char> new_indices(
      raw->begin() + metadata.get_indexes_block_offset(), raw->end());

  this->indices = std::move(new_indices);
}

void SSTableStorage::save() {

  if (!std::filesystem::is_directory(this->base_path)) {
    std::filesystem::create_directory(this->base_path);
  }

  std::ofstream file(this->get_path(), std::ios::binary);
  if (!file) {
    this->logger->error("failed to open a file: " + this->name);
    return;
  }

  this->logger->info(std::format("writing to file: {}, size: {}", this->name,
                                 this->data.size() + this->indices.size()));

  // We are assuming here that whole table was loaded to the memory, and
  // contents are in this->data (done when saving to disk for the first time)
  this->update_metadata(this->data.size());

  EncodedBuffer encoded_metadata = this->metadata.encode();
  file.write(encoded_metadata.data(), encoded_metadata.size());
  file.write(this->data.data(), this->data.size());
  file.write(this->indices.data(), this->indices.size());

  this->logger->info(std::format("successfuly written {} bytes to {}",
                                 this->data.size() + this->indices.size(),
                                 this->name));
};

void SSTableStorage::append_save(EncodedBuffer new_data,
                                 EncodedBuffer new_indexes) {

  if (!std::filesystem::is_directory(this->base_path)) {
    std::filesystem::create_directory(this->base_path);
  }

  // If table doesn't exist yet, we simply create it
  if (!std::filesystem::exists(this->get_path())) {
    this->set_data(new_data);
    this->set_indexes(new_indexes);
    this->save();
    return;
  }

  std::fstream file(this->get_path(), std::ios::binary | std::ios::app);
  if (!file) {
    this->logger->error("failed to open a file: " + this->name);
    return;
  }

  this->logger->info(std::format("appending to file: {}, size: {}", this->name,
                                 new_data.size() + new_indexes.size()));

  this->initialize();

  size_t old_index_start_offset = this->metadata.get_indexes_block_offset();
  size_t new_index_start_offset = old_index_start_offset + new_data.size();

  // Updating index block offset metadata
  this->metadata.set_indexes_block_offset(new_index_start_offset);

  file.seekg(0, std::ios::beg);
  auto encoded_metadata = this->metadata.encode();
  file.write(encoded_metadata.data(), encoded_metadata.size());

  // Writing the new data at the end of the old one
  file.seekg(old_index_start_offset, std::ios::beg);
  file.write(new_data.data(), new_data.size());

  // Writing new indexes
  file.seekg(new_index_start_offset, std::ios::beg);
  this->indices.insert(this->indices.end(), new_data.begin(), new_data.end());
  file.write(this->indices.data(), this->indices.size());

  this->logger->info(std::format("successfuly written {} bytes to {}",
                                 this->data.size() + this->indices.size(),
                                 this->name));
};

void SSTableStorage::update_metadata(size_t data_size) {
  this->metadata.set_indexes_block_offset(
      data_size + INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES);
}

void SSTableStorage::initialize() {

  if (this->indices.size() != 0) {
    // Already initialized
    return;
  }

  // We skip initialization if file is not present (table doesn't exist on disk
  // yet)
  std::ifstream file(this->get_path(), std::ios::in | std::ofstream::binary);
  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    return;
  }

  // TODO: REMOVE TO NOT LOAD WHOLE FILE, AS the table raw data is not needed in
  // memory
  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();

  this->logger->info(std::format("reading file of size: {}", size));
  file.seekg(0, std::ios::beg);

  this->data.resize(size);

  EncodedBuffer file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  this->load(&file_data);

  this->logger->info(std::format("initialized table {}", this->name));
}

std::string SSTableStorage::get_path() {
  return this->base_path + "/" + this->name;
}

std::shared_ptr<EncodedBuffer> SSTableStorage::get_data() {
  return std::make_shared<EncodedBuffer>(this->data);
}

void SSTableStorage::set_data(EncodedBuffer new_data) {
  this->data = std::move(new_data);
}

void SSTableStorage::set_indexes(EncodedBuffer new_indexes) {
  this->indices = std::move(new_indexes);
}

std::shared_ptr<EncodedBuffer> SSTableStorage::get_indexes() {
  return std::make_shared<EncodedBuffer>(this->indices);
}

std::string SSTableStorage::get_series() { return this->series; }

void SSTableStorage::set_base_path(std::string path) { this->base_path = path; }

std::string SSTableStorage::get_name() { return this->name; }

void SSTableStorage::remove() {

  if (!std::filesystem::exists(this->get_path())) {
    this->logger->error(
        std::format("failed to remove ss table storage (file {} doesn't exist)",
                    this->get_path()));
    return;
  }

  std::filesystem::remove(this->get_path());
  this->logger->info(
      std::format("removed compacted ss_table: {}", this->get_path()));
}
