#include "data_point.h"
#include "decoder.h"
#include "encoder.h"
#include "indexes_block.h"
#include "logger.h"
#include "ss_table.h"
#include <format>
#include <memory>
#include <string>
#include <variant>
#include <vector>

template <typename K> class GenericSSTable {
private:
  std::shared_ptr<Logger> logger;
  std::shared_ptr<RawSSTable> raw_table;
  std::vector<DataPoint<K>> data;
  std::shared_ptr<Decoder<K>> decoder;
  std::shared_ptr<Encoder<K>> encoder;
  IndexesMetadataBlock indexes;

public:
  GenericSSTable(std::shared_ptr<Logger> logger,
                 std::shared_ptr<RawSSTable> raw_table,
                 std::shared_ptr<Decoder<K>> decoder,
                 std::shared_ptr<Encoder<K>> encoder)
      : logger(logger), raw_table(raw_table), decoder(decoder),
        encoder(encoder) {};
  ~GenericSSTable() = default;
  void read() {
    this->raw_table->read();

    auto raw_indexes = this->raw_table->get_indexes();
    this->indexes.decode(*raw_indexes);

    auto raw_data = this->raw_table->get_data();

    this->logger->info(
        std::format("read raw data points of length {}", raw_data->size()));

    auto data_points = this->decoder->decode_many(raw_data);
    auto decoded_points = std::get_if<std::vector<DataPoint<K>>>(&data_points);
    if (!decoded_points) {
      Error error = std::get<Error>(data_points);
      this->logger->error(
          std::format("failed to decode data: {}", error.message));
      return;
    }

    this->logger->info(
        std::format("read data points of length: {}", decoded_points->size()));

    this->data = std::move(*decoded_points);

    for (auto &point : data) {
      this->logger->info(point.to_string());
    }
  };
  std::vector<char> load_table_range(const std::string &filePath,
                                     std::size_t startByte,
                                     std::size_t endByte) {
    return {};
  };
  void merge_with(std::shared_ptr<RawSSTable> table) {};
  void load_data(std::vector<DataPoint<K>> new_data) { this->data = new_data; };
  void write() {
    auto raw_data = this->encoder->encode_many(
        std::make_shared<std::vector<DataPoint<K>>>(this->data));

    if (auto ptr = std::get_if<std::vector<char>>(&raw_data)) {
      std::vector<char> data_points = std::move(*ptr);
      this->logger->info(
          std::format("loaded data points of size: {}", data_points.size()));
      this->raw_table->load(&data_points);
      this->raw_table->write();
    } else {
      this->logger->error("failed to load data");
    }
  };

  std::shared_ptr<std::vector<K>> get_data() {
    return std::make_shared<std::vector<K>>(this->data);
  }
};
