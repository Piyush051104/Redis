#include "expiry_manager.h"
#include <thread>
#include <chrono>
#include <iostream>

ExpiryManager::ExpiryManager(Store& store)
    : store(store), running(false) {}

void ExpiryManager::start() {
    running = true;

    // Launch background thread
    std::thread t([this]() {
        while (running) {
            // Sleep 100ms between cleanups
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
            // Delete all expired keys
            store.cleanupExpired();
        }
    });

    // Detach — runs independently in background
    t.detach();

    std::cout << "Expiry manager started\n";
}

void ExpiryManager::stop() {
    running = false;
}