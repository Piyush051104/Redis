#include "server.h"
#include "store.h"
#include "expiry_manager.h"

// Global store
Store store;

int main() {
    // Start background expiry cleanup thread
    ExpiryManager expiryManager(store);
    expiryManager.start();

    // Start TCP server
    startServer(6379);

    return 0;
}