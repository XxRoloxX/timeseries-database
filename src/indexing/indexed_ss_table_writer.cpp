#include "./indexed_ss_table_writer.h"
#include "../storage/ss_table_storage.h"
#include "indexes_block.h"
#include <memory>

template <typename K>
void IndexedSSTableWriter<K>::save(
    std::string name, std::shared_ptr<std::vector<DataPoint<K>>> datapoints) {

  std::vector<IndexMapping> indexes;

  int current_offset = 0;

  for (auto &datapoint : *datapoints) {
    auto encoded_datapoint =
        this->encoder->encode(std::make_shared<DataPoint<K>>(datapoint));

    current_offset += encoded_datapoint.size();

    indexes.push_back(
        IndexMapping{.key = datapoint.get_key(), .offset = current_offset});
  }

  auto encoded_datapoints = this->encoder->encode_many(datapoints);

  SSTableStorage storage(this->logger, name);
  IndexesMetadataBlock indexes_block(indexes);

  auto encoded_indexes = indexes_block.encode();

  storage.set_data(encoded_datapoints);
  storage.set_indexes(encoded_indexes);

  this->logger->info(
      std::format("saving {} datapoints to {}", datapoints->size(), name));
  storage.save();
}

template <typename K>
IndexedSSTableWriter<K>::IndexedSSTableWriter(
    std::shared_ptr<Logger> logger, std::shared_ptr<Decoder<K>> decoder,
    std::shared_ptr<Encoder<K>> encoder)
    : logger(logger), decoder(decoder), encoder(encoder) {}

template <typename K>
IndexedSSTableWriter<K>::~IndexedSSTableWriter() = default;

template class IndexedSSTableWriter<int>;
