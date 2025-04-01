#include "../storage/ss_table_storage.h"
#include "../logger/logger.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>

SSTableStorage::SSTableStorage(std::shared_ptr<Logger> logger, std::string name)
    : logger(logger), name(std::move(name)) {

  this->logger->info(std::format("created storage for {}", this->name));
}

std::shared_ptr<EncodedBuffer> SSTableStorage::read_range(std::size_t startByte,
                                                          std::size_t endByte) {

  std::ifstream file(STORAGE_PATH + "/" + this->name,
                     std::ios::in | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  file.seekg(startByte + INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES,
             std::ios::beg);

  std::streamsize size = endByte - startByte;

  this->logger->info(
      std::format("reading data of size {} from {}", size, this->name));

  std::vector<char> file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  return std::make_shared<EncodedBuffer>(file_data);
};

std::shared_ptr<EncodedBuffer> SSTableStorage::read_all() {
  std::ifstream file(STORAGE_PATH + "/" + this->name,
                     std::ios::in | std::ofstream::binary);

  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    throw std::filesystem::filesystem_error(
        "file not found: ",
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  file.seekg(0, std::ios::end);

  auto end_pos = file.tellg();

  file.seekg(INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES, std::ios::beg);

  std::streamsize size =
      int(end_pos) - INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES;

  this->logger->info(
      std::format("reading data of size {} from {}", size, this->name));

  std::vector<char> file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  return std::make_shared<EncodedBuffer>(file_data);
}

void SSTableStorage::load(EncodedBuffer *raw) {

  // We start reading the blob with metadata block.
  auto _ = metadata.load(raw);

  std::vector<char> new_indices(
      raw->begin() + metadata.get_indexes_block_offset(), raw->end());

  this->indices = std::move(new_indices);
}

void SSTableStorage::save() {

  if (!std::filesystem::is_directory(STORAGE_PATH)) {
    std::filesystem::create_directory(STORAGE_PATH);
  }

  std::ofstream file(STORAGE_PATH + "/" + this->name, std::ios::binary);
  if (!file) {
    this->logger->error("failed to open a file: " + this->name);
    return;
  }
  this->logger->info(std::format("writing to file: {}, size: {}", this->name,
                                 this->data.size() + this->indices.size()));

  this->update_metadata();

  EncodedBuffer encoded_metadata = this->metadata.encode();
  file.write(encoded_metadata.data(), encoded_metadata.size());
  file.write(this->data.data(), this->data.size());
  file.write(this->indices.data(), this->indices.size());

  this->logger->info(std::format("successfuly written {} bytes to {}",
                                 this->data.size() + this->indices.size(),
                                 this->name));
};

void SSTableStorage::update_metadata() {
  this->metadata.set_indexes_block_offset(
      this->data.size() + INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES);
}

void SSTableStorage::initialize() {

  std::ifstream file(STORAGE_PATH + "/" + this->name,
                     std::ios::in | std::ofstream::binary);
  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    return;
  }

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();

  this->logger->info(std::format("reading file of size: {}", size));
  file.seekg(0, std::ios::beg);

  this->data.resize(size);

  EncodedBuffer file_data(size);
  if (!file.read(file_data.data(), size)) {
    this->logger->error("failed to load table from file");
  }

  this->load(&file_data);

  this->logger->info(std::format("initialized table {}", this->name));
}

std::shared_ptr<EncodedBuffer> SSTableStorage::get_data() {
  return std::make_shared<EncodedBuffer>(this->data);
}

void SSTableStorage::set_data(EncodedBuffer new_data) {
  this->data = std::move(new_data);
}

void SSTableStorage::set_indexes(EncodedBuffer new_indexes) {
  this->indices = std::move(new_indexes);
}

std::shared_ptr<EncodedBuffer> SSTableStorage::get_indexes() {
  return std::make_shared<EncodedBuffer>(this->indices);
}

std::string SSTableStorage::get_name() { return this->name; }
