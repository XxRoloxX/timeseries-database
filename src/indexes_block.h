#include "data_point.h"
#include <vector>

struct IndexMapping {
  DataPointKey key;
  int offset;
};
class IndexesMetadataBlock {
private:
  std::vector<char> raw_indicies_block;
  std::vector<IndexMapping> indexes;

public:
  void load_raw(std::vector<char> data);
  void load_indexes(std::vector<IndexMapping> data);
};
