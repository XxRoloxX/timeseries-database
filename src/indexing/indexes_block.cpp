#include "./indexes_block.h"
#include <cstring>
#include <vector>

bool IndexMapping::operator>(const IndexMapping &f) const {
  return key > f.key;
}

bool IndexMapping::operator<(const IndexMapping &f) const {
  return key < f.key;
}

bool IndexMapping::operator==(const IndexMapping &f) const {
  return key == f.key;
}

IndexMapping find_closest_index(std::vector<IndexMapping> *elements,
                                DataPointKey searched_value) {
  std::size_t min_idx = 0;
  std::size_t max_idx = elements->size() - 1;
  std::size_t mid = (max_idx + min_idx) / 2;

  while (min_idx < max_idx) {

    mid = (max_idx + min_idx) / 2;

    if ((*elements)[mid].key == searched_value) {
      return (*elements)[mid];
    }

    if ((*elements)[mid].key < searched_value) {
      min_idx = mid;
      continue;
    }

    if ((*elements)[mid].key > searched_value) {
      max_idx = mid;
      continue;
    }
  }

  return (*elements)[mid];
};

IndexesMetadataBlock::IndexesMetadataBlock() = default;
IndexesMetadataBlock::IndexesMetadataBlock(std::vector<IndexMapping> mappings)
    : indexes(mappings) {}

IndexResult IndexesMetadataBlock::index_range(DataPointKey start_key,
                                              DataPointKey end_key) {

  if (indexes.size() == 0) {
    throw std::runtime_error("indexes not loaded");
  }

  IndexMapping start = find_closest_index(&indexes, start_key);
  IndexMapping end = find_closest_index(&indexes, start_key);

  IndexResult result{.start_byte_offset = start.offset,
                     .end_byte_offset = end.offset};
  return result;
}

std::size_t IndexesMetadataBlock::size() { return this->indexes.size(); }

void IndexesMetadataBlock::decode(EncodedBuffer &data) {

  size_t mapping_size = sizeof(IndexMapping);

  for (size_t i = 0; i < data.size() / mapping_size; i++) {
    IndexMapping index;
    std::memcpy(&index, data.data() + i * mapping_size, mapping_size);
    this->indexes.push_back(index);
  }
}
EncodedBuffer IndexesMetadataBlock::encode() {
  EncodedBuffer result;

  for (auto &index : this->indexes) {
    EncodedBuffer encoded_index(sizeof(IndexMapping));
    std::memcpy(encoded_index.data(), &index, sizeof(IndexMapping));
    result.insert(result.end(), encoded_index.begin(), encoded_index.end());
  }

  return result;
}
