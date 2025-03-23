#include "../datapoints/data_point.h"
#include "./decoder.h"
#include "./encoder.h"
#include <memory>
#include <vector>

template <typename T> class DataPointDecoder : public Decoder<T> {
public:
  DataPoint<T> decode(std::shared_ptr<std::vector<char>> data) override {
    DataPoint<T> data_point;
    data_point.decode(data);
    return data_point;
  };
  std::vector<DataPoint<T>>
  decode_many(std::shared_ptr<std::vector<char>> data) override {
    auto iter = 0;
    std::vector<DataPoint<T>> result;
    while (iter < data->size()) {

      std::shared_ptr<std::vector<char>> slice =
          std::make_shared<std::vector<char>>(data->begin() + iter,
                                              data->end());

      DataPoint<T> data_point;
      auto read = data_point.decode(slice);

      iter += read;
      result.push_back(data_point);
    }

    return result;
  };
};

template <typename T> class DataPointEncoder : public Encoder<T> {
public:
  EncodedBuffer encode(std::shared_ptr<DataPoint<T>> data) override {
    return data->encode();
  };
  EncodedBuffer
  encode_many(std::shared_ptr<std::vector<DataPoint<T>>> data) override {
    EncodedBuffer res;

    for (auto &point : *data) {
      auto encoded_point = point.encode();
      res.insert(res.end(), encoded_point.begin(), encoded_point.end());
    }

    return res;
  };
};
