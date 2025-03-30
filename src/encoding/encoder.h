
#pragma once

#include "../datapoints/data_point.h"
#include "encodable.h"
#include <memory>
#include <vector>

class Encoder {
public:
  virtual EncodedBuffer encode(std::shared_ptr<DataPoint> data) = 0;
  virtual EncodedBuffer
  encode_many(std::shared_ptr<std::vector<DataPoint>> data) = 0;
};
