#include "ss_table.h"
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <variant>

RawSSTable::RawSSTable(std::shared_ptr<Logger> logger, std::string name)
    : logger(logger), name(std::move(name)) {

  this->logger->info(std::format("created reader for {}", this->name));
}
void read();
std::vector<char> load_table_range(const std::string &filePath,
                                   std::size_t startByte, std::size_t endByte);

void RawSSTable::load(std::vector<char> *raw) {

  // We start reading the blob with metadata block.
  Result<int> read_bytes = metadata.load(raw);
  if (auto ptr = std::get_if<Error>(&read_bytes)) {
    this->logger->error(
        std::format("failed to load metadata block: {}", ptr->message));
    return;
  }

  int data_offset = std::get<int>(read_bytes);

  std::vector<char> new_data(raw->begin() + data_offset,
                             raw->begin() +
                                 metadata.get_indexes_block_offset());

  std::vector<char> new_indices(
      raw->begin() + metadata.get_indexes_block_offset(), raw->end());

  this->data = std::move(new_data);
  this->indices = std::move(new_indices);
}

void RawSSTable::write() {
  std::ofstream file(this->name, std::ios::binary);
  if (!file) {
    this->logger->error("failed to open a file: " + this->name);
    return;
  }
  this->logger->info(std::format("writing to file: {}, size: {}", this->name,
                                 this->data.size()));

  this->update_metadata();

  std::vector<char> encoded_metadata = this->metadata.encode();
  file.write(encoded_metadata.data(), encoded_metadata.size());
  file.write(this->data.data(), this->data.size());
  file.write(this->indices.data(), this->indices.size());
};

void RawSSTable::update_metadata() {
  this->metadata.set_indexes_block_offset(this->data.size() +
                                          METADATA_BLOCK_LENGTH_BYTES);
}

void RawSSTable::read() {

  std::ifstream file(this->name, std::ios::in | std::ofstream::binary);
  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    return;
  }

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();

  this->logger->info(std::format("reading file of size: {}", size));
  file.seekg(0, std::ios::beg);

  std::vector<char> file_data(size);
  if (!file.read(this->data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  this->load(&file_data);
}
std::shared_ptr<std::vector<char>> RawSSTable::get_data() {
  return std::make_shared<std::vector<char>>(this->data);
}

std::shared_ptr<std::vector<char>> RawSSTable::get_indices() {
  return std::make_shared<std::vector<char>>(this->indices);
}
