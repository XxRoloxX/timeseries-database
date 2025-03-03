#include "database.h"
#include <memory>
#include <string>

void Database::insert(std::string series_name, int key,
                      std::shared_ptr<Encodable> value) {
  this->memtable->set(key, value);
}

std::vector<std::shared_ptr<Encodable>> Database::read(size_t start,
                                                       size_t end) {

  return this->memtable->get_range(start, end);
};

Database::Database(size_t memtable_size) {
  this->memtable_size = memtable_size;
  this->memtable = new MemTable<int, Encodable>;
}
