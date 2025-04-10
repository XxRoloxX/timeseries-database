
#pragma once

#include "../datapoints/data_point.h"

template <typename T> class TypedDataPoint {
private:
  DataPointKey timestamp;
  T value;

public:
  TypedDataPoint() = default;
  TypedDataPoint(DataPoint raw_datapoint) {

    int value_length = sizeof(T);

    std::memcpy(&this->value, raw_datapoint.get_value().data(), value_length);

    this->timestamp = raw_datapoint.get_key();
  };
  TypedDataPoint(DataPointKey key, T value) : timestamp(key), value(value) {};
  T get_value() { return value; }
  DataPointKey get_key() { return timestamp; }
  std::string to_string() {
    return std::format("Timestamp {}, Value {}", timestamp, value);
  }
};
