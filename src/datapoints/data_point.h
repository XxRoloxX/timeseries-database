#pragma once

#include "../encoding/encodable.h"
#include <format>
#include <memory>
#include <string>

using DataPointKey = int;

template <typename T> class DataPoint : public Encodable {
private:
  DataPointKey timestamp;
  T value;

public:
  T get_value() { return value; }
  DataPoint() = default;
  DataPoint(DataPointKey timestamp, T value) {
    this->value = value;
    this->timestamp = timestamp;
  }
  DataPoint(DataPoint<T> &&point) = default;
  DataPoint(const DataPoint<T> &point) = default;
  DataPoint<T> &operator=(DataPoint<T> &&data_point) = default;
  DataPoint<T> &operator=(const DataPoint<T> &data_point) = default;
  ~DataPoint() = default;
  EncodedBuffer encode() override {
    EncodedBuffer encoded;

    encoded.push_back(timestamp);

    // TODO: Add support for bigger types (int32, float32, int64, float64,
    // structs, strings)
    encoded.push_back(value);

    return encoded;
  }
  int decode(std::shared_ptr<EncodedBuffer> data) {

    timestamp = (*data)[0];
    value = (*data)[1];

    // Read bytes.
    return 2;
  }

  std::string to_string() const {
    return std::format("Timestamp: {}, Value: {}", timestamp, value);
  }
};
