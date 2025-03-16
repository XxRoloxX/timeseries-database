#pragma once

#include "encoder.h"

template <typename T> class DataPoint : public Encodable {
private:
  int timestamp;
  T value;

public:
  T get_value() { return value; }
  DataPoint(int timestamp, T value) {
    this->value = value;
    this->timestamp = timestamp;
  }
  EncodedBuffer encode() override {
    std::vector<char> encoded;

    encoded.push_back(timestamp);

    // TODO: Add support for bigger types (int32, float32, int64, float64,
    // structs, strings)
    encoded.push_back(value);

    return encoded;
  }
  void decode(EncodedBuffer data) {

    timestamp = data[0];
    value = data[1];
  }
};
