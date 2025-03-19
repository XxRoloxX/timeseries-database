#pragma once

#include "data_point.h"
#include "indexes_block.h"
#include "logger.h"
#include "metadata_block.h"
#include <memory>
#include <string>
#include <vector>

const int METADATA_BLOCK_LENGTH_BYTES = 100;

class RawSSTable {
private:
  std::vector<char> raw_metadata;
  std::vector<char> data;
  std::vector<char> indices;
  std::shared_ptr<Logger> logger;
  std::string name;
  MetadataBlock metadata;
  IndexesMetadataBlock indexes;

public:
  RawSSTable(std::shared_ptr<Logger> logger, std::string name);
  void read();
  Result<std::vector<char>> load_range(std::size_t startByte,
                                       std::size_t endByte);

  void merge_with(std::shared_ptr<RawSSTable> table) {};
  void load(std::vector<char> *raw);
  void set_data(std::vector<char> data);
  void set_indexes(std::vector<char> indexes);
  void update_metadata();
  // void load_indices(std::vector<char> new_indices);
  void write();
  std::shared_ptr<std::vector<char>> get_data();
  std::shared_ptr<std::vector<char>> get_indexes();
};
