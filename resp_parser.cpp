#include "resp_parser.h"
#include <sstream>
#include <iostream>

std::vector<std::string> parseRESP(const std::string& raw) {
    std::vector<std::string> result;
    std::istringstream ss(raw);
    std::string line;

    // Read first line — should be *N (array of N elements)
    if (!std::getline(ss, line)) return result;

    // Remove \r if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    // Must start with * for array
    if (line.empty() || line[0] != '*') {
        // Fallback — treat as plain text command (for telnet testing)
        std::istringstream plain(raw);
        std::string token;
        while (plain >> token) {
            result.push_back(token);
        }
        return result;
    }

    // Parse number of elements
    int numElements = std::stoi(line.substr(1));

    for (int i = 0; i < numElements; i++) {
        // Read $N line (length of next string)
        if (!std::getline(ss, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Must start with $
        if (line.empty() || line[0] != '$') break;

        int strLen = std::stoi(line.substr(1));

        // Read actual string value
        if (!std::getline(ss, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        result.push_back(line);
    }

    return result;
}