#include "./ss_table_indexer.h"
#include "indexes_block.h"
#include <memory>

IndexesMetadataBlock SSTableIndexer::create_indexes(
    std::shared_ptr<std::vector<DataPoint>> datapoints) {

  std::shared_ptr<std::vector<IndexMapping>> indexes =
      std::make_shared<std::vector<IndexMapping>>();

  size_t current_offset = 0;

  for (int i = 0; i < datapoints->size(); i++) {

    auto encoded_datapoint = this->encoder->encode(datapoints->at(i));

    // If index is neither last nor first we check the ratio.
    if (i == 0 || i == (datapoints->size() - 1) || i % ratio == 0) {

      indexes->push_back(IndexMapping{.key = datapoints->at(i).get_key(),
                                      .offset = current_offset,
                                      .length = encoded_datapoint.size()});
    }

    current_offset += encoded_datapoint.size();
  }

  return IndexesMetadataBlock(indexes);
}

SSTableIndexer::SSTableIndexer(std::shared_ptr<Logger> logger,
                               std::shared_ptr<Decoder> decoder,
                               std::shared_ptr<Encoder> encoder, int ratio)
    : logger(logger), decoder(decoder), encoder(encoder), ratio(ratio) {}

SSTableIndexer::~SSTableIndexer() = default;
