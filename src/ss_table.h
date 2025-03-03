#include <string>
#include <vector>

class RawSSTableReader {
public:
  std::vector<char> load_table(std::string table_path);
  std::vector<char> load_table_range(const std::string &filePath,
                                     std::size_t startByte,
                                     std::size_t endByte);
};
