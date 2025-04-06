#pragma once

#include "../logger/logger.h"
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

template <class K, class V> class WriteBackCache {
public:
  virtual void set(std::string series, K key, V val) = 0;
  virtual std::vector<V> get_all(std::string series) = 0;
  virtual std::vector<V> get_range(std::string series, K start, K end) = 0;
  virtual std::optional<V> get(std::string series, K key) = 0;
  virtual size_t size(std::string series) = 0;
  virtual int purge(std::string series) = 0;
  virtual std::vector<std::string> get_series() = 0;
};

template <class K, class V> class MemTable : public WriteBackCache<K, V> {
private:
  std::map<std::string, std::shared_ptr<std::map<K, V>>> cache;
  std::shared_ptr<Logger> logger;

public:
  MemTable(std::shared_ptr<Logger> logger) : logger(logger) {}

  void set(std::string series, K key, V val) override {

    if (!this->is_series_initialized(series)) {
      cache[series] = std::make_shared<std::map<K, V>>();
    }

    (*cache[series])[key] = val;
  }
  std::vector<V> get_all(std::string series) override {

    std::vector<V> res;

    if (!this->is_series_initialized(series)) {
      return res;
    }

    for (auto &[key, value] : *cache[series]) {
      res.push_back(value);
    }

    return res;
  }
  std::optional<V> get(std::string series, K key) override {

    if (!this->is_series_initialized(series)) {
      return std::nullopt;
    }

    auto it = cache[series]->find(key);
    if (it != cache[series]->end()) {
      return std::nullopt;
    }

    return std::move(it->second);
  }

  std::vector<V> get_range(std::string series, K start, K end) override {

    std::vector<V> res;

    if (!this->is_series_initialized(series)) {
      return res;
    }

    for (auto &[key, value] : *cache[series]) {
      if (key >= start && key <= end) {
        res.push_back(value);
      }
    }

    this->logger->info(std::format("reading {} points from cache", res.size()));

    return res;
  }

  bool is_series_initialized(std::string series) {
    auto it = cache.find(series);
    return it != cache.end();
  }

  size_t size(std::string series) override {

    if (!this->is_series_initialized(series)) {
      return 0;
    }

    return cache[series]->size();
  }
  int purge(std::string series) override {
    auto size = cache[series]->size();
    cache[series] = std::make_shared<std::map<K, V>>();
    return size;
  }

  std::vector<std::string> get_series() override {
    auto ks = std::views::keys(this->cache);
    std::vector<std::string> keys{ks.begin(), ks.end()};
    return keys;
  };
};
