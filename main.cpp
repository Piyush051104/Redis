#include "server.h"
#include "store.h"
#include "aof.h"
#include "command_handler.h"
#include "expiry_manager.h"
#include "stats.h"
#include <iostream>

// Global objects shared across all files
Store store(1000); // max 1000 keys
AOF aof("../data/appendonly.aof");
Stats stats;

int main() {
    // Step 1 — Restore from AOF
    auto commands = aof.load();
    if (!commands.empty()) {
        std::cout << "Restoring " << commands.size()
                  << " commands from AOF...\n";
        for (const auto& tokens : commands) {
            if (tokens[0] == "SET" && tokens.size() >= 3)
                store.set(tokens[1], tokens[2]);
            else if (tokens[0] == "DEL" && tokens.size() >= 2)
                store.del(tokens[1]);
            else if (tokens[0] == "SETEX" && tokens.size() >= 4)
                store.setex(tokens[1], tokens[3],
                            std::stoi(tokens[2]));
            else if (tokens[0] == "EXPIRE" && tokens.size() >= 3)
                store.expire(tokens[1], std::stoi(tokens[2]));
        }
        std::cout << "Data restored successfully!\n";
    }

    // Step 2 — Start expiry manager
    ExpiryManager expiryManager(store);
    expiryManager.start();

    // Step 3 — Start server
    startServer(6379);

    return 0;
}