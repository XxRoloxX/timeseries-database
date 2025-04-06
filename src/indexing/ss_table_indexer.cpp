#include "./ss_table_indexer.h"
#include "indexes_block.h"
#include <memory>

IndexesMetadataBlock SSTableIndexer::create_indexes(
    std::shared_ptr<std::vector<DataPoint>> datapoints) {

  std::shared_ptr<std::vector<IndexMapping>> indexes =
      std::make_shared<std::vector<IndexMapping>>();

  size_t current_offset = 0;

  for (auto &datapoint : *datapoints) {
    auto encoded_datapoint =
        this->encoder->encode(std::make_shared<DataPoint>(datapoint));

    indexes->push_back(IndexMapping{.key = datapoint.get_key(),
                                    .offset = current_offset,
                                    .length = encoded_datapoint.size()});

    current_offset += encoded_datapoint.size();
  }

  return IndexesMetadataBlock(indexes);
}

SSTableIndexer::SSTableIndexer(std::shared_ptr<Logger> logger,
                               std::shared_ptr<Decoder> decoder,
                               std::shared_ptr<Encoder> encoder)
    : logger(logger), decoder(decoder), encoder(encoder) {}

SSTableIndexer::~SSTableIndexer() = default;
