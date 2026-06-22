#include "resp_parser.h"
#include <sstream>
#include <iostream>

using namespace std;

vector<string> parseRESP(const string& raw) {
    vector<string> result;
    istringstream ss(raw);
    string line;

    // Read first line — should be *N (array of N elements)
    if (!getline(ss, line)) return result;

    // Remove \r if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    // Must start with * for array
    if (line.empty() || line[0] != '*') {
        // Fallback — treat as plain text command (for telnet testing)
        istringstream plain(raw);
        string token;
        while (plain >> token) {
            result.push_back(token);
        }
        return result;
    }

    // Parse number of elements
    int numElements = stoi(line.substr(1));

    for (int i = 0; i < numElements; i++) {
        // Read $N line (length of next string)
        if (!getline(ss, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Must start with $
        if (line.empty() || line[0] != '$') break;

        int strLen = stoi(line.substr(1));

        // Read actual string value
        if (!getline(ss, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        result.push_back(line);
    }

    return result;
}
