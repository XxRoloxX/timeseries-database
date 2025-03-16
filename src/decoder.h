#pragma once

#include "error.h"
#include <vector>

template <typename K> class Decoder {
  virtual Result<K> decode(std::vector<char> data) = 0;
  virtual Result<std::vector<K>> decode_many(std::vector<char> data) = 0;
};
