#include "aof.h"
#include <iostream>
#include <sstream>

AOF::AOF(const std::string& filepath) : filepath(filepath) {
    // Open file in append mode
    // ios::app = never overwrite, always add to end
    // ios::out = open for writing
    writeStream.open(filepath,
                     std::ios::app | std::ios::out);

    if (!writeStream.is_open()) {
        std::cerr << "Failed to open AOF file: "
                  << filepath << "\n";
    }
}

AOF::~AOF() {
    if (writeStream.is_open()) {
        writeStream.close();
    }
}

void AOF::append(const std::vector<std::string>& tokens) {
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

std::vector<std::vector<std::string>> AOF::load() {
    std::vector<std::vector<std::string>> commands;

    // Open file for reading
    std::ifstream readStream(filepath);
    if (!readStream.is_open()) {
        std::cout << "No AOF file found. Starting fresh.\n";
        return commands;
    }

    std::string line;
    while (std::getline(readStream, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Split line into tokens
        std::vector<std::string> tokens;
        std::istringstream ss(line);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            commands.push_back(tokens);
        }
    }

    std::cout << "AOF: loaded " << commands.size()
              << " commands from disk\n";
    return commands;
}

bool AOF::exists() {
    std::ifstream f(filepath);
    return f.good();
}