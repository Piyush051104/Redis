#pragma once

#include <string>
#include <vector>
#include "store.h"
#include "aof.h"
#include "stats.h"
#include "data_types.h"

class CommandHandler {
private:
    Store& store;
    AOF& aof;
    Stats& stats;
    ListStore listStore;
    SetStore setStore;
    HashStore hashStore;

public:
    CommandHandler(Store& store, AOF& aof, Stats& stats);
    std::string handle(const std::vector<std::string>& tokens);

private:
    std::string handlePing();
    std::string handleSet(const std::vector<std::string>& t);
    std::string handleGet(const std::vector<std::string>& t);
    std::string handleDel(const std::vector<std::string>& t);
    std::string handleExists(const std::vector<std::string>& t);
    std::string handleKeys();
    std::string handleExpire(const std::vector<std::string>& t);
    std::string handleTTL(const std::vector<std::string>& t);
    std::string handleSetEx(const std::vector<std::string>& t);
    std::string handleInfo();
    std::string handleFlushAll();
    std::string handleLPush(const std::vector<std::string>& t);
    std::string handleRPush(const std::vector<std::string>& t);
    std::string handleLPop(const std::vector<std::string>& t);
    std::string handleRPop(const std::vector<std::string>& t);
    std::string handleLRange(const std::vector<std::string>& t);
    std::string handleLLen(const std::vector<std::string>& t);
    std::string handleSAdd(const std::vector<std::string>& t);
    std::string handleSRem(const std::vector<std::string>& t);
    std::string handleSMembers(const std::vector<std::string>& t);
    std::string handleSIsMember(const std::vector<std::string>& t);
    std::string handleSCard(const std::vector<std::string>& t);
    std::string handleHSet(const std::vector<std::string>& t);
    std::string handleHGet(const std::vector<std::string>& t);
    std::string handleHDel(const std::vector<std::string>& t);
    std::string handleHGetAll(const std::vector<std::string>& t);
    std::string handleHLen(const std::vector<std::string>& t);

    std::string ok();
    std::string error(const std::string& msg);
    std::string integer(int n);
    std::string bulkString(const std::string& s);
    std::string nilResponse();
    std::string arrayResponse(const std::vector<std::string>& items);
};