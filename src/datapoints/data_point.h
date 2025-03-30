#pragma once

#include "../encoding/encodable.h"
#include <format>
#include <memory>
#include <string>

using DataPointKey = int;
using DataPointValue = std::vector<char>;

class DataPoint : public Encodable {
private:
  DataPointKey timestamp;
  DataPointValue value;

public:
  DataPointValue get_value() { return value; }
  DataPointKey get_key() { return timestamp; }
  DataPoint() = default;
  DataPoint(DataPointKey timestamp, DataPointValue value) {
    this->value = value;
    this->timestamp = timestamp;
  }
  DataPoint(DataPoint &&point) = default;
  DataPoint(const DataPoint &point) = default;
  DataPoint &operator=(DataPoint &&data_point) = default;
  DataPoint &operator=(const DataPoint &data_point) = default;
  ~DataPoint() = default;
  EncodedBuffer encode() override {
    EncodedBuffer encoded;

    encoded.push_back(timestamp);

    // TODO: Add support for bigger types (int32, float32, int64, float64,
    // structs, strings)
    encoded.insert(encoded.end(), value.begin(), value.end());

    return encoded;
  }
  int decode(std::shared_ptr<EncodedBuffer> data) {

    timestamp = data->at(0);
    // TODO: add support for longer types.
    value = std::vector(data->begin() + 1, data->begin() + 2);
    return 2;
  }

  std::string to_string() const {
    return std::format(
        "Timestamp: {}, Value: {}", timestamp,
        encoded_buffer_to_string(make_shared<DataPointValue>(value)));
  }
};

// using RawDataPoint = DataPoint<EncodedBuffer>;
