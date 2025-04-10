#include "./encoding/binary_encoder.h"
#include "./indexing/ss_table_indexer.h"
#include "./storage/storage_manager.h"
#include "./utils.h"
#include "database.h"
#include "datapoints/data_point.h"
#include "query/parser.h"
#include "server/server.h"
#include "typed_series/typed_series.h"
#include <csignal>
#include <cstdlib>
#include <memory>
#include <unistd.h>

int main() {
  std::shared_ptr<Decoder> decoder = std::make_shared<BinaryDataPointDecoder>();
  std::shared_ptr<Encoder> encoder = std::make_shared<BinaryDataPointEncoder>();
  std::unordered_map<DataType, std::shared_ptr<Encoder>> response_encoders = {
      {DataType::INT,
       std::make_shared<PlaintextDataPointEncoder>(DataType::INT)},
      {DataType::FLOAT,
       std::make_shared<PlaintextDataPointEncoder>(DataType::FLOAT)},
      {DataType::BINARY,
       std::make_shared<PlaintextDataPointEncoder>(DataType::BINARY)}};

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint> data = {
      DataPoint(20, std::vector<char>{19}),
      DataPoint(21, std::vector<char>{19}),
  };
  auto indexer = std::make_shared<SSTableIndexer>(logger, decoder, encoder, 2);

  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache =
      std::make_shared<MemTable<DataPointKey, DataPoint>>(logger);

  std::shared_ptr<StorageManager> storage_manager =
      std::make_shared<StorageManager>(logger, decoder, encoder, cache, indexer,
                                       "storage");

  std::shared_ptr<Database> database = std::make_shared<Database>(
      logger, decoder, encoder, 150, indexer, storage_manager);

  std::shared_ptr<QueryParser> query_parser =
      std::make_shared<PlaintextQueryParser>(logger);

  TcpServer server(logger, database, query_parser, response_encoders, 9002,
                   1000);

  server.initialize();

  server.listen();
}
