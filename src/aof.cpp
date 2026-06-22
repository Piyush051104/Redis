#include "aof.h"
#include <iostream>
#include <sstream>

using namespace std;

AOF::AOF(const string& filepath) : filepath(filepath) {
    // Open file in append mode
    // ios::app = never overwrite, always add to end
    // ios::out = open for writing
    writeStream.open(filepath, ios::app | ios::out);

    if (!writeStream.is_open()) {
        cerr << "Failed to open AOF file: "
             << filepath << "\n";
    }
}

AOF::~AOF() {
    if (writeStream.is_open()) {
        writeStream.close();
    }
}

void AOF::append(const vector<string>& tokens) {
    if (!writeStream.is_open()) return;
    if (tokens.empty()) return;

    // Write command as single line
    // Format: SET name Piyush
    for (int i = 0; i < (int)tokens.size(); i++) {
        writeStream << tokens[i];
        if (i < (int)tokens.size() - 1) {
            writeStream << " ";
        }
    }
    writeStream << "\n";

    // Critical — force write to disk immediately
    // Without this, data stays in buffer and is lost on crash
    writeStream.flush();
}

vector<vector<string>> AOF::load() {
    vector<vector<string>> commands;

    // Open file for reading
    ifstream readStream(filepath);
    if (!readStream.is_open()) {
        cout << "No AOF file found. Starting fresh.\n";
        return commands;
    }

    string line;
    while (getline(readStream, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Split line into tokens
        vector<string> tokens;
        istringstream ss(line);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            commands.push_back(tokens);
        }
    }

    cout << "AOF: loaded " << commands.size()
         << " commands from disk\n";
    return commands;
}

bool AOF::exists() {
    ifstream f(filepath);
    return f.good();
}
