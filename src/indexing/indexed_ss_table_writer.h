#pragma once

#include "../datapoints/data_point.h"
#include "../encoding/decoder.h"
#include "../encoding/encoder.h"
#include "../logger/logger.h"
#include <memory>
#include <vector>

template <typename K> class IndexedSSTableWriter {
private:
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Decoder<K>> decoder;
  std::shared_ptr<Encoder<K>> encoder;

public:
  IndexedSSTableWriter(std::shared_ptr<Logger> logger,
                       std::shared_ptr<Decoder<K>> decoder,
                       std::shared_ptr<Encoder<K>> encoder);

  ~IndexedSSTableWriter();

  // void initialize();
  void save(std::string name,
            std::shared_ptr<std::vector<DataPoint<K>>> datapoints);
};
