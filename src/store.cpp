#include "store.h"
#include <mutex>
#include <shared_mutex>
#include <vector>

Store::Store(size_t maxKeys) : lru(maxKeys) {}

bool Store::isExpired(const std::string& key) const {
    auto it = expiry.find(key);
    if (it == expiry.end()) return false;
    return std::chrono::steady_clock::now() > it->second;
}

void Store::set(const std::string& key,
                const std::string& value,
                std::optional<std::string>* evicted) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    // Insert into LRU — get evicted key if any
    auto evictedKey = lru.insert(key);

    // If a key was evicted remove it from store too
    if (evictedKey.has_value()) {
        data.erase(evictedKey.value());
        expiry.erase(evictedKey.value());
        if (evicted) *evicted = evictedKey;
    }

    data[key] = value;
    expiry.erase(key);
}

std::optional<std::string> Store::get(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        lru.remove(key);
        return std::nullopt;
    }

    auto it = data.find(key);
    if (it == data.end()) return std::nullopt;

    // Update LRU on access
    lru.access(key);
    return it->second;
}

bool Store::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    expiry.erase(key);
    lru.remove(key);
    return data.erase(key) == 1;
}

bool Store::exists(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        lru.remove(key);
        return false;
    }
    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::unique_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [key, value] : data) {
        if (!isExpired(key)) result.push_back(key);
    }
    return result;
}

bool Store::expire(const std::string& key, int seconds) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (data.find(key) == data.end()) return false;
    expiry[key] = std::chrono::steady_clock::now()
                + std::chrono::seconds(seconds);
    return true;
}

int Store::ttl(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    if (data.find(key) == data.end()) return -2;
    auto it = expiry.find(key);
    if (it == expiry.end()) return -1;
    auto remaining = std::chrono::duration_cast
        <std::chrono::seconds>(
            it->second - std::chrono::steady_clock::now()
        ).count();
    if (remaining <= 0) return -2;
    return (int)remaining;
}

void Store::setex(const std::string& key,
                  const std::string& value,
                  int seconds) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    lru.insert(key);
    data[key] = value;
    expiry[key] = std::chrono::steady_clock::now()
                + std::chrono::seconds(seconds);
}

void Store::cleanupExpired() {
    std::unique_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> toDelete;
    for (const auto& [key, expireAt] : expiry) {
        if (std::chrono::steady_clock::now() > expireAt) {
            toDelete.push_back(key);
        }
    }
    for (const auto& key : toDelete) {
        data.erase(key);
        expiry.erase(key);
        lru.remove(key);
    }
}

size_t Store::keyCount() {
    std::shared_lock<std::shared_mutex> lock(mutex);
    return data.size();
}