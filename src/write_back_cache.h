
#include <map>
#include <vector>
template <class K, class V> class WriteBackCache {
public:
  virtual void append(K key, V val) = 0;
  virtual std::vector<V> flush() = 0;
};

template <class K, class V> class MemTable : public WriteBackCache<K, V> {
private:
  std::map<K, V> cache;

public:
  MemTable() {}
  void append(K key, V val) { cache[key] = val; }
  std::vector<V> flush() {

    std::vector<V> res;

    for (auto [key, value] : cache) {
      res.push_back(value);
    }

    return res;
  }
};
