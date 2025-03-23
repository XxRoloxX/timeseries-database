#include "../encoding/encodable.h"
#include <vector>

const int INDEXES_METADATA_BLOCK_OFFSET_LENGTH_BYTES = sizeof(int);

class MetadataBlock {
private:
  int indexes_block_offset;

public:
  int load(std::vector<char> *data);
  EncodedBuffer encode();
  int get_indexes_block_offset();
  void set_indexes_block_offset(int offset);
};
