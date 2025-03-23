#include "./indexes_block.h"
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

IndexResult IndexesMetadataBlock::index_range(DataPointKey start_key,
                                              DataPointKey end_key) {

  IndexMapping start = find_closest_index(&indexes, start_key);
  IndexMapping end = find_closest_index(&indexes, start_key);

  IndexResult result{.start_byte_offset = start.offset,
                     .end_byte_offset = end.offset};
  return result;
}

void IndexesMetadataBlock::decode(EncodedBuffer data) {}
std::vector<char> IndexesMetadataBlock::encode() { return {}; }
