
#pragma once

#include "data_point.h"
#include "error.h"
#include <memory>
#include <vector>

template <typename K> class Encoder {
public:
  virtual Result<std::vector<char>>
  encode(std::shared_ptr<DataPoint<K>> data) = 0;
  virtual Result<std::vector<char>>
  encode_many(std::shared_ptr<std::vector<DataPoint<K>>> data) = 0;
};
