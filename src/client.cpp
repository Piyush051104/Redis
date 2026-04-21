#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

// Build RESP protocol message from command tokens
// Example: ["SET", "name", "Piyush"] →
// "*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$6\r\nPiyush\r\n"
std::string buildRESP(const std::vector<std::string>& tokens) {
    std::string resp = "*" + std::to_string(tokens.size()) + "\r\n";
    for (const auto& token : tokens) {
        resp += "$" + std::to_string(token.size()) + "\r\n";
        resp += token + "\r\n";
    }
    return resp;
}

// Parse server response into human readable format
// Converts RESP response → clean output
std::string parseResponse(const std::string& response) {
    if (response.empty()) return "(empty)";

    char type = response[0];
    std::string body = response.substr(1);

    // Remove trailing \r\n
    while (!body.empty() &&
          (body.back() == '\r' || body.back() == '\n')) {
        body.pop_back();
    }

    switch (type) {
        case '+':
            // Simple string → print as is
            return body;

        case '-':
            // Error → print with (error) prefix
            return "(error) " + body;

        case ':':
            // Integer
            return "(integer) " + body;

        case '$': {
            // Bulk string
            int len = std::stoi(body);
            if (len == -1) return "(nil)";

            // Find the actual string after \r\n
            size_t pos = response.find("\r\n");
            if (pos == std::string::npos) return "(nil)";
            std::string value = response.substr(pos + 2);

            // Remove trailing \r\n
            while (!value.empty() &&
                  (value.back() == '\r' || value.back() == '\n')) {
                value.pop_back();
            }
            return "\"" + value + "\"";
        }

        case '*': {
            // Array (for KEYS command)
            int count = std::stoi(body);
            if (count == 0) return "(empty array)";

            // Split response lines
            std::istringstream ss(response);
            std::string line;
            std::vector<std::string> items;

            // Skip first line (*N)
            std::getline(ss, line);

            while (std::getline(ss, line)) {
                // Remove \r
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                // Skip $N length lines
                if (!line.empty() && line[0] == '$') continue;
                // Skip empty lines
                if (line.empty()) continue;
                items.push_back(line);
            }

            // Format as numbered list
            std::string result;
            for (int i = 0; i < (int)items.size(); i++) {
                result += std::to_string(i + 1) + ") \"" + items[i] + "\"";
                if (i < (int)items.size() - 1) result += "\n";
            }
            return result;
        }

        default:
            return response;
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Connect to server
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6379);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serverAddr,
                sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Could not connect to server.\n";
        std::cerr << "Make sure redis-server is running!\n";
        return 1;
    }

    std::cout << "Connected to server at 127.0.0.1:6379\n";

    char buffer[4096];
    std::string input;

    // Main REPL loop
    // REPL = Read, Evaluate, Print, Loop
    while (true) {
        // Print prompt
        std::cout << "127.0.0.1:6379> ";
        std::getline(std::cin, input);

        // Skip empty input
        if (input.empty()) continue;

        // Handle EXIT command locally
        if (input == "EXIT" || input == "exit" ||
            input == "QUIT" || input == "quit") {
            std::cout << "Bye!\n";
            break;
        }

        // Split input into tokens
        std::vector<std::string> tokens;
        std::istringstream ss(input);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        // Build RESP message
        std::string resp = buildRESP(tokens);

        // Send to server
        send(sock, resp.c_str(), resp.size(), 0);

        // Receive response
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            std::cout << "Server disconnected\n";
            break;
        }

        // Parse and print response
        std::string response(buffer, bytesRead);
        std::cout << parseResponse(response) << "\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}