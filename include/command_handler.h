#pragma once

#include <string>
#include <vector>
#include "store.h"

class CommandHandler {
private:
    Store& store;

public:
    CommandHandler(Store& store);
    std::string handle(const std::vector<std::string>& tokens);

private:
    std::string handlePing();
    std::string handleSet(const std::vector<std::string>& tokens);
    std::string handleGet(const std::vector<std::string>& tokens);
    std::string handleDel(const std::vector<std::string>& tokens);
    std::string handleExists(const std::vector<std::string>& tokens);
    std::string handleKeys();
    std::string handleExpire(const std::vector<std::string>& tokens);
    std::string handleTTL(const std::vector<std::string>& tokens);
    std::string handleSetEx(const std::vector<std::string>& tokens);

    std::string ok();
    std::string error(const std::string& msg);
    std::string integer(int n);
    std::string bulkString(const std::string& s);
    std::string nilResponse();
};