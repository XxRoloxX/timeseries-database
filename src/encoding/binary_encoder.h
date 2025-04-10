#include "../datapoints/data_point.h"
#include "../query/parser.h"
#include "../typed_series/typed_datapoint.h"
#include "./decoder.h"
#include "./encoder.h"
#include <memory>
#include <vector>

class BinaryDataPointDecoder : public Decoder {
public:
  DataPoint decode(std::shared_ptr<std::vector<char>> data) override {
    DataPoint data_point;
    data_point.decode(data);
    return data_point;
  };
  std::vector<DataPoint>
  decode_many(std::shared_ptr<std::vector<char>> data) override {

    size_t iter = 0;
    std::vector<DataPoint> result;
    while (iter < data->size()) {

      std::shared_ptr<std::vector<char>> slice =
          std::make_shared<std::vector<char>>(data->begin() + iter,
                                              data->end());

      DataPoint data_point;
      auto read = data_point.decode(slice);

      iter += read;
      result.push_back(data_point);
    }

    return result;
  };
};

class BinaryDataPointEncoder : public Encoder {
public:
  EncodedBuffer encode(DataPoint &data) override { return data.encode(); };
  EncodedBuffer
  encode_many(std::shared_ptr<std::vector<DataPoint>> data) override {
    EncodedBuffer res;

    for (auto &point : *data) {
      auto encoded_point = point.encode();
      res.insert(res.end(), encoded_point.begin(), encoded_point.end());
    }

    return res;
  };
};
class PlaintextDataPointEncoder : public Encoder {
private:
  DataType data_type;

public:
  PlaintextDataPointEncoder(DataType datatype) : data_type(datatype) {};
  EncodedBuffer encode(DataPoint &point) override {

    if (data_type == DataType::INT) {
      TypedDataPoint<int> typed_datapoint(point);
      auto encoded_datapoint = typed_datapoint.to_string();

      return EncodedBuffer(encoded_datapoint.begin(), encoded_datapoint.end());

    } else if (data_type == DataType::FLOAT) {
      TypedDataPoint<float> typed_datapoint(point);
      auto encoded_datapoint = typed_datapoint.to_string();

      return EncodedBuffer(encoded_datapoint.begin(), encoded_datapoint.end());

    } else {
      auto encoded_point = point.to_string();
      return EncodedBuffer(encoded_point.begin(), encoded_point.end());
    }
  };
  EncodedBuffer
  encode_many(std::shared_ptr<std::vector<DataPoint>> data) override {
    EncodedBuffer res;

    for (auto &point : *data) {
      auto encoded = this->encode(point);
      encoded.push_back('\n');
      res.insert(res.end(), encoded.begin(), encoded.end());
    }

    return res;
  };
};
