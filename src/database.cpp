#include "database.h"
#include "indexing/indexed_ss_table_reader.h"
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

void Database::insert(std::string series_name, DataPointKey key,
                      DataPointValue value) {

  if (this->memtable->size() >= this->memtable_size) {
    auto cache_contents = this->memtable->get_all();
    this->save_table(series_name, cache_contents);
    this->load_tables();
    this->memtable->purge();
  }

  this->memtable->set(key, DataPoint(key, value));
}

void Database::save_table(std::string series_name,
                          std::vector<DataPoint> points) {

  auto now = std::chrono::system_clock::now();

  auto duration = now.time_since_epoch();

  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  std::string name_with_timestamp =
      std::format("{}-{}", series_name, milliseconds);

  this->ss_writer->save(name_with_timestamp,
                        std::make_shared<std::vector<DataPoint>>(points));
}

std::vector<DataPoint> Database::read(DataPointKey start, DataPointKey end) {

  std::vector<DataPoint> points;
  for (auto &reader : this->readers) {
    auto from_table = reader.read_range(start, end);
    points.insert(points.end(), from_table->begin(), from_table->end());
  }

  auto from_memtable = this->memtable->get_range(start, end);

  points.insert(points.end(), from_memtable.begin(), from_memtable.end());

  return points;
};

void Database::load_tables() {
  this->readers = std::vector<IndexedSSTableReader>();

  for (const auto &file : std::filesystem::directory_iterator(STORAGE_PATH)) {

    std::filesystem::path path = file.path();

    std::shared_ptr<SSTableStorage> storage =
        make_shared<SSTableStorage>(this->logger, path.filename());

    IndexedSSTableReader reader(this->logger, storage, this->decoder,
                                this->encoder);

    reader.initialize();

    if (path.filename() == "cache") {
      auto cached_points = reader.read_all();
      for (auto &point : *cached_points) {
        this->memtable->set(point.get_key(), point);
      }

      this->logger->info(std::format("loaded {} points from saved cache",
                                     cached_points->size()));
    }

    this->readers.push_back(reader);
  }
}
Database::~Database() {
  auto cached_points = this->memtable->get_all();

  SSTableStorage storage(this->logger, "cache");

  auto encoded_points = this->encoder->encode_many(
      std::make_shared<std::vector<DataPoint>>(cached_points));

  storage.set_data(encoded_points);

  storage.save();
}

Database::Database(
    std::shared_ptr<Logger> logger, std::shared_ptr<Decoder> decoder,
    std::shared_ptr<Encoder> encoder, size_t memtable_size,
    std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache,
    std::shared_ptr<IndexedSSTableWriter> ss_writer)
    : memtable_size(memtable_size), decoder(decoder), encoder(encoder),
      logger(logger), memtable(cache), ss_writer(ss_writer) {

  this->load_tables();
}
