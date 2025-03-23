#include "./encoding/binary_encoder.h"
#include "./indexing/indexed_ss_table.h"
#include <memory>

int main() {

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();
  std::shared_ptr<SSTableStorage> raw_table =
      std::make_shared<SSTableStorage>(logger, "table1");

  std::shared_ptr<Decoder<int>> decoder =
      std::make_shared<DataPointDecoder<int>>();
  std::shared_ptr<Encoder<int>> encoder =
      std::make_shared<DataPointEncoder<int>>();

  auto indexed_table = std::make_shared<IndexedSSTable<int>>(logger, raw_table,
                                                             decoder, encoder);

  std::vector<DataPoint<int>> data = {
      DataPoint<int>(12, 12),
      DataPoint<int>(13, 14),
  };

  // indexed_table->load_data(data);

  // indexed_table->save();

  std::shared_ptr<SSTableStorage> read_table =
      std::make_shared<SSTableStorage>(logger, "table1");

  auto generic_table_2 = std::make_shared<IndexedSSTable<int>>(
      logger, raw_table, decoder, encoder);

  // generic_table_2->read();
}
