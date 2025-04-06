#pragma once

#include "../encoding/encodable.h"
#include <cstring>
#include <format>
#include <memory>
#include <string>

using DataPointKey = int;
using DataPointValue = std::vector<char>;

// Maximum length of the datapoint value would be 2^7-1 = 127
const unsigned char POINT_LENGTH_BYTES = 1;

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
    EncodedBuffer encoded(POINT_LENGTH_BYTES + sizeof(DataPointKey));

    unsigned char value_length = value.size();
    std::memcpy(encoded.data(), &value_length, POINT_LENGTH_BYTES);

    std::memcpy(encoded.data() + POINT_LENGTH_BYTES, &timestamp,
                sizeof(DataPointKey));

    encoded.insert(encoded.end(), value.begin(), value.end());

    return encoded;
  }

  int decode(std::shared_ptr<EncodedBuffer> data) {

    unsigned char value_length;

    std::memcpy(&value_length, data->data(), POINT_LENGTH_BYTES);

    std::memcpy(&timestamp, data->data() + POINT_LENGTH_BYTES,
                sizeof(DataPointKey));
    value =
        std::vector(data->begin() + POINT_LENGTH_BYTES + sizeof(DataPointKey),
                    data->begin() + POINT_LENGTH_BYTES + sizeof(DataPointKey) +
                        value_length);

    return POINT_LENGTH_BYTES + sizeof(DataPointKey) + value_length;
  }

  std::string to_string() {
    return std::format("Timestamp: {}, Value: {}", timestamp,
                       encoded_buffer_to_string(value));
  }
};
