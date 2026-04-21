#pragma once

#include <string>
#include <list>
#include <unordered_map>
#include <mutex>
#include <optional>

class LRUCache {
private:
    // Maximum number of keys allowed
    size_t maxSize;

    // LRU order — front is most recent, back is oldest
    std::list<std::string> lruList;

    // Maps key → iterator in lruList for O(1) access
    std::unordered_map<std::string, std::list<std::string>::iterator> lruMap;

    std::mutex mutex;

public:
    LRUCache(size_t maxSize);

    // Call on every GET/SET — moves key to front
    void access(const std::string& key);

    // Call on every SET — adds new key
    // Returns evicted key if limit hit, empty if not
    std::optional<std::string> insert(const std::string& key);

    // Call on DEL — removes key from LRU tracker
    void remove(const std::string& key);

    // Current number of tracked keys
    size_t size();

    // Check if at capacity
    bool atCapacity();
};