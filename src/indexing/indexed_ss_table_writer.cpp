#include "./indexed_ss_table_writer.h"
#include "../storage/ss_table_storage.h"
#include "indexes_block.h"
#include <memory>

void IndexedSSTableWriter::save(
    std::string name, std::shared_ptr<std::vector<DataPoint>> datapoints) {

  std::vector<IndexMapping> indexes;

  size_t current_offset = 0;

  for (auto &datapoint : *datapoints) {
    auto encoded_datapoint =
        this->encoder->encode(std::make_shared<DataPoint>(datapoint));

    indexes.push_back(IndexMapping{.key = datapoint.get_key(),
                                   .offset = current_offset,
                                   .length = encoded_datapoint.size()});

    current_offset += encoded_datapoint.size();
  }

  auto encoded_datapoints = this->encoder->encode_many(datapoints);

  SSTableStorage storage(this->logger, name);
  IndexesMetadataBlock indexes_block(indexes);

  auto encoded_indexes = indexes_block.encode();

  storage.set_data(encoded_datapoints);
  storage.set_indexes(encoded_indexes);

  this->logger->info(
      std::format("saving {} datapoints to {}", datapoints->size(), name));

  this->logger->info(
      std::format("saving {} indexes to {}", indexes.size(), name));
  storage.save();
}

IndexedSSTableWriter::IndexedSSTableWriter(std::shared_ptr<Logger> logger,
                                           std::shared_ptr<Decoder> decoder,
                                           std::shared_ptr<Encoder> encoder)
    : logger(logger), decoder(decoder), encoder(encoder) {}

IndexedSSTableWriter::~IndexedSSTableWriter() = default;
