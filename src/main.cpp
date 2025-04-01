#include "./encoding/binary_encoder.h"
#include "./indexing/indexed_ss_table_writer.h"
#include "database.h"
#include <iostream>
#include <memory>
#include <ranges>
#include <unistd.h>

std::vector<std::string> split(std::string input, std::string delimiter) {

  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;

  while ((pos = input.find(delimiter)) != std::string::npos) {
    token = input.substr(0, pos);
    tokens.push_back(token);
    input.erase(0, pos + 1);
  }

  tokens.push_back(input);

  return tokens;
}

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

  Database database =
      Database(logger, decoder, encoder, 3, cache, ss_table_writer);

  for (std::string line; std::getline(std::cin, line);) {
    auto tokens = split(line, " ");
    if (tokens.size() != 3) {
      return 0;
    }
    auto operation = tokens[0];
    auto key = stoi(tokens[1], 0, 10);
    auto value = stoi(tokens[2], 0, 10);

    if (operation == "r") {
      auto read_points = database.read(key, value);

      std::cout << std::format("reading: {}-{}", key, value) << std::endl;

      for (auto &point : read_points) {
        std::cout << (point.to_string()) << std::endl;
      }

    } else if (operation == "i") {
      database.insert("cli", key, std::vector{char(value)});
      std::cout << (std::format("inserted: {}, {}", key, value)) << std::endl;
      ;
    } else {
      return 0;
    }
  }
}
