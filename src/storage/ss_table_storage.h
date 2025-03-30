#pragma once

#include "../logger/logger.h"
#include "./metadata_block.h"
#include <memory>
#include <string>

const std::string STORAGE_PATH = "storage";

class SSTableStorage {
private:
  EncodedBuffer raw_metadata;
  EncodedBuffer data;
  EncodedBuffer indices;
  std::shared_ptr<Logger> logger;
  std::string name;
  MetadataBlock metadata;

  void load(EncodedBuffer *raw);

public:
  SSTableStorage(std::shared_ptr<Logger> logger, std::string name);
  void initialize();
  std::shared_ptr<EncodedBuffer> read_range(std::size_t start_byte,
                                            std::size_t end_byte);

  std::shared_ptr<EncodedBuffer> read_all();
  void merge_with(std::shared_ptr<SSTableStorage> table) {};
  void set_data(EncodedBuffer data);
  void set_indexes(EncodedBuffer indexes);
  void update_metadata();

  void save();
  std::shared_ptr<EncodedBuffer> get_data();
  std::shared_ptr<EncodedBuffer> get_indexes();
  std::string get_name();
};
