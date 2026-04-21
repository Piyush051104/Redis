#pragma once

#include <string>
#include <atomic>
#include <chrono>

class Stats {
private:
    // Server start time
    std::chrono::steady_clock::time_point startTime;

public:
    // Atomic counters — thread safe without mutex
    std::atomic<long long> totalCommands{0};
    std::atomic<long long> totalConnections{0};
    std::atomic<long long> totalKeysEvicted{0};
    std::atomic<long long> currentConnections{0};

    Stats();

    // Returns uptime in seconds
    long long uptimeSeconds();

    // Returns formatted INFO string
    std::string infoString(size_t keyCount);
};