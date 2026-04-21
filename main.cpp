#include "server.h"

int main() {
    // Start server on Redis default port
    startServer(6379);
    return 0;
}