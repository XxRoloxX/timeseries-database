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

std::string IndexMapping::to_string() const {
  return std::format("IndexMapping: key: {}, offset: {}", this->key,
                     this->offset);
}

// TODO: Try to make it log(n) with binary search
IndexMapping
find_closest_index(std::shared_ptr<std::vector<IndexMapping>> elements,
                   DataPointKey searched_value, bool from_right = false) {

  int start_index = 0;
  int last_index = elements->size();
  int iter = 1;

  if (from_right) {
    start_index = elements->size() - 1;
    last_index = 0;
    iter = -1;
  }

  for (int i = start_index; i * iter <= last_index * iter; i += iter) {
    auto curr_element = elements->at(i);

    if (curr_element.key == searched_value) {
      return curr_element;
    }

    if (!from_right && curr_element.key > searched_value) {
      return curr_element;
    }

    if (from_right && curr_element.key < searched_value) {
      return curr_element;
    }
  }

  return {};
}

IndexesMetadataBlock::IndexesMetadataBlock() {
  indexes = std::make_shared<std::vector<IndexMapping>>();
};

IndexesMetadataBlock::IndexesMetadataBlock(
    std::shared_ptr<std::vector<IndexMapping>> mappings)
    : indexes(mappings) {}

IndexResult IndexesMetadataBlock::index_range(DataPointKey start_key,
                                              DataPointKey end_key) {

  if (indexes->size() == 0) {
    throw std::runtime_error("indexes not loaded");
  }

  IndexMapping start = find_closest_index(indexes, start_key, false);
  IndexMapping end = find_closest_index(indexes, end_key, true);

  // Adding the end.length so that the last element is included.
  IndexResult result{.start_byte_offset = start.offset,
                     .end_byte_offset = end.offset + end.length};
  return result;
}

std::size_t IndexesMetadataBlock::size() { return this->indexes->size(); }

void IndexesMetadataBlock::decode(EncodedBuffer &data) {

  size_t mapping_size = sizeof(IndexMapping);

  for (size_t i = 0; i < data.size() / mapping_size; i++) {
    IndexMapping index;

    std::memcpy(&index, data.data() + i * mapping_size, mapping_size);

    this->indexes->push_back(index);
  }
}
EncodedBuffer IndexesMetadataBlock::encode() {
  EncodedBuffer result;

  for (auto &index : *this->indexes) {
    EncodedBuffer encoded_index(sizeof(IndexMapping));
    std::memcpy(encoded_index.data(), &index, sizeof(IndexMapping));
    result.insert(result.end(), encoded_index.begin(), encoded_index.end());
  }

  return result;
}

void IndexesMetadataBlock::operator=(const IndexesMetadataBlock &new_indexes) {

  this->indexes = new_indexes.indexes;
}

std::shared_ptr<std::vector<IndexMapping>>
IndexesMetadataBlock::get_ordered_indexes() {
  return this->indexes;
}

IndexMapping IndexesMetadataBlock::last_index() {
  return this->indexes->at(this->indexes->size() - 1);
}

IndexMapping IndexesMetadataBlock::first_index() {
  return this->indexes->at(0);
}
