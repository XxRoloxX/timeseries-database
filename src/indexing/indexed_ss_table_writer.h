#pragma once

#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../logger/logger.h"
#include <memory>
#include <vector>

class IndexedSSTableWriter {
private:
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Decoder> decoder;
  std::shared_ptr<Encoder> encoder;

public:
  IndexedSSTableWriter(std::shared_ptr<Logger> logger,
                       std::shared_ptr<Decoder> decoder,
                       std::shared_ptr<Encoder> encoder);

  ~IndexedSSTableWriter();

  void save(std::string name,
            std::shared_ptr<std::vector<DataPoint>> datapoints);
};
