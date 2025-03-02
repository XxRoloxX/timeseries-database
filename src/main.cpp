#include "data_point.h"
#include "write_back_cache.h"
#include <iostream>

using namespace std;

int main() {

  DataPoint<int> *point = new DataPoint(12, 12);

  WriteBackCache<int, int> *cache = new MemTable<int, int>();

  cache->append(12, 10);

  auto flushed = cache->flush();

  cout << flushed[0] << endl;

  return 0;
}
