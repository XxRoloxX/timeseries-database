#include "data_point.h"
#include "decoder.h"
#include "encoder.h"
#include "error.h"
#include <vector>

template <typename T> class DataPointDecoder : Decoder<DataPoint<T>> {

  Result<DataPoint<T>> decode(std::vector<char> data) override {
    DataPoint<T> data_point;
    data_point.decode(data);
    return data_point;
  };
  Result<std::vector<DataPoint<T>>>
  decode_many(std::vector<char> data) override {
    auto iter = 0;
    std::vector<DataPoint<T>> result;
    while (iter < data.size()) {
      DataPoint<T> data_point;
      auto read = data_point.decode(data);
      iter += read;
      result.push(data_point);
    }

    return result;
  };
};

template <typename T> class DataPointEncoder : Encoder<DataPoint<T>> {

  Result<std::vector<char>>
  encode(std::shared_ptr<DataPoint<T>> data) override {
    return data.encode();
  };
  Result<std::vector<char>>
  encode_many(std::shared_ptr<std::vector<T>> data) override {
    std::vector<char> res;

    for (auto point : data) {
      auto encoded_point = point.encode();
      res.insert(res.end(), encoded_point.begin(), encoded_point.end());
    }

    return res;
  };
};
