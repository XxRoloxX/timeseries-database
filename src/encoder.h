
#pragma once

#include <vector>

using EncodedBuffer = std::vector<char>;

class Encodable {
public:
  virtual EncodedBuffer encode() = 0;
  // virtual void decode(EncodedBuffer data) = 0;
};


class Decodable{
public:
  virtual void decode() = 0;
  // virtual void decode(EncodedBuffer data) = 0;
};
