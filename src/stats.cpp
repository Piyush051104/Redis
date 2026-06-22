#include "stats.h"
#include <sstream>

using namespace std;

Stats::Stats() {
    startTime = chrono::steady_clock::now();
}

long long Stats::uptimeSeconds() {
    auto now = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::seconds>
           (now - startTime).count();
}

string Stats::infoString(size_t keyCount) {
    ostringstream ss;
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
