#include "ss_table.h"
#include <fstream>
#include <iostream>
#include <memory>

RawSSTable::RawSSTable(std::shared_ptr<Logger> logger, std::string name)
    : logger(logger), name(std::move(name)) {}
void read();
std::vector<char> load_table_range(const std::string &filePath,
                                   std::size_t startByte, std::size_t endByte);

void RawSSTable::load_data(std::vector<char> new_data) {
  data = std::move(new_data);
}

void RawSSTable::write() {
  std::ofstream file(this->name, std::ios::binary);
  if (!file) {
    this->logger->error("failed to open a file: " + this->name);
    return;
  }

  file.write(this->data.data(), this->data.size());
};

void RawSSTable::read() {

  std::ifstream file(this->name, std::ios::in | std::ofstream::binary);
  if (!file) {
    this->logger->error("failed to read file: " + this->name);
    return;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  this->data.resize(size);
  if (!file.read(this->data.data(), size)) {
    this->logger->error("failed to load table from file");
  }
}
std::shared_ptr<std::vector<char>> RawSSTable::get_data() {
  return std::make_shared<std::vector<char>>(this->data);
}
