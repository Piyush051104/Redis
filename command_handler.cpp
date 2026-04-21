#include "command_handler.h"
#include <algorithm>

CommandHandler::CommandHandler(Store& store) : store(store) {}

std::string CommandHandler::handle(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return error("empty command");

    // Convert command to uppercase
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (cmd == "PING")   return handlePing();
    if (cmd == "SET")    return handleSet(tokens);
    if (cmd == "GET")    return handleGet(tokens);
    if (cmd == "DEL")    return handleDel(tokens);
    if (cmd == "EXISTS") return handleExists(tokens);
    if (cmd == "KEYS")   return handleKeys();

    return error("unknown command '" + tokens[0] + "'");
}

// --- Command Handlers ---

std::string CommandHandler::handlePing() {
    return "+PONG\r\n";
}

std::string CommandHandler::handleSet(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return error("wrong number of arguments for SET");
    }
    store.set(tokens[1], tokens[2]);
    return ok();
}

std::string CommandHandler::handleGet(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return error("wrong number of arguments for GET");
    }
    auto value = store.get(tokens[1]);
    if (!value.has_value()) {
        return nilResponse();
    }
    return bulkString(value.value());
}

std::string CommandHandler::handleDel(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return error("wrong number of arguments for DEL");
    }
    bool deleted = store.del(tokens[1]);
    return integer(deleted ? 1 : 0);
}

std::string CommandHandler::handleExists(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return error("wrong number of arguments for EXISTS");
    }
    bool found = store.exists(tokens[1]);
    return integer(found ? 1 : 0);
}

std::string CommandHandler::handleKeys() {
    auto allKeys = store.keys();
    // Build RESP array response
    std::string response = "*" + std::to_string(allKeys.size()) + "\r\n";
    for (const auto& k : allKeys) {
        response += bulkString(k);
    }
    return response;
}

// --- RESP Response Helpers ---

std::string CommandHandler::ok() {
    return "+OK\r\n";
}

std::string CommandHandler::error(const std::string& msg) {
    return "-ERR " + msg + "\r\n";
}

std::string CommandHandler::integer(int n) {
    return ":" + std::to_string(n) + "\r\n";
}

std::string CommandHandler::bulkString(const std::string& s) {
    return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
}

std::string CommandHandler::nilResponse() {
    return "$-1\r\n";
}