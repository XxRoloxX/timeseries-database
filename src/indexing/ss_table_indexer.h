#pragma once

#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../logger/logger.h"
#include "./indexes_block.h"
#include <memory>
#include <vector>

class SSTableIndexer {
private:
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;

public:
  SSTableIndexer(std::shared_ptr<Logger> logger,
                 std::shared_ptr<Decoder> decoder,
                 std::shared_ptr<Encoder> encoder);

  ~SSTableIndexer();

  IndexesMetadataBlock
  create_indexes(std::shared_ptr<std::vector<DataPoint>> datapoints);
};
