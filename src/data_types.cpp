#include "data_types.h"
#include <mutex>

using namespace std;

// ─────────────────────────────────────────
// LIST STORE IMPLEMENTATION
// ─────────────────────────────────────────

int ListStore::lpush(const string& key,
                     const string& value) {
    unique_lock<shared_mutex> lock(mutex);
    data[key].push_front(value);
    return data[key].size();
}

int ListStore::rpush(const string& key,
                     const string& value) {
    unique_lock<shared_mutex> lock(mutex);
    data[key].push_back(value);
    return data[key].size();
}

optional<string> ListStore::lpop(
        const string& key) {
    unique_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end() || it->second.empty()) {
        return nullopt;
    }
    string val = it->second.front();
    it->second.pop_front();
    // Clean up empty list
    if (it->second.empty()) data.erase(it);
    return val;
}

optional<string> ListStore::rpop(
        const string& key) {
    unique_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end() || it->second.empty()) {
        return nullopt;
    }
    string val = it->second.back();
    it->second.pop_back();
    if (it->second.empty()) data.erase(it);
    return val;
}

vector<string> ListStore::lrange(
        const string& key, int start, int end) {
    shared_lock<shared_mutex> lock(mutex);
    vector<string> result;

    auto it = data.find(key);
    if (it == data.end()) return result;

    int size = it->second.size();

    // Handle negative indices like Redis
    // -1 means last element, -2 means second to last
    if (start < 0) start = max(0, size + start);
    if (end < 0)   end   = size + end;
    if (end >= size) end = size - 1;
    if (start > end) return result;

    for (int i = start; i <= end; i++) {
        result.push_back(it->second[i]);
    }
    return result;
}

int ListStore::llen(const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}

// ─────────────────────────────────────────
// SET STORE IMPLEMENTATION
// ─────────────────────────────────────────

int SetStore::sadd(const string& key,
                   const string& member) {
    unique_lock<shared_mutex> lock(mutex);
    // insert() returns {iterator, bool}
    // bool = true if inserted, false if already existed
    auto result = data[key].insert(member);
    return result.second ? 1 : 0;
}

int SetStore::srem(const string& key,
                   const string& member) {
    unique_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    int removed = it->second.erase(member);
    if (it->second.empty()) data.erase(it);
    return removed;
}

vector<string> SetStore::smembers(
        const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    vector<string> result;
    auto it = data.find(key);
    if (it == data.end()) return result;
    for (const auto& m : it->second) {
        result.push_back(m);
    }
    return result;
}

bool SetStore::sismember(const string& key,
                         const string& member) {
    shared_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return false;
    return it->second.count(member) > 0;
}

int SetStore::scard(const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}

// ─────────────────────────────────────────
// HASH STORE IMPLEMENTATION
// ─────────────────────────────────────────

void HashStore::hset(const string& key,
                     const string& field,
                     const string& value) {
    unique_lock<shared_mutex> lock(mutex);
    data[key][field] = value;
}

optional<string> HashStore::hget(
        const string& key,
        const string& field) {
    shared_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return nullopt;
    auto fit = it->second.find(field);
    if (fit == it->second.end()) return nullopt;
    return fit->second;
}

bool HashStore::hdel(const string& key,
                     const string& field) {
    unique_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return false;
    bool deleted = it->second.erase(field) > 0;
    if (it->second.empty()) data.erase(it);
    return deleted;
}

vector<string> HashStore::hgetall(
        const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    vector<string> result;
    auto it = data.find(key);
    if (it == data.end()) return result;
    // Return field, value, field, value...
    for (const auto& [field, value] : it->second) {
        result.push_back(field);
        result.push_back(value);
    }
    return result;
}

int HashStore::hlen(const string& key) {
    shared_lock<shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) return 0;
    return it->second.size();
}
