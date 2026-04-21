#include "server.h"
#include "store.h"
#include "aof.h"
#include "stats.h"
#include "resp_parser.h"
#include "command_handler.h"

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

extern Store store;
extern AOF aof;
extern Stats stats;
CommandHandler handler(store, aof, stats);

void handleClient(SOCKET clientFd) {
    // Track connections
    stats.totalConnections++;
    stats.currentConnections++;

    char buffer[4096];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientFd, buffer,
                             sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::string raw(buffer, bytesRead);
        std::vector<std::string> tokens = parseRESP(raw);
        std::string response = handler.handle(tokens);

        // Count every command
        stats.totalCommands++;

        send(clientFd, response.c_str(), response.size(), 0);
    }

    stats.currentConnections--;
    closesocket(clientFd);
}

void startServer(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverFd, (struct sockaddr*)&addr,
             sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind\n";
        return;
    }

    if (listen(serverFd, 10) == SOCKET_ERROR) {
        std::cerr << "Failed to listen\n";
        return;
    }

    std::cout << "Server started on port " << port << "\n";
    std::cout << "Waiting for connections...\n";

    while (true) {
        struct sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);

        SOCKET clientFd = accept(serverFd,
                                (struct sockaddr*)&clientAddr,
                                &clientLen);

        if (clientFd == INVALID_SOCKET) continue;

        std::cout << "New client connected!\n";
        std::thread t(handleClient, clientFd);
        t.detach();
    }

    closesocket(serverFd);
    WSACleanup();
}