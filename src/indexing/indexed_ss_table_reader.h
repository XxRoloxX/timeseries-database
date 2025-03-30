#pragma once

#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../indexing/indexes_block.h"
#include "../logger/logger.h"
#include "../storage/ss_table_storage.h"
#include <memory>
#include <vector>

class IndexedSSTableReader {
private:
  std::string name;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<SSTableStorage> raw_table;
  std::vector<DataPoint> data;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;
  IndexesMetadataBlock indexes;

public:
  IndexedSSTableReader(std::shared_ptr<Logger> logger,
                       std::shared_ptr<SSTableStorage> raw_table,
                       std::shared_ptr<Decoder> decoder,
                       std::shared_ptr<Encoder> encoder);

  ~IndexedSSTableReader();

  void initialize();
  std::shared_ptr<std::vector<DataPoint>> read_range(DataPointKey start_key,
                                                     DataPointKey end_key);
  std::shared_ptr<std::vector<DataPoint>> read_all();
  std::string get_name();
  std::shared_ptr<std::vector<DataPoint>> get_data();
  std::shared_ptr<std::vector<DataPoint>> get_indexes();
};
