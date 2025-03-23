#pragma once

#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../indexing/indexes_block.h"
#include "../logger/logger.h"
#include "../storage/ss_table_storage.h"
#include <memory>
#include <vector>

template <typename K> class IndexedSSTable {
private:
  std::string name;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<SSTableStorage> raw_table;
  std::vector<DataPoint<K>> data;
  std::shared_ptr<Decoder<K>> decoder;
  std::shared_ptr<Encoder<K>> encoder;
  IndexesMetadataBlock indexes;

public:
  IndexedSSTable(std::shared_ptr<Logger> logger,
                 std::shared_ptr<SSTableStorage> raw_table,
                 std::shared_ptr<Decoder<K>> decoder,
                 std::shared_ptr<Encoder<K>> encoder);

  ~IndexedSSTable();

  void initialize();
  std::shared_ptr<std::vector<DataPoint<K>>> read_range(DataPointKey start_key,
                                                        DataPointKey end_key);
  // void save();
  std::string get_name();
  std::shared_ptr<std::vector<DataPoint<K>>> get_data();
  std::shared_ptr<std::vector<DataPoint<K>>> get_indexes();
};
