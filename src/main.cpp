#include "datapoint_decoder.h"
#include "generic_ss_table.h"
#include "ss_table.h"
#include <memory>

int main() {

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();
  std::shared_ptr<RawSSTable> raw_table =
      std::make_shared<RawSSTable>(logger, "table1");

  std::shared_ptr<Decoder<int>> decoder =
      std::make_shared<DataPointDecoder<int>>();
  std::shared_ptr<Encoder<int>> encoder =
      std::make_shared<DataPointEncoder<int>>();

  auto generic_table = std::make_shared<GenericSSTable<int>>(logger, raw_table,
                                                             decoder, encoder);

  std::vector<DataPoint<int>> data = {
      DataPoint<int>(12, 12),
      DataPoint<int>(13, 14),
  };

  generic_table->load_data(data);

  generic_table->write();

  std::shared_ptr<RawSSTable> read_table =
      std::make_shared<RawSSTable>(logger, "table1");

  auto generic_table_2 = std::make_shared<GenericSSTable<int>>(
      logger, raw_table, decoder, encoder);

  generic_table_2->read();
}
