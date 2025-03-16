
#pragma once

#include "error.h"
#include <memory>
#include <variant>
#include <vector>

using EncodedBuffer = std::vector<char>;

class Encodable {
public:
  virtual EncodedBuffer encode() = 0;
  // virtual void decode(EncodedBuffer data) = 0;
};

class Decodable {
public:
  virtual void decode() = 0;
  // virtual void decode(EncodedBuffer data) = 0;
};

template <typename K> class Encoder {
  virtual Result<std::vector<char>> encode(std::shared_ptr<K> data) = 0;
  virtual Result<std::vector<char>>
  encode_many(std::shared_ptr<std::vector<K>> data) = 0;
};
