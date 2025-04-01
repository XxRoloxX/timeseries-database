#pragma once

#include "../logger/logger.h"
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <vector>

template <class K, class V> class WriteBackCache {
public:
  virtual void set(K key, V val) = 0;
  virtual std::vector<V> get_all() = 0;
  virtual std::vector<V> get_range(K start, K end) = 0;
  virtual std::optional<V> get(K key) = 0;
  virtual size_t size() = 0;
  virtual int purge() = 0;
};

template <class K, class V> class MemTable : public WriteBackCache<K, V> {
private:
  std::map<K, V> cache;
  std::shared_ptr<Logger> logger;

public:
  MemTable(std::shared_ptr<Logger> logger) : logger(logger) {}
  void set(K key, V val) override { cache[key] = val; }
  std::vector<V> get_all() override {

    std::vector<V> res;

    for (auto [key, value] : cache) {
      res.push_back(value);
    }

    return res;
  }
  std::optional<V> get(K key) override {

    auto it = cache.find(key);
    if (it != cache.end()) {
      return std::nullopt;
    }

    return std::move(it->second);
  }

  std::vector<V> get_range(K start, K end) override {

    std::vector<V> res;

    for (auto [key, value] : cache) {
      if (key >= start && end <= end) {
        res.push_back(value);
      }
    }

    this->logger->info(
        std::format("reading data of size {} from cache", res.size()));

    return res;
  }

  size_t size() override { return cache.size(); }
  int purge() override {
    auto size = cache.size();
    cache = std::map<K, V>();
    return size;
  }
};
