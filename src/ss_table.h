#include <string>
#include <vector>
#include "encoder.h"
#include <fstream>
#include <iostream>

template<typename T> class RawSSTableReader {
private:
  std::vector<std::shared_ptr<T>> rows;
public:
  void load_table(std::string table_path);
  std::vector<char> load_table_range(const std::string &filePath,
                                     std::size_t startByte,
                                     std::size_t endByte);
  void merge_with(std::shared_ptr<RawSSTableReader<T>> table);
};

template <typename T>
void RawSSTableReader<T>::load_table(std::string table_path)
{
 std::fstream table_file(table_path);

  char *data = new char[1000];

 table_file.read(data, 1000);

 table_file.close();
}

template <typename T>
inline std::vector<char> RawSSTableReader<T>::load_table_range(const std::string &filePath, std::size_t startByte, std::size_t endByte)
{
    return std::vector<char>();
}

template <typename T>
inline void RawSSTableReader<T>::merge_with(std::shared_ptr<RawSSTableReader<T>> table)
{
}

template<typename T>class RawSSTableWriter {
  public:
    void write(std::string name, std::vector<char> body) {
      std::ofstream new_table(name);
      std::string output;
      for (auto ch: body){
        std::cout<<ch<<std::endl;
        output+=ch;
      }
      new_table<<output;
      new_table.close();
    }
};

