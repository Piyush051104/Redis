#include "command_handler.h"
#include <algorithm>

CommandHandler::CommandHandler(Store& store,
                               AOF& aof,
                               Stats& stats)
    : store(store), aof(aof), stats(stats) {}

std::string CommandHandler::handle(
        const std::vector<std::string>& tokens) {
    if (tokens.empty()) return error("empty command");

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(),
                   cmd.begin(), ::toupper);

    if (cmd == "PING")      return handlePing();
    if (cmd == "SET")       return handleSet(tokens);
    if (cmd == "GET")       return handleGet(tokens);
    if (cmd == "DEL")       return handleDel(tokens);
    if (cmd == "EXISTS")    return handleExists(tokens);
    if (cmd == "KEYS")      return handleKeys();
    if (cmd == "EXPIRE")    return handleExpire(tokens);
    if (cmd == "TTL")       return handleTTL(tokens);
    if (cmd == "SETEX")     return handleSetEx(tokens);
    if (cmd == "INFO")      return handleInfo();
    if (cmd == "FLUSHALL")  return handleFlushAll();
    if (cmd == "LPUSH")     return handleLPush(tokens);
    if (cmd == "RPUSH")     return handleRPush(tokens);
    if (cmd == "LPOP")      return handleLPop(tokens);
    if (cmd == "RPOP")      return handleRPop(tokens);
    if (cmd == "LRANGE")    return handleLRange(tokens);
    if (cmd == "LLEN")      return handleLLen(tokens);
    if (cmd == "SADD")      return handleSAdd(tokens);
    if (cmd == "SREM")      return handleSRem(tokens);
    if (cmd == "SMEMBERS")  return handleSMembers(tokens);
    if (cmd == "SISMEMBER") return handleSIsMember(tokens);
    if (cmd == "SCARD")     return handleSCard(tokens);
    if (cmd == "HSET")      return handleHSet(tokens);
    if (cmd == "HGET")      return handleHGet(tokens);
    if (cmd == "HDEL")      return handleHDel(tokens);
    if (cmd == "HGETALL")   return handleHGetAll(tokens);
    if (cmd == "HLEN")      return handleHLen(tokens);

    return error("unknown command '" + tokens[0] + "'");
}

// String commands — same as Phase 5
std::string CommandHandler::handlePing() { return "+PONG\r\n"; }

std::string CommandHandler::handleSet(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SET");
    std::optional<std::string> evicted;
    store.set(t[1], t[2], &evicted);
    if (evicted.has_value()) stats.totalKeysEvicted++;
    aof.append(t);
    return ok();
}

std::string CommandHandler::handleGet(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for GET");
    auto value = store.get(t[1]);
    if (!value.has_value()) return nilResponse();
    return bulkString(value.value());
}

std::string CommandHandler::handleDel(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for DEL");
    bool deleted = store.del(t[1]);
    if (deleted) aof.append(t);
    return integer(deleted ? 1 : 0);
}

std::string CommandHandler::handleExists(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for EXISTS");
    return integer(store.exists(t[1]) ? 1 : 0);
}

std::string CommandHandler::handleKeys() {
    return arrayResponse(store.keys());
}

std::string CommandHandler::handleExpire(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for EXPIRE");
    bool result = store.expire(t[1], std::stoi(t[2]));
    if (result) aof.append(t);
    return integer(result ? 1 : 0);
}

std::string CommandHandler::handleTTL(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for TTL");
    return integer(store.ttl(t[1]));
}

std::string CommandHandler::handleSetEx(
        const std::vector<std::string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for SETEX");
    store.setex(t[1], t[3], std::stoi(t[2]));
    aof.append(t);
    return ok();
}

std::string CommandHandler::handleInfo() {
    std::string info = stats.infoString(store.keyCount());
    return bulkString(info);
}

std::string CommandHandler::handleFlushAll() {
    // Clear all keys by deleting and recreating AOF
    auto allKeys = store.keys();
    for (const auto& key : allKeys) {
        store.del(key);
    }
    return ok();
}

// List commands
std::string CommandHandler::handleLPush(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for LPUSH");
    int len = listStore.lpush(t[1], t[2]);
    aof.append(t);
    return integer(len);
}

std::string CommandHandler::handleRPush(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for RPUSH");
    int len = listStore.rpush(t[1], t[2]);
    aof.append(t);
    return integer(len);
}

std::string CommandHandler::handleLPop(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for LPOP");
    auto val = listStore.lpop(t[1]);
    if (!val.has_value()) return nilResponse();
    aof.append(t);
    return bulkString(val.value());
}

std::string CommandHandler::handleRPop(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for RPOP");
    auto val = listStore.rpop(t[1]);
    if (!val.has_value()) return nilResponse();
    aof.append(t);
    return bulkString(val.value());
}

std::string CommandHandler::handleLRange(
        const std::vector<std::string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for LRANGE");
    auto items = listStore.lrange(t[1],
                                  std::stoi(t[2]),
                                  std::stoi(t[3]));
    return arrayResponse(items);
}

std::string CommandHandler::handleLLen(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for LLEN");
    return integer(listStore.llen(t[1]));
}

// Set commands
std::string CommandHandler::handleSAdd(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SADD");
    aof.append(t);
    return integer(setStore.sadd(t[1], t[2]));
}

std::string CommandHandler::handleSRem(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SREM");
    int result = setStore.srem(t[1], t[2]);
    if (result) aof.append(t);
    return integer(result);
}

std::string CommandHandler::handleSMembers(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for SMEMBERS");
    return arrayResponse(setStore.smembers(t[1]));
}

std::string CommandHandler::handleSIsMember(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SISMEMBER");
    return integer(setStore.sismember(t[1], t[2]) ? 1 : 0);
}

std::string CommandHandler::handleSCard(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for SCARD");
    return integer(setStore.scard(t[1]));
}

// Hash commands
std::string CommandHandler::handleHSet(
        const std::vector<std::string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for HSET");
    hashStore.hset(t[1], t[2], t[3]);
    aof.append(t);
    return ok();
}

std::string CommandHandler::handleHGet(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for HGET");
    auto val = hashStore.hget(t[1], t[2]);
    if (!val.has_value()) return nilResponse();
    return bulkString(val.value());
}

std::string CommandHandler::handleHDel(
        const std::vector<std::string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for HDEL");
    bool deleted = hashStore.hdel(t[1], t[2]);
    if (deleted) aof.append(t);
    return integer(deleted ? 1 : 0);
}

std::string CommandHandler::handleHGetAll(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for HGETALL");
    return arrayResponse(hashStore.hgetall(t[1]));
}

std::string CommandHandler::handleHLen(
        const std::vector<std::string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for HLEN");
    return integer(hashStore.hlen(t[1]));
}

// RESP helpers
std::string CommandHandler::ok() { return "+OK\r\n"; }

std::string CommandHandler::error(const std::string& msg) {
    return "-ERR " + msg + "\r\n";
}

std::string CommandHandler::integer(int n) {
    return ":" + std::to_string(n) + "\r\n";
}

std::string CommandHandler::bulkString(const std::string& s) {
    return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
}

std::string CommandHandler::nilResponse() { return "$-1\r\n"; }

std::string CommandHandler::arrayResponse(
        const std::vector<std::string>& items) {
    if (items.empty()) return "*0\r\n";
    std::string response = "*" +
                           std::to_string(items.size()) + "\r\n";
    for (const auto& item : items) {
        response += bulkString(item);
    }
    return response;
}