#include "./indexes_block.h"
#include <algorithm>
#include <cstring>
#include <iostream>
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
IndexMapping find_closest_index(std::vector<IndexMapping> *elements,
                                DataPointKey searched_value,
                                bool from_right = false) {

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
// std::size_t min_idx = 0;
// std::size_t max_idx = elements->size() - 1;
// std::size_t mid = (max_idx + min_idx) / 2;
//
// std::cout << searched_value << std::endl;
//
// while (min_idx + 1 < max_idx) {
//
//   std::cout << min_idx << " " << max_idx << std::endl;
//   std::cout << elements->at(mid).to_string() << std::endl;
//
//   mid = (max_idx + min_idx) / 2;
//
//   if ((*elements)[mid].key == searched_value) {
//     return elements->at(mid);
//   }
//
//   if ((*elements)[mid].key < searched_value) {
//     min_idx = mid;
//     continue;
//   }
//
//   if ((*elements)[mid].key > searched_value) {
//     max_idx = mid;
//     continue;
//   }
// }
//
// return elements->at(mid);
//}
;

IndexesMetadataBlock::IndexesMetadataBlock() = default;
IndexesMetadataBlock::IndexesMetadataBlock(std::vector<IndexMapping> mappings)
    : indexes(mappings) {}

IndexResult IndexesMetadataBlock::index_range(DataPointKey start_key,
                                              DataPointKey end_key) {

  if (indexes.size() == 0) {
    throw std::runtime_error("indexes not loaded");
  }

  IndexMapping start = find_closest_index(&indexes, start_key, false);
  IndexMapping end = find_closest_index(&indexes, end_key, true);

  std::cout << start.to_string() << std::endl;
  std::cout << end.to_string() << std::endl;

  // Adding the end.length so that the last element is included.
  IndexResult result{.start_byte_offset = start.offset,
                     .end_byte_offset = end.offset + end.length};
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
