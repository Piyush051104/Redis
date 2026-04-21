#pragma once

#include <string>
#include <vector>

// Parses raw RESP bytes into a list of strings
// Example: "*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$6\r\nPiyush\r\n"
// Result:  ["SET", "name", "Piyush"]
std::vector<std::string> parseRESP(const std::string& raw);