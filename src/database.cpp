#include "database.h"
#include "encoding/encodable.h"
#include "indexing/indexed_ss_table_reader.h"
#include "utils.h"
#include <memory>
#include <string>

using std::move;

void Database::insert(std::string series_name, DataPointKey key,
                      DataPointValue value) {

  this->logger->info(std::format("inserting {},{} in {}", key,
                                 encoded_buffer_to_string(value), series_name));

  auto cache = this->storage_manager->get_cache();

  if (cache->size() >= this->memtable_size) {
    auto cache_contents =
        std::make_shared<std::vector<DataPoint>>(cache->get_all());

    auto indexes = this->ss_writer->create_indexes(cache_contents);

    this->storage_manager->add_table(series_name, cache_contents, indexes);
    this->storage_manager->clear_cache();
    this->storage_manager->reload_tables();
    this->load_indexed_tables();

    // TODO remove after tests
    this->compact_tables();
  }

  cache->set(key, DataPoint(key, value));
}

void Database::compact_tables() {

  if (indexed_readers.size() > 1) {
    this->storage_manager->compact_tables(this->indexed_readers.at(0),
                                          this->indexed_readers.at(1));
  }

  this->storage_manager->reload_tables();

  this->load_indexed_tables();
}

std::vector<DataPoint> Database::read(std::string series_name,
                                      DataPointKey start, DataPointKey end) {

  this->logger->info(
      std::format("reading from {} to {} for {}", series_name, start, end));

  std::vector<DataPoint> points;
  for (auto &reader : this->indexed_readers) {

    // Skipping tables from other series
    if (reader.get_series() != series_name) {
      continue;
    }

    auto from_table = reader.read_range(start, end);
    points = merge_points(&points, &(*from_table));
  }

  auto from_memtable =
      this->storage_manager->get_cache()->get_range(start, end);

  points = merge_points(&points, &from_memtable);

  return points;
};

Database::~Database() {}

Database::Database(std::shared_ptr<Logger> logger,
                   std::shared_ptr<Decoder> decoder,
                   std::shared_ptr<Encoder> encoder, size_t memtable_size,
                   std::shared_ptr<IndexedSSTableWriter> ss_writer,
                   std::shared_ptr<StorageManager> storage_manager)
    : memtable_size(memtable_size), decoder(decoder), encoder(encoder),
      logger(logger), ss_writer(ss_writer), storage_manager(storage_manager) {

  this->load_indexed_tables();
}

void Database::load_indexed_tables() {

  this->indexed_readers.clear();

  auto tables = this->storage_manager->get_tables();
  for (auto table : tables) {

    IndexedSSTableReader reader(this->logger, table, this->decoder,
                                this->encoder);

    reader.initialize();

    this->indexed_readers.push_back(reader);
  }
}
