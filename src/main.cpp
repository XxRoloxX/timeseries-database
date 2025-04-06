#include "./encoding/binary_encoder.h"
#include "./indexing/ss_table_indexer.h"
#include "./storage/storage_manager.h"
#include "./utils.h"
#include "database.h"
#include "datapoints/data_point.h"
#include "typed_series/typed_series.h"
#include <iostream>
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
  auto indexer = std::make_shared<SSTableIndexer>(logger, decoder, encoder);

  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache =
      std::make_shared<MemTable<DataPointKey, DataPoint>>(logger);

  std::shared_ptr<StorageManager> storage_manager =
      std::make_shared<StorageManager>(logger, decoder, encoder, cache, indexer,
                                       "storage");

  std::shared_ptr<Database> database = std::make_shared<Database>(
      logger, decoder, encoder, 3, indexer, storage_manager);

  TypedSeries<int> int_series("typed_test_2", database);

  for (std::string line; std::getline(std::cin, line);) {
    auto tokens = split(line, " ");

    if (tokens.size() == 0) {
      return 0;
    }

    auto operation = tokens[0];

    if (operation == "r") {

      if (tokens.size() != 3) {
        return 0;
      }

      auto key = stoi(tokens[1], 0, 10);
      auto value = stoi(tokens[2], 0, 10);
      auto read_points = int_series.read(key, value);

      std::cout << std::format("reading: {}-{}", key, value) << std::endl;

      for (auto &point : read_points) {
        std::cout << (point.to_string()) << std::endl;
      }

    } else if (operation == "i") {

      if (tokens.size() != 3) {
        return 0;
      }

      auto key = stoi(tokens[1], 0, 10);
      int value = stoi(tokens[2], 0, 10);

      int_series.insert(key, value);
      std::cout << (std::format("inserted: {}, {}", key, value)) << std::endl;
      ;
    } else if (operation == "c") {

      auto read_points = cache->get_all("typed_test");
      std::cout << "cache contents: " << std::endl;

      for (auto &point : read_points) {
        std::cout << (point.to_string()) << std::endl;
      }

    } else {
      return 0;
    }
  }
}
