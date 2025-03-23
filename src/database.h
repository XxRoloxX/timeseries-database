#pragma once

#include "cache/write_back_cache.h"
#include "encoding/encoder.h"
#include <string>

class Database {
public:
  Database(size_t memtable_size) {
    this->memtable_size = memtable_size;
    this->memtable = new MemTable<int, Encodable>;
  }
  void insert(std::string series_name, int key,
              std::shared_ptr<Encodable> value) {
    this->memtable->set(key, value);
  }

  std::vector<std::shared_ptr<Encodable>> read(size_t from, size_t to) {
    return this->memtable->get_range(from, to);
  }

private:
  size_t memtable_size;
  WriteBackCache<int, Encodable> *memtable;
};
