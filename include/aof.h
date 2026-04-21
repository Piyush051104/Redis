#pragma once

#include <string>
#include <vector>
#include <fstream>

class AOF {
private:
    // Path to AOF file
    std::string filepath;

    // File stream for writing
    std::ofstream writeStream;

public:
    AOF(const std::string& filepath);
    ~AOF();

    // Append a command to AOF file
    void append(const std::vector<std::string>& tokens);

    // Read all commands from AOF file on startup
    std::vector<std::vector<std::string>> load();

    // Check if AOF file exists
    bool exists();
};