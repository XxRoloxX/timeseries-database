#pragma once

#include "encoder.h"
#include "write_back_cache.h"
#include <string>

class Database {
public:
  Database(size_t memtable_size);
  void insert(std::string series_name, int key,
              std::shared_ptr<Encodable> value);
  std::vector<std::shared_ptr<Encodable>> read(size_t from, size_t to);

private:
  size_t memtable_size;
  WriteBackCache<int, Encodable> *memtable;
};
