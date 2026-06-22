#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

// Build RESP protocol message from command tokens
// Example: ["SET", "name", "Piyush"] →
// "*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$6\r\nPiyush\r\n"
string buildRESP(const vector<string>& tokens) {
    string resp = "*" + to_string(tokens.size()) + "\r\n";
    for (const auto& token : tokens) {
        resp += "$" + to_string(token.size()) + "\r\n";
        resp += token + "\r\n";
    }
    return resp;
}

// Parse server response into human readable format
// Converts RESP response → clean output
string parseResponse(const string& response) {
    if (response.empty()) return "(empty)";

    char type = response[0];
    string body = response.substr(1);

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
            int len = stoi(body);
            if (len == -1) return "(nil)";

            // Find the actual string after \r\n
            size_t pos = response.find("\r\n");
            if (pos == string::npos) return "(nil)";
            string value = response.substr(pos + 2);

            // Remove trailing \r\n
            while (!value.empty() &&
                  (value.back() == '\r' || value.back() == '\n')) {
                value.pop_back();
            }
            return "\"" + value + "\"";
        }

        case '*': {
            // Array (for KEYS command)
            int count = stoi(body);
            if (count == 0) return "(empty array)";

            // Split response lines
            istringstream ss(response);
            string line;
            vector<string> items;

            // Skip first line (*N)
            getline(ss, line);

            while (getline(ss, line)) {
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
            string result;
            for (int i = 0; i < (int)items.size(); i++) {
                result += to_string(i + 1) + ") \"" + items[i] + "\"";
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
        cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Failed to create socket\n";
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
        cerr << "Could not connect to server.\n";
        cerr << "Make sure redis-server is running!\n";
        return 1;
    }

    cout << "Connected to server at 127.0.0.1:6379\n";

    char buffer[4096];
    string input;

    // Main REPL loop
    // REPL = Read, Evaluate, Print, Loop
    while (true) {
        // Print prompt
        cout << "127.0.0.1:6379> ";
        getline(cin, input);

        // Skip empty input
        if (input.empty()) continue;

        // Handle EXIT command locally
        if (input == "EXIT" || input == "exit" ||
            input == "QUIT" || input == "quit") {
            cout << "Bye!\n";
            break;
        }

        // Split input into tokens
        vector<string> tokens;
        istringstream ss(input);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        // Build RESP message
        string resp = buildRESP(tokens);

        // Send to server
        send(sock, resp.c_str(), resp.size(), 0);

        // Receive response
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            cout << "Server disconnected\n";
            break;
        }

        // Parse and print response
        string response(buffer, bytesRead);
        cout << parseResponse(response) << "\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
