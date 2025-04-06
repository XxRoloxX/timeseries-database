
#pragma once

#include <format>
#include <memory>
#include <string>
#include <vector>

using EncodedBuffer = std::vector<char>;

inline std::string encoded_buffer_to_string(EncodedBuffer &buf) {
  std::string res = "[";
  for (auto &ch : buf) {
    res.append(std::format("{}", int(ch)));
    res.append(",");
  }

  res.append("]");

  return res;
}

class Encodable {
public:
  virtual EncodedBuffer encode() = 0;
  virtual ~Encodable() = default;
  // virtual void decode(EncodedBuffer data) = 0;
};

class Decodable {
public:
  virtual void decode() = 0;
  virtual ~Decodable() = default;
  ;
  // virtual void decode(EncodedBuffer data) = 0;
};
