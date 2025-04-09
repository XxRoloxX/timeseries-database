#include "database.h"
#include "encoding/encodable.h"
#include "indexing/indexed_ss_table_reader.h"
#include "utils.h"
#include <memory>
#include <string>

void Database::insert(std::string series_name, DataPointKey key,
                      DataPointValue value) {

  this->logger->info(std::format("inserting {},{} in {}", key,
                                 encoded_buffer_to_string(value), series_name));

  auto cache = this->storage_manager->get_cache();

  if (cache->size(series_name) >= this->memtable_size) {
    auto cache_contents =
        std::make_shared<std::vector<DataPoint>>(cache->get_all(series_name));

    auto indexes = this->indexer->create_indexes(cache_contents);

    this->storage_manager->add_table(series_name, cache_contents, indexes);
    this->storage_manager->clear_cache(series_name);
    this->storage_manager->reload_tables();
    this->load_indexed_tables();

    this->compact_tables(series_name);
  }

  cache->set(series_name, key, DataPoint(key, value));
}

void Database::compact_tables(std::string series) {

  auto tables = this->tables_for_series(series);

  if (tables.size() > 1) {
    this->storage_manager->compact_tables(tables.at(0), tables.at(1));
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
      this->storage_manager->get_cache()->get_range(series_name, start, end);

  points = merge_points(&points, &from_memtable);

  return points;
};

Database::~Database() {}

Database::Database(std::shared_ptr<Logger> logger,
                   std::shared_ptr<Decoder> decoder,
                   std::shared_ptr<Encoder> encoder, size_t memtable_size,
                   std::shared_ptr<SSTableIndexer> indexer,
                   std::shared_ptr<StorageManager> storage_manager)
    : memtable_size(memtable_size), decoder(decoder), encoder(encoder),
      logger(logger), indexer(indexer), storage_manager(storage_manager) {

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

std::vector<IndexedSSTableReader>
Database::tables_for_series(std::string series) {

  std::vector<IndexedSSTableReader> filtered_tables;
  for (auto &reader : this->indexed_readers) {
    if (reader.get_series() == series) {
      filtered_tables.push_back(reader);
    }
  }

  return filtered_tables;
}
