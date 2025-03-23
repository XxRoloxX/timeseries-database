#pragma once

#include "../datapoints/data_point.h"
#include <memory>
#include <vector>

template <typename K> class Decoder {
public:
  virtual DataPoint<K> decode(std::shared_ptr<EncodedBuffer> data) = 0;
  virtual std::vector<DataPoint<K>>
  decode_many(std::shared_ptr<EncodedBuffer> data) = 0;
};
