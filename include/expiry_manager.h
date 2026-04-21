#pragma once
#include "store.h"

class ExpiryManager {
private:
    Store& store;
    bool running;

public:
    ExpiryManager(Store& store);

    // Starts background cleanup thread
    void start();

    // Stops the background thread
    void stop();
};