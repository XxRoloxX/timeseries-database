
#include "../datapoints/data_point.h"

template <typename T> class TypedDataPoint {
private:
  DataPointKey timestamp;
  T value;

public:
  TypedDataPoint() = default;
  TypedDataPoint(DataPointKey key, T value) : timestamp(key), value(value) {};
  T get_value() { return value; }
  DataPointKey get_key() { return timestamp; }
  std::string to_string() {
    return std::format("Timestamp {}, Value {}", timestamp, value);
  }
};
