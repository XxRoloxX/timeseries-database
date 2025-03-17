#pragma once

#include "data_point.h"
#include "error.h"
#include <memory>
#include <vector>

template <typename K> class Decoder {
public:
  virtual Result<DataPoint<K>>
  decode(std::shared_ptr<std::vector<char>> data) = 0;
  virtual Result<std::vector<DataPoint<K>>>
  decode_many(std::shared_ptr<std::vector<char>> data) = 0;
};
