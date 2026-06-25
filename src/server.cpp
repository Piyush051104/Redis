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

using namespace std;

extern Store store;
extern AOF aof;
extern Stats stats;
extern ListStore listStore;
extern SetStore setStore;
extern HashStore hashStore;

CommandHandler handler(store, aof, stats, listStore, setStore, hashStore);


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
            cout << "Client disconnected\n";
            break;
        }

        string raw(buffer, bytesRead);
        vector<string> tokens = parseRESP(raw);
        string response = handler.handle(tokens);

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
        cerr << "WSAStartup failed\n";
        return;
    }

    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == INVALID_SOCKET) {
        cerr << "Failed to create socket\n";
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
        cerr << "Failed to bind\n";
        return;
    }

    if (listen(serverFd, 10) == SOCKET_ERROR) {
        cerr << "Failed to listen\n";
        return;
    }

    cout << "Server started on port " << port << "\n";
    cout << "Waiting for connections...\n";

    while (true) {
        struct sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);

        SOCKET clientFd = accept(serverFd,
                                (struct sockaddr*)&clientAddr,
                                &clientLen);

        if (clientFd == INVALID_SOCKET) continue;

        cout << "New client connected!\n";
        thread t(handleClient, clientFd);
        t.detach();
    }

    closesocket(serverFd);
    WSACleanup();
}
