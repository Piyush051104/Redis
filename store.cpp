#include "store.h"
#include <mutex>
#include <shared_mutex>
#include <vector>

void Store::set(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    data[key] = value;
}

std::optional<std::string> Store::get(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = data.find(key);
    if (it == data.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool Store::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    return data.erase(key) == 1;
}

bool Store::exists(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    return data.find(key) != data.end();
}

std::vector<std::string> Store::keys() {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& [key, value] : data) {
        result.push_back(key);
    }
    return result;
}