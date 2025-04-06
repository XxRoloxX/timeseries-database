#include "./encoding/binary_encoder.h"
#include "./indexing/indexed_ss_table_writer.h"
#include "./storage/storage_manager.h"
#include "./utils.h"
#include "database.h"
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
  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter>(logger, decoder, encoder);

  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache =
      std::make_shared<MemTable<DataPointKey, DataPoint>>(logger);

  std::shared_ptr<StorageManager> storage_manager =
      std::make_shared<StorageManager>(logger, decoder, encoder, cache,
                                       ss_table_writer, "storage");

  Database database(logger, decoder, encoder, 3, ss_table_writer,
                    storage_manager);

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
      auto read_points = database.read("cli", key, value);

      std::cout << std::format("reading: {}-{}", key, value) << std::endl;

      for (auto &point : read_points) {
        std::cout << (point.to_string()) << std::endl;
      }

    } else if (operation == "i") {

      if (tokens.size() != 3) {
        return 0;
      }

      auto key = stoi(tokens[1], 0, 10);
      auto value = stoi(tokens[2], 0, 10);

      database.insert("cli", key, std::vector{char(value)});
      std::cout << (std::format("inserted: {}, {}", key, value)) << std::endl;
      ;
    } else if (operation == "c") {

      auto read_points = cache->get_all();
      std::cout << "cache contents: " << std::endl;

      for (auto &point : read_points) {
        std::cout << (point.to_string()) << std::endl;
      }

    } else {
      return 0;
    }
  }
}
