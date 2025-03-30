#include "./encoding/binary_encoder.h"
#include "./indexing/indexed_ss_table_writer.h"
#include "database.h"
#include <memory>
#include <unistd.h>

int main() {

  std::shared_ptr<Decoder> decoder = std::make_shared<DataPointDecoder>();
  std::shared_ptr<Encoder> encoder = std::make_shared<DataPointEncoder>();

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint> data = {
      DataPoint(20, std::vector<char>{19}),
      DataPoint(21, std::vector<char>{19}),
  };
  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter>(logger, decoder, encoder);

  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache =
      std::make_shared<MemTable<DataPointKey, DataPoint>>();

  Database database =
      Database(logger, decoder, encoder, 3, cache, ss_table_writer);

  auto points = database.read(10, 20);

  // for (auto &point : data) {
  //   logger->info(point.to_string());
  //   database.insert("dupa", point.get_key(), point.get_value());
  //   sleep(1);
  // }
}
