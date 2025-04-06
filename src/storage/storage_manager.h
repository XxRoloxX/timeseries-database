#pragma once

#include "../cache/write_back_cache.h"
#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../indexing/indexed_ss_table_reader.h"
#include "../indexing/indexed_ss_table_writer.h"
#include "../indexing/indexes_block.h"
#include "./ss_table_storage.h"
#include <string>
#include <vector>

const std::string CACHE_PATH = "cache";

class StorageManager {

private:
  std::string base_path;
  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache;
  std::vector<std::shared_ptr<SSTableStorage>> tables;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;
  std::shared_ptr<IndexedSSTableWriter> ss_writer;
  std::string get_storage_path(std::string table_name);
  void load_tables();
  void load_cache();
  std::string create_table_name(std::string series_name);
  std::string get_series_from_table_name(std::string path);
  void append_to_table(std::string series, std::string table_name,
                       std::shared_ptr<std::vector<DataPoint>> points,
                       IndexesMetadataBlock indexes);

public:
  StorageManager(std::shared_ptr<Logger> logger,
                 std::shared_ptr<Decoder> decoder,
                 std::shared_ptr<Encoder> encoder,
                 std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache,
                 std::shared_ptr<IndexedSSTableWriter> ss_writer,
                 std::string base_path);
  ~StorageManager();
  std::vector<std::shared_ptr<SSTableStorage>> get_tables();
  void add_table(std::string series,
                 std::shared_ptr<std::vector<DataPoint>> datapoints,
                 IndexesMetadataBlock index);
  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> get_cache();

  void compact_tables(IndexedSSTableReader table_a,
                      IndexedSSTableReader table_b);

  void reload_tables();
  void clear_cache();
  void save_cache();
};
