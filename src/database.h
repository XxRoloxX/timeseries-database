#pragma once

#include "indexing/indexed_ss_table_reader.h"
#include "indexing/indexed_ss_table_writer.h"
#include "storage/storage_manager.h"
#include <string>

class Database {
public:
  ~Database();
  Database(std::shared_ptr<Logger> logger, std::shared_ptr<Decoder> decoder,
           std::shared_ptr<Encoder> encoder, size_t memtable_size,
           std::shared_ptr<IndexedSSTableWriter> ss_writer,
           std::shared_ptr<StorageManager> storage_manager);

  void insert(std::string series_name, DataPointKey key, DataPointValue value);
  std::vector<DataPoint> read(std::string series_name, DataPointKey from,
                              DataPointKey to);

private:
  size_t memtable_size;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<IndexedSSTableWriter> ss_writer;
  std::shared_ptr<StorageManager> storage_manager;
  std::vector<IndexedSSTableReader> indexed_readers;
  void compact_tables(std::string series);
  void load_indexed_tables();
  std::vector<IndexedSSTableReader> tables_for_series(std::string series);
};
