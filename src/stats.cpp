#include "stats.h"
#include <sstream>

Stats::Stats() {
    startTime = std::chrono::steady_clock::now();
}

long long Stats::uptimeSeconds() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>
           (now - startTime).count();
}

std::string Stats::infoString(size_t keyCount) {
    std::ostringstream ss;
    ss << "# Server\r\n";
    ss << "uptime_in_seconds:" << uptimeSeconds() << "\r\n";
    ss << "\r\n";
    ss << "# Stats\r\n";
    ss << "total_commands_processed:" << totalCommands << "\r\n";
    ss << "total_connections_received:" << totalConnections << "\r\n";
    ss << "total_keys_evicted:" << totalKeysEvicted << "\r\n";
    ss << "connected_clients:" << currentConnections << "\r\n";
    ss << "\r\n";
    ss << "# Keyspace\r\n";
    ss << "total_keys:" << keyCount << "\r\n";
    return ss.str();
}