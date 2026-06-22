#include "store.h"
#include <mutex>
#include <shared_mutex>
#include <vector>

using namespace std;

Store::Store(size_t maxKeys) : lru(maxKeys) {}

bool Store::isExpired(const string& key) const {
    auto it = expiry.find(key);
    if (it == expiry.end()) return false;
    return chrono::steady_clock::now() > it->second;
}

void Store::set(const string& key,
                const string& value,
                optional<string>* evicted) {
    unique_lock<shared_mutex> lock(mutex);

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

optional<string> Store::get(const string& key) {
    unique_lock<shared_mutex> lock(mutex);

    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        lru.remove(key);
        return nullopt;
    }

    auto it = data.find(key);
    if (it == data.end()) return nullopt;

    // Update LRU on access
    lru.access(key);
    return it->second;
}

bool Store::del(const string& key) {
    unique_lock<shared_mutex> lock(mutex);
    expiry.erase(key);
    lru.remove(key);
    return data.erase(key) == 1;
}

bool Store::exists(const string& key) {
    unique_lock<shared_mutex> lock(mutex);
    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        lru.remove(key);
        return false;
    }
    return data.find(key) != data.end();
}

vector<string> Store::keys() {
    unique_lock<shared_mutex> lock(mutex);
    vector<string> result;
    for (const auto& [key, value] : data) {
        if (!isExpired(key)) result.push_back(key);
    }
    return result;
}

bool Store::expire(const string& key, int seconds) {
    unique_lock<shared_mutex> lock(mutex);
    if (data.find(key) == data.end()) return false;
    expiry[key] = chrono::steady_clock::now()
                + chrono::seconds(seconds);
    return true;
}

int Store::ttl(const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    if (data.find(key) == data.end()) return -2;
    auto it = expiry.find(key);
    if (it == expiry.end()) return -1;
    auto remaining = chrono::duration_cast
        <chrono::seconds>(
            it->second - chrono::steady_clock::now()
        ).count();
    if (remaining <= 0) return -2;
    return (int)remaining;
}

void Store::setex(const string& key,
                  const string& value,
                  int seconds) {
    unique_lock<shared_mutex> lock(mutex);
    lru.insert(key);
    data[key] = value;
    expiry[key] = chrono::steady_clock::now()
                + chrono::seconds(seconds);
}

void Store::cleanupExpired() {
    unique_lock<shared_mutex> lock(mutex);
    vector<string> toDelete;
    for (const auto& [key, expireAt] : expiry) {
        if (chrono::steady_clock::now() > expireAt) {
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
    shared_lock<shared_mutex> lock(mutex);
    return data.size();
}
