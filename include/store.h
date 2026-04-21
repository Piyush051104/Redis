#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <chrono>

// Type alias for time point — makes code cleaner
using TimePoint = std::chrono::steady_clock::time_point;

class Store {
private:
    std::unordered_map<std::string, std::string> data;

    // Stores expiry time for each key
    // If key not in this map → no expiry
    std::unordered_map<std::string, TimePoint> expiry;

    mutable std::shared_mutex mutex;

    // Internal check — is this key expired right now?
    // Must be called with lock already held
    bool isExpired(const std::string& key) const;

public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    std::vector<std::string> keys();

    // Set expiry on existing key
    // Returns false if key doesn't exist
    bool expire(const std::string& key, int seconds);

    // Get remaining TTL in seconds
    // Returns -1 if no expiry, -2 if key doesn't exist
    int ttl(const std::string& key);

    // SET + EXPIRE in one command
    void setex(const std::string& key,
               const std::string& value,
               int seconds);

    // Called by background thread
    // Deletes all expired keys
    void cleanupExpired();
};