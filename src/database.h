#pragma once

#include "cache/write_back_cache.h"
#include "indexing/indexed_ss_table_reader.h"
#include "indexing/indexed_ss_table_writer.h"
#include <string>

class Database {
public:
  ~Database();
  Database(std::shared_ptr<Logger> logger, std::shared_ptr<Decoder> decoder,
           std::shared_ptr<Encoder> encoder, size_t memtable_size,
           std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache,
           std::shared_ptr<IndexedSSTableWriter> ss_writer);

  void insert(std::string series_name, DataPointKey key, DataPointValue value);

  std::vector<DataPoint> read(DataPointKey from, DataPointKey to);

private:
  size_t memtable_size;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> memtable;
  std::shared_ptr<IndexedSSTableWriter> ss_writer;
  std::vector<IndexedSSTableReader> readers;

  void save_table(std::string series_name, std::vector<DataPoint> points);
  void load_tables();
  void clear_cache();
};
