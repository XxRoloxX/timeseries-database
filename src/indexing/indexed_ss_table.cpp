#include "./indexed_ss_table.h"
#include <memory>

template <typename K>
IndexedSSTable<K>::IndexedSSTable(std::shared_ptr<Logger> logger,
                                  std::shared_ptr<SSTableStorage> raw_table,
                                  std::shared_ptr<Decoder<K>> decoder,
                                  std::shared_ptr<Encoder<K>> encoder)
    : logger(logger), raw_table(raw_table), decoder(decoder), encoder(encoder) {
}

template <typename K> IndexedSSTable<K>::~IndexedSSTable() = default;

template <typename K> void IndexedSSTable<K>::initialize() {
  this->raw_table->initialize();

  this->name = this->raw_table->get_name();

  auto raw_indexes = this->raw_table->get_indexes();
  this->indexes.decode(*raw_indexes);

  this->logger->info(std::format("initializes ss_table: {}", this->name));
}

template <typename K>
std::shared_ptr<std::vector<DataPoint<K>>>
IndexedSSTable<K>::read_range(DataPointKey start_key, DataPointKey end_key) {
  auto results = this->indexes.index_range(start_key, end_key);

  try {
    auto datapoints_result = this->raw_table->read_range(
        results.start_byte_offset, results.end_byte_offset);

    std::vector<DataPoint<K>> datapoints =
        this->decoder->decode_many(datapoints_result);

    for (auto &point : datapoints) {
      this->logger->info(point.to_string());
    }

    return std::make_shared<std::vector<DataPoint<K>>>(datapoints);
  } catch (const std::runtime_error &err) {
    this->logger->error(std::format("{} failed to read range from: {}-{}]",
                                    err.what(), results.start_byte_offset,
                                    results.end_byte_offset));
    return {};
  }
}

// template <typename K> void IndexedSSTable<K>::save() {
//   auto encoded_points = this->encoder->encode_many(
//       std::make_shared<std::vector<DataPoint<K>>>(this->data));
//
//   this->logger->info(
//       std::format("loaded data points of size: {}", encoded_points.size()));
//   this->raw_table->set_data(encoded_points);
//
//   this->raw_table->save();
// }

template <typename K>
std::shared_ptr<std::vector<DataPoint<K>>> IndexedSSTable<K>::get_data() {
  return std::make_shared<std::vector<DataPoint<K>>>(this->data);
}

// Explicit template instantiations
template class IndexedSSTable<int>;
// template class IndexedSSTable<std::string>;
