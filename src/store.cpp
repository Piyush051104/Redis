#include "store.h"
#include <mutex>
#include <shared_mutex>
#include <vector>

// Internal helper — check if key is expired
// Caller must hold the lock
bool Store::isExpired(const std::string& key) const {
    auto it = expiry.find(key);
    if (it == expiry.end()) return false; // no expiry set

    // Compare current time with expiry time
    return std::chrono::steady_clock::now() > it->second;
}

void Store::set(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key] = value;
    // Clear any existing expiry when key is reset
    expiry.erase(key);
}

std::optional<std::string> Store::get(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    // Lazy expiry check — delete if expired
    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        return std::nullopt;
    }

    auto it = data.find(key);
    if (it == data.end()) return std::nullopt;
    return it->second;
}

bool Store::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    expiry.erase(key); // also remove expiry
    return data.erase(key) == 1;
}

bool Store::exists(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    // Lazy expiry check
    if (isExpired(key)) {
        data.erase(key);
        expiry.erase(key);
        return false;
    }

    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::unique_lock<std::shared_mutex> lock(mutex);

    std::vector<std::string> result;
    for (const auto& [key, value] : data) {
        // Skip expired keys
        if (!isExpired(key)) {
            result.push_back(key);
        }
    }
    return result;
}

bool Store::expire(const std::string& key, int seconds) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    // Key must exist
    if (data.find(key) == data.end()) return false;

    // Set expiry time = now + seconds
    expiry[key] = std::chrono::steady_clock::now()
                + std::chrono::seconds(seconds);
    return true;
}

int Store::ttl(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);

    // Key doesn't exist
    if (data.find(key) == data.end()) return -2;

    // Key exists but no expiry
    auto it = expiry.find(key);
    if (it == expiry.end()) return -1;

    // Calculate remaining seconds
    auto now = std::chrono::steady_clock::now();
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>
                     (it->second - now).count();

    // Already expired
    if (remaining <= 0) return -2;

    return (int)remaining;
}

void Store::setex(const std::string& key,
                  const std::string& value,
                  int seconds) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key] = value;
    expiry[key] = std::chrono::steady_clock::now()
                + std::chrono::seconds(seconds);
}

void Store::cleanupExpired() {
    std::unique_lock<std::shared_mutex> lock(mutex);

    // Collect expired keys first
    std::vector<std::string> toDelete;
    for (const auto& [key, expireAt] : expiry) {
        if (std::chrono::steady_clock::now() > expireAt) {
            toDelete.push_back(key);
        }
    }

    // Delete them
    for (const auto& key : toDelete) {
        data.erase(key);
        expiry.erase(key);
    }
}