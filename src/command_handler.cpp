#include "command_handler.h"
#include <algorithm>

using namespace std;

CommandHandler::CommandHandler(Store& store,
                               AOF& aof,
                               Stats& stats,
                               ListStore& listStore,
                               SetStore& setStore,
                               HashStore& hashStore)
    : store(store), aof(aof), stats(stats),
      listStore(listStore), setStore(setStore),
      hashStore(hashStore) {}


string CommandHandler::handle(
        const vector<string>& tokens) {
    if (tokens.empty()) return error("empty command");

    string cmd = tokens[0];
    transform(cmd.begin(), cmd.end(),
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

// String commands
string CommandHandler::handlePing() { return "+PONG\r\n"; }

string CommandHandler::handleSet(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SET");
    optional<string> evicted;
    store.set(t[1], t[2], &evicted);
    if (evicted.has_value()) stats.totalKeysEvicted++;
    aof.append(t);
    return ok();
}

string CommandHandler::handleGet(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for GET");
    auto value = store.get(t[1]);
    if (!value.has_value()) return nilResponse();
    return bulkString(value.value());
}

string CommandHandler::handleDel(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for DEL");
    bool deleted = store.del(t[1]);
    if (deleted) aof.append(t);
    return integer(deleted ? 1 : 0);
}

string CommandHandler::handleExists(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for EXISTS");
    return integer(store.exists(t[1]) ? 1 : 0);
}

string CommandHandler::handleKeys() {
    return arrayResponse(store.keys());
}

string CommandHandler::handleExpire(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for EXPIRE");
    bool result = store.expire(t[1], stoi(t[2]));
    if (result) aof.append(t);
    return integer(result ? 1 : 0);
}

string CommandHandler::handleTTL(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for TTL");
    return integer(store.ttl(t[1]));
}

string CommandHandler::handleSetEx(
        const vector<string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for SETEX");
    store.setex(t[1], t[3], stoi(t[2]));
    aof.append(t);
    return ok();
}

string CommandHandler::handleInfo() {
    string info = stats.infoString(store.keyCount());
    return bulkString(info);
}

string CommandHandler::handleFlushAll() {
    // Clear all keys by deleting and recreating AOF
    auto allKeys = store.keys();
    for (const auto& key : allKeys) {
        store.del(key);
    }
    return ok();
}

// List commands
string CommandHandler::handleLPush(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for LPUSH");
    int len = listStore.lpush(t[1], t[2]);
    aof.append(t);
    return integer(len);
}

string CommandHandler::handleRPush(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for RPUSH");
    int len = listStore.rpush(t[1], t[2]);
    aof.append(t);
    return integer(len);
}

string CommandHandler::handleLPop(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for LPOP");
    auto val = listStore.lpop(t[1]);
    if (!val.has_value()) return nilResponse();
    aof.append(t);
    return bulkString(val.value());
}

string CommandHandler::handleRPop(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for RPOP");
    auto val = listStore.rpop(t[1]);
    if (!val.has_value()) return nilResponse();
    aof.append(t);
    return bulkString(val.value());
}

string CommandHandler::handleLRange(
        const vector<string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for LRANGE");
    auto items = listStore.lrange(t[1],
                                  stoi(t[2]),
                                  stoi(t[3]));
    return arrayResponse(items);
}

string CommandHandler::handleLLen(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for LLEN");
    return integer(listStore.llen(t[1]));
}

// Set commands
string CommandHandler::handleSAdd(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SADD");
    aof.append(t);
    return integer(setStore.sadd(t[1], t[2]));
}

string CommandHandler::handleSRem(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SREM");
    int result = setStore.srem(t[1], t[2]);
    if (result) aof.append(t);
    return integer(result);
}

string CommandHandler::handleSMembers(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for SMEMBERS");
    return arrayResponse(setStore.smembers(t[1]));
}

string CommandHandler::handleSIsMember(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for SISMEMBER");
    return integer(setStore.sismember(t[1], t[2]) ? 1 : 0);
}

string CommandHandler::handleSCard(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for SCARD");
    return integer(setStore.scard(t[1]));
}

// Hash commands
string CommandHandler::handleHSet(
        const vector<string>& t) {
    if (t.size() < 4) return error("wrong number of arguments for HSET");
    hashStore.hset(t[1], t[2], t[3]);
    aof.append(t);
    return ok();
}

string CommandHandler::handleHGet(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for HGET");
    auto val = hashStore.hget(t[1], t[2]);
    if (!val.has_value()) return nilResponse();
    return bulkString(val.value());
}

string CommandHandler::handleHDel(
        const vector<string>& t) {
    if (t.size() < 3) return error("wrong number of arguments for HDEL");
    bool deleted = hashStore.hdel(t[1], t[2]);
    if (deleted) aof.append(t);
    return integer(deleted ? 1 : 0);
}

string CommandHandler::handleHGetAll(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for HGETALL");
    return arrayResponse(hashStore.hgetall(t[1]));
}

string CommandHandler::handleHLen(
        const vector<string>& t) {
    if (t.size() < 2) return error("wrong number of arguments for HLEN");
    return integer(hashStore.hlen(t[1]));
}

// RESP helpers
string CommandHandler::ok() { return "+OK\r\n"; }

string CommandHandler::error(const string& msg) {
    return "-ERR " + msg + "\r\n";
}

string CommandHandler::integer(int n) {
    return ":" + to_string(n) + "\r\n";
}

string CommandHandler::bulkString(const string& s) {
    return "$" + to_string(s.size()) + "\r\n" + s + "\r\n";
}

string CommandHandler::nilResponse() { return "$-1\r\n"; }

string CommandHandler::arrayResponse(
        const vector<string>& items) {
    if (items.empty()) return "*0\r\n";
    string response = "*" +
                           to_string(items.size()) + "\r\n";
    for (const auto& item : items) {
        response += bulkString(item);
    }
    return response;
}
