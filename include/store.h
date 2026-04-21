#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <chrono>
#include "lru.h"

using TimePoint = std::chrono::steady_clock::time_point;

class Store {
private:
    std::unordered_map<std::string, std::string> data;
    std::unordered_map<std::string, TimePoint> expiry;
    mutable std::shared_mutex mutex;

    // LRU tracker — max 1000 keys by default
    LRUCache lru;

    bool isExpired(const std::string& key) const;

public:
    Store(size_t maxKeys = 1000);

    void set(const std::string& key,
             const std::string& value,
             std::optional<std::string>* evicted = nullptr);

    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    std::vector<std::string> keys();
    bool expire(const std::string& key, int seconds);
    int ttl(const std::string& key);
    void setex(const std::string& key,
               const std::string& value,
               int seconds);
    void cleanupExpired();
    size_t keyCount();
};