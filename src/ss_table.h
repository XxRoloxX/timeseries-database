#include "logger.h"
#include <memory>
#include <string>
#include <vector>

class RawSSTable {
private:
  std::vector<char> data;
  std::shared_ptr<Logger> logger;
  std::string name;

public:
  RawSSTable(std::shared_ptr<Logger> logger, std::string name);
  void read();
  std::vector<char> load_table_range(const std::string &filePath,
                                     std::size_t startByte,
                                     std::size_t endByte) {
    return {};
  };
  void merge_with(std::shared_ptr<RawSSTable> table) {};
  void load_data(std::vector<char> new_data);
  void write();
  std::shared_ptr<std::vector<char>> get_data();
};
