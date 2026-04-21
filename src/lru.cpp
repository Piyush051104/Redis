#include "lru.h"

LRUCache::LRUCache(size_t maxSize) : maxSize(maxSize) {}

void LRUCache::access(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = lruMap.find(key);
    if (it == lruMap.end()) return;

    // Move to front — most recently used
    lruList.splice(lruList.begin(), lruList, it->second);
}

std::optional<std::string> LRUCache::insert(
        const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);

    // If key already exists just move to front
    auto it = lruMap.find(key);
    if (it != lruMap.end()) {
        lruList.splice(lruList.begin(), lruList, it->second);
        return std::nullopt;
    }

    std::optional<std::string> evicted = std::nullopt;

    // If at capacity — evict least recently used (back)
    if (lruList.size() >= maxSize) {
        std::string lruKey = lruList.back();
        lruList.pop_back();
        lruMap.erase(lruKey);
        evicted = lruKey;
    }

    // Insert new key at front
    lruList.push_front(key);
    lruMap[key] = lruList.begin();

    return evicted;
}

void LRUCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = lruMap.find(key);
    if (it == lruMap.end()) return;

    lruList.erase(it->second);
    lruMap.erase(it);
}

size_t LRUCache::size() {
    std::lock_guard<std::mutex> lock(mutex);
    return lruList.size();
}

bool LRUCache::atCapacity() {
    std::lock_guard<std::mutex> lock(mutex);
    return lruList.size() >= maxSize;
}