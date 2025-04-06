#include "../database.h"
#include "../datapoints/data_point.h"
#include "typed_datapoint.h"

template <typename T> class TypedSeries {
private:
  std::string series;
  std::shared_ptr<Database> database;

public:
  TypedSeries(std::string series, std::shared_ptr<Database> database)
      : series(series), database(database) {};
  void insert(DataPointKey key, T value) {

    auto value_length = sizeof(T);

    EncodedBuffer encoded_value(POINT_LENGTH_BYTES + value_length);

    std::memcpy(encoded_value.data(), &value_length,
                sizeof(POINT_LENGTH_BYTES));

    std::memcpy(encoded_value.data() + POINT_LENGTH_BYTES, &value,
                value_length);

    this->database->insert(this->series, key, encoded_value);
  };

  std::vector<TypedDataPoint<T>> read(DataPointKey from, DataPointKey to) {
    std::vector<DataPoint> raw_datapoints =
        this->database->read(this->series, from, to);

    std::vector<TypedDataPoint<T>> datapoints;

    for (auto &point : raw_datapoints) {
      EncodedBuffer value = point.get_value();
      T typed_value;

      std::memcpy(&typed_value, value.data() + POINT_LENGTH_BYTES, sizeof(T));
      datapoints.push_back(TypedDataPoint<T>(point.get_key(), typed_value));
    }

    return datapoints;
  };
};
