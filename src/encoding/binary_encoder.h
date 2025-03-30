#include "../datapoints/data_point.h"
#include "./decoder.h"
#include "./encoder.h"
#include <memory>
#include <vector>

class DataPointDecoder : public Decoder {
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

class DataPointEncoder : public Encoder {
public:
  EncodedBuffer encode(std::shared_ptr<DataPoint> data) override {
    return data->encode();
  };
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
