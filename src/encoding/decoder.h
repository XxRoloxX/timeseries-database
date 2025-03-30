#pragma once

#include "../datapoints/data_point.h"
#include <memory>
#include <vector>

class Decoder {
public:
  virtual DataPoint decode(std::shared_ptr<EncodedBuffer> data) = 0;
  virtual std::vector<DataPoint>
  decode_many(std::shared_ptr<EncodedBuffer> data) = 0;
};
