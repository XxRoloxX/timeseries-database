#pragma once

#include <map>
#include <memory>
#include <optional>
#include <vector>

template <class K, class V> class WriteBackCache {
public:
  virtual void set(K key, std::shared_ptr<V> val) = 0;
  virtual std::vector<std::shared_ptr<V>> get_all() = 0;
  virtual std::vector<std::shared_ptr<V>> get_range(K start, K end) = 0;
  virtual std::optional<std::shared_ptr<V>> get(K key) = 0;
};

template <class K, class V> class MemTable : public WriteBackCache<K, V> {
private:
  std::map<K, std::shared_ptr<V>> cache;

public:
  MemTable() {}
  void set(K key, std::shared_ptr<V> val) override { cache[key] = val; }
  std::vector<std::shared_ptr<V>> get_all() override {

    std::vector<std::shared_ptr<V>> res;

    for (auto [key, value] : cache) {
      res.push_back(value);
    }

    return res;
  }
  std::optional<std::shared_ptr<V>> get(K key) override {

    auto it = cache.find(key);
    if (it != cache.end()) {
      return std::nullopt;
    }

    return std::move(it->second);
  }

  std::vector<std::shared_ptr<V>> get_range(K start, K end) override {

    std::vector<std::shared_ptr<V>> res;

    for (auto [key, value] : cache) {
      if (key >= start && end <= end) {
        res.push_back(value);
      }
    }

    return res;
  }
};
