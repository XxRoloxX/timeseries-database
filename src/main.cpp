#include "./encoding/binary_encoder.h"
#include "./indexing/indexed_ss_table_reader.h"
#include "./indexing/indexed_ss_table_writer.h"
#include <memory>

int main() {

  std::shared_ptr<Decoder<int>> decoder =
      std::make_shared<DataPointDecoder<int>>();
  std::shared_ptr<Encoder<int>> encoder =
      std::make_shared<DataPointEncoder<int>>();

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint<int>> data = {
      DataPoint<int>(12, 12), DataPoint<int>(13, 14), DataPoint<int>(13, 14),
      DataPoint<int>(13, 14), DataPoint<int>(13, 14), DataPoint<int>(17, 14),
  };

  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter<int>>(logger, decoder, encoder);

  ss_table_writer->save("table1",
                        std::make_shared<std::vector<DataPoint<int>>>(data));

  std::shared_ptr<SSTableStorage> raw_table =
      std::make_shared<SSTableStorage>(logger, "table1");

  raw_table->initialize();

  auto indexed_table = std::make_shared<IndexedSSTableReader<int>>(
      logger, raw_table, decoder, encoder);

  // indexed_table->get_data();
  indexed_table->initialize();

  auto datapoints = indexed_table->read_range(12, 17);

  logger->info("RESULTS");
  for (auto &point : *datapoints) {
    logger->info(point.to_string());
  }
}
