#pragma once

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <optional>

// ─────────────────────────────────────────
// LIST STORE
// ─────────────────────────────────────────
class ListStore {
private:
    std::unordered_map<std::string,
                       std::deque<std::string>> data;
    mutable std::shared_mutex mutex;

public:
    // LPUSH — push to front, returns new length
    int lpush(const std::string& key,
              const std::string& value);

    // RPUSH — push to back, returns new length
    int rpush(const std::string& key,
              const std::string& value);

    // LPOP — pop from front
    std::optional<std::string> lpop(const std::string& key);

    // RPOP — pop from back
    std::optional<std::string> rpop(const std::string& key);

    // LRANGE — get elements from start to end index
    std::vector<std::string> lrange(const std::string& key,
                                    int start, int end);

    // LLEN — get length of list
    int llen(const std::string& key);
};

// ─────────────────────────────────────────
// SET STORE
// ─────────────────────────────────────────
class SetStore {
private:
    std::unordered_map<std::string,
                       std::unordered_set<std::string>> data;
    mutable std::shared_mutex mutex;

public:
    // SADD — add member, returns 1 if added, 0 if existed
    int sadd(const std::string& key,
             const std::string& member);

    // SREM — remove member, returns 1 if removed
    int srem(const std::string& key,
             const std::string& member);

    // SMEMBERS — get all members
    std::vector<std::string> smembers(const std::string& key);

    // SISMEMBER — check if member exists
    bool sismember(const std::string& key,
                   const std::string& member);

    // SCARD — get number of members
    int scard(const std::string& key);
};

// ─────────────────────────────────────────
// HASH STORE
// ─────────────────────────────────────────
class HashStore {
private:
    // key → field → value
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::string>> data;
    mutable std::shared_mutex mutex;

public:
    // HSET — set field in hash
    void hset(const std::string& key,
              const std::string& field,
              const std::string& value);

    // HGET — get field value
    std::optional<std::string> hget(const std::string& key,
                                    const std::string& field);

    // HDEL — delete field
    bool hdel(const std::string& key,
              const std::string& field);

    // HGETALL — get all fields and values
    std::vector<std::string> hgetall(const std::string& key);

    // HLEN — number of fields
    int hlen(const std::string& key);
};