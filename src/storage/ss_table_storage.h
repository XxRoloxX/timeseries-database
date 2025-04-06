#pragma once

#include "../logger/logger.h"
#include "./metadata_block.h"
#include <memory>
#include <string>

class SSTableStorage {
private:
  EncodedBuffer raw_metadata;
  EncodedBuffer data;
  EncodedBuffer indices;
  std::shared_ptr<Logger> logger;
  std::string name;
  std::string series;
  MetadataBlock metadata;

  void load(EncodedBuffer *raw);
  std::string get_path();
  std::string base_path;

public:
  SSTableStorage(std::shared_ptr<Logger> logger, std::string base_path,
                 std::string name, std::string series);
  void initialize();
  std::shared_ptr<EncodedBuffer> read_range(std::size_t start_byte,
                                            std::size_t end_byte);

  std::shared_ptr<EncodedBuffer> read_all();
  // void merge_with(std::shared_ptr<SSTableStorage> table) {};
  void set_data(EncodedBuffer data);
  void set_indexes(EncodedBuffer indexes);
  void update_metadata(size_t data_size);
  void save();
  void append_save(EncodedBuffer data, EncodedBuffer indexes);
  std::shared_ptr<EncodedBuffer> get_data();
  std::shared_ptr<EncodedBuffer> get_indexes();
  std::string get_name();
  void set_base_path(std::string path);
  std::string get_series();
  void remove();
};
