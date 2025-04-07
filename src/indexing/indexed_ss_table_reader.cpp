#include "./indexed_ss_table_reader.h"
#include <format>
#include <memory>
#include <stdexcept>

IndexedSSTableReader::IndexedSSTableReader(
    std::shared_ptr<Logger> logger, std::shared_ptr<SSTableStorage> raw_table,
    std::shared_ptr<Decoder> decoder, std::shared_ptr<Encoder> encoder)
    : logger(logger), raw_table(raw_table), decoder(decoder), encoder(encoder) {

  indexes = std::make_shared<IndexesMetadataBlock>();
}

IndexedSSTableReader::~IndexedSSTableReader() = default;

void IndexedSSTableReader::initialize() {

  this->raw_table->initialize();

  auto raw_indexes = this->raw_table->get_indexes();

  this->indexes->decode(*raw_indexes);

  this->logger->info(
      std::format("initialized ss_table: {}", this->raw_table->get_name()));
}

std::string IndexedSSTableReader::get_name() {
  return this->raw_table->get_name();
}

std::shared_ptr<std::vector<DataPoint>>
IndexedSSTableReader::read_range(DataPointKey start_key, DataPointKey end_key) {

  auto results = this->indexes->index_range(start_key, end_key);

  std::vector<DataPoint> filtered_datapoints;

  try {
    auto datapoints_result = this->raw_table->read_range(
        results.start_byte_offset, results.end_byte_offset);

    std::vector<DataPoint> datapoints =
        this->decoder->decode_many(datapoints_result);

    for (DataPoint &point : datapoints) {
      if (point.get_key() >= start_key && point.get_key() <= end_key) {
        filtered_datapoints.push_back(point);
        this->logger->info(point.to_string());
      }
    }

    return std::make_shared<std::vector<DataPoint>>(filtered_datapoints);
  } catch (const std::runtime_error &err) {
    this->logger->error(std::format("{} failed to read range from: {}-{}]",
                                    err.what(), results.start_byte_offset,
                                    results.end_byte_offset));
    return {};
  }
}

std::shared_ptr<std::vector<DataPoint>> IndexedSSTableReader::read_all() {

  auto encoded_points = this->raw_table->read_all();
  auto decoded_points = this->decoder->decode_many(encoded_points);

  return std::make_shared<std::vector<DataPoint>>(decoded_points);
}

std::shared_ptr<IndexesMetadataBlock> IndexedSSTableReader::get_indexes() {
  return this->indexes;
}

std::string IndexedSSTableReader::get_series() {
  return this->raw_table->get_series();
}

void IndexedSSTableReader::remove() { this->raw_table->remove(); }
