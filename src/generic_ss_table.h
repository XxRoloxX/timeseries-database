#include "decoder.h"
#include "encoder.h"
#include "logger.h"
#include "ss_table.h"
#include <memory>
#include <string>
#include <vector>

template <class K> class GenericSSTable {
private:
  std::shared_ptr<Logger> logger;
  std::shared_ptr<RawSSTable> raw_table;
  std::vector<K> data;
  std::shared_ptr<Decoder<K>> decoder;
  std::shared_ptr<Encoder<K>> encoder;

public:
  GenericSSTable(std::shared_ptr<Logger> logger,
                 std::shared_ptr<RawSSTable> raw_table,
                 std::shared_ptr<Decoder<K>> decoder)
      : logger(logger), raw_table(raw_table), decoder(decoder) {};
  void read() {
    this->raw_table->read();

    auto raw_data = this->raw_table->get_data();

    auto data_points = this->decoder->decode_many(raw_data);
    this->data = data_points;
  };
  std::vector<char> load_table_range(const std::string &filePath,
                                     std::size_t startByte,
                                     std::size_t endByte) {
    return {};
  };
  void merge_with(std::shared_ptr<RawSSTable> table) {};
  void load_data(std::vector<K> new_data) { this->data = new_data; };
  void write() {
    auto raw_data = this->encoder->encode_many(this->data);
    this->raw_table->write(raw_data);
  };

  std::shared_ptr<std::vector<K>> get_data() {
    return std::make_shared<std::vector<K>>(this->data);
  }
};
