#pragma once

#include "data_point.h"
#include <vector>

struct IndexMapping {
  DataPointKey key;
  int offset;

  bool operator>(const IndexMapping &) const;
  bool operator<(const IndexMapping &) const;
  bool operator==(const IndexMapping &) const;
};

struct IndexResult {
  int start_byte_offset;
  int end_byte_offset;
};

class IndexesMetadataBlock {
private:
  std::vector<IndexMapping> indexes;

public:
  // IndexesMetadataBlock();
  void decode(std::vector<char> data);
  std::vector<char> encode();
  IndexResult index_range(DataPointKey start_key, DataPointKey end_key);
};
