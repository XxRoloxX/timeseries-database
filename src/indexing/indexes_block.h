#pragma once

#include "../datapoints/data_point.h"
#include <vector>

struct IndexMapping {
  DataPointKey key;
  size_t offset;
  size_t length;

  bool operator>(const IndexMapping &) const;
  bool operator<(const IndexMapping &) const;
  bool operator==(const IndexMapping &) const;
  std::string to_string() const;
};

struct IndexResult {
  size_t start_byte_offset;
  size_t end_byte_offset;
};

class IndexesMetadataBlock {
private:
  std::shared_ptr<std::vector<IndexMapping>> indexes;

public:
  IndexesMetadataBlock(std::shared_ptr<std::vector<IndexMapping>> mappings);
  IndexesMetadataBlock();
  void decode(EncodedBuffer &data);
  std::vector<char> encode();
  std::size_t size();
  IndexResult index_range(DataPointKey start_key, DataPointKey end_key);
  std::shared_ptr<std::vector<IndexMapping>> get_ordered_indexes();
  IndexMapping last_index();
  IndexMapping first_index();
  void operator=(const IndexesMetadataBlock &);
};
