#include "server.h"
#include "store.h"
#include "aof.h"
#include "command_handler.h"
#include "expiry_manager.h"
#include "stats.h"
#include "data_types.h"
#include <iostream>

using namespace std;

// Global objects shared across all files
Store store(1000); // max 1000 keys
AOF aof("../data/appendonly.aof");
Stats stats;
ListStore listStore;
SetStore setStore;
HashStore hashStore;

int main() {
    // Step 1 — Restore from AOF
    auto commands = aof.load();
    if (!commands.empty()) {
        cout << "Restoring " << commands.size()
             << " commands from AOF...\n";
        for (const auto& tokens : commands) {
            if (tokens[0] == "SET" && tokens.size() >= 3)
                store.set(tokens[1], tokens[2]);
            else if (tokens[0] == "DEL" && tokens.size() >= 2)
                store.del(tokens[1]);
            else if (tokens[0] == "SETEX" && tokens.size() >= 4)
                store.setex(tokens[1], tokens[3],
                            stoi(tokens[2]));
            else if (tokens[0] == "EXPIRE" && tokens.size() >= 3)
                store.expire(tokens[1], stoi(tokens[2]));
            // List commands
            else if (tokens[0] == "LPUSH" && tokens.size() >= 3)
                listStore.lpush(tokens[1], tokens[2]);
            else if (tokens[0] == "RPUSH" && tokens.size() >= 3)
                listStore.rpush(tokens[1], tokens[2]);
            else if (tokens[0] == "LPOP" && tokens.size() >= 2)
                listStore.lpop(tokens[1]);
            else if (tokens[0] == "RPOP" && tokens.size() >= 2)
                listStore.rpop(tokens[1]);
            // Set commands
            else if (tokens[0] == "SADD" && tokens.size() >= 3)
                setStore.sadd(tokens[1], tokens[2]);
            else if (tokens[0] == "SREM" && tokens.size() >= 3)
                setStore.srem(tokens[1], tokens[2]);
            // Hash commands
            else if (tokens[0] == "HSET" && tokens.size() >= 4)
                hashStore.hset(tokens[1], tokens[2], tokens[3]);
            else if (tokens[0] == "HDEL" && tokens.size() >= 3)
                hashStore.hdel(tokens[1], tokens[2]);
        }
        cout << "Data restored successfully!\n";
    }

    // Step 2 — Start expiry manager
    ExpiryManager expiryManager(store);
    expiryManager.start();

    // Step 3 — Start server
    startServer(6379);

    return 0;
}
