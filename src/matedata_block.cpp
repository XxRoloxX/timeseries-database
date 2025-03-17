#include "error.h"
#include "metadata_block.h"
#include <cstring>
#include <format>
#include <vector>

Result<int> MetadataBlock::load(std::vector<char> *data) {
  if (data->size() < INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES) {
    return Error(std::format("data passed to metadata block is too short: {}",
                             data->size()));
  }

  int index_offset = 0;
  std::memcpy(&index_offset, data->data(),
              INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES);

  this->indexes_block_offset = index_offset;
  return INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES;
}
std::vector<char> MetadataBlock::encode() {
  std::vector<char> encoded_data(sizeof(int));
  std::memcpy(encoded_data.data(), &this->indexes_block_offset, sizeof(int));

  return encoded_data;
}

int MetadataBlock::get_indexes_block_offset() {
  return this->indexes_block_offset;
}

void MetadataBlock::set_indexes_block_offset(int offset) {
  this->indexes_block_offset = offset;
}
