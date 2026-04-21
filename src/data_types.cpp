#include "data_types.h"
#include <mutex>

// ─────────────────────────────────────────
// LIST STORE IMPLEMENTATION
// ─────────────────────────────────────────

int ListStore::lpush(const std::string& key,
                     const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key].push_front(value);
    return data[key].size();
}

int ListStore::rpush(const std::string& key,
                     const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key].push_back(value);
    return data[key].size();
}

std::optional<std::string> ListStore::lpop(
        const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end() || it->second.empty()) {
        return std::nullopt;
    }
    std::string val = it->second.front();
    it->second.pop_front();
    // Clean up empty list
    if (it->second.empty()) data.erase(it);
    return val;
}

std::optional<std::string> ListStore::rpop(
        const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end() || it->second.empty()) {
        return std::nullopt;
    }
    std::string val = it->second.back();
    it->second.pop_back();
    if (it->second.empty()) data.erase(it);
    return val;
}

std::vector<std::string> ListStore::lrange(
        const std::string& key, int start, int end) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> result;

    auto it = data.find(key);
    if (it == data.end()) return result;

    int size = it->second.size();

    // Handle negative indices like Redis
    // -1 means last element, -2 means second to last
    if (start < 0) start = std::max(0, size + start);
    if (end < 0)   end   = size + end;
    if (end >= size) end = size - 1;
    if (start > end) return result;

    for (int i = start; i <= end; i++) {
        result.push_back(it->second[i]);
    }
    return result;
}

int ListStore::llen(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}

// ─────────────────────────────────────────
// SET STORE IMPLEMENTATION
// ─────────────────────────────────────────

int SetStore::sadd(const std::string& key,
                   const std::string& member) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    // insert() returns {iterator, bool}
    // bool = true if inserted, false if already existed
    auto result = data[key].insert(member);
    return result.second ? 1 : 0;
}

int SetStore::srem(const std::string& key,
                   const std::string& member) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    int removed = it->second.erase(member);
    if (it->second.empty()) data.erase(it);
    return removed;
}

std::vector<std::string> SetStore::smembers(
        const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> result;
    auto it = data.find(key);
    if (it == data.end()) return result;
    for (const auto& m : it->second) {
        result.push_back(m);
    }
    return result;
}

bool SetStore::sismember(const std::string& key,
                         const std::string& member) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return false;
    return it->second.count(member) > 0;
}

int SetStore::scard(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}

// ─────────────────────────────────────────
// HASH STORE IMPLEMENTATION
// ─────────────────────────────────────────

void HashStore::hset(const std::string& key,
                     const std::string& field,
                     const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key][field] = value;
}

std::optional<std::string> HashStore::hget(
        const std::string& key,
        const std::string& field) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return std::nullopt;
    auto fit = it->second.find(field);
    if (fit == it->second.end()) return std::nullopt;
    return fit->second;
}

bool HashStore::hdel(const std::string& key,
                     const std::string& field) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return false;
    bool deleted = it->second.erase(field) > 0;
    if (it->second.empty()) data.erase(it);
    return deleted;
}

std::vector<std::string> HashStore::hgetall(
        const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> result;
    auto it = data.find(key);
    if (it == data.end()) return result;
    // Return field, value, field, value...
    for (const auto& [field, value] : it->second) {
        result.push_back(field);
        result.push_back(value);
    }
    return result;
}

int HashStore::hlen(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}