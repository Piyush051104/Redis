# 🚀 RedisAntiGravity

A **Redis-compatible in-memory database server** built from scratch in C++ using raw TCP sockets, multithreading, and the RESP (Redis Serialization Protocol). This project demonstrates core systems programming concepts including concurrent data access, network programming, persistence, and memory management.

---

## 📌 Features

- **TCP Server** — Custom socket-based server listening on port `6379`
- **RESP Protocol** — Full implementation of Redis Serialization Protocol for client-server communication
- **Thread-per-Client Model** — Each connected client gets a dedicated thread for concurrent request handling
- **Concurrency Control** — `shared_mutex` with `shared_lock` (reads) and `unique_lock` (writes) to prevent race conditions
- **LRU Eviction** — Least Recently Used cache eviction using `std::list` + `std::unordered_map` for O(1) access and eviction
- **TTL & Expiry** — Background thread that automatically cleans up expired keys every 100ms
- **AOF Persistence** — Append-Only File logging for crash recovery; all write commands are saved to disk and replayed on restart
- **Multiple Data Types** — Supports Strings, Lists, Sets, and Hashes (similar to real Redis)
- **Custom CLI Client** — Interactive REPL client with RESP encoding/decoding and formatted output

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────────────────┐
│                     redis-server                     │
│                                                      │
│  ┌─────────┐  ┌─────────────┐  ┌──────────────────┐ │
│  │  Main   │  │   Expiry    │  │  Client Threads  │ │
│  │ Thread  │  │  Manager    │  │  (one per client) │ │
│  │         │  │  (Thread 2) │  │                   │ │
│  │ accept()│  │  cleanup()  │  │  recv() → parse   │ │
│  │  loop   │  │  every 100ms│  │  → execute → send │ │
│  └─────────┘  └─────────────┘  └──────────────────┘ │
│                       │                │             │
│                       ▼                ▼             │
│              ┌──────────────────────────────┐        │
│              │       Store (RAM Database)    │        │
│              │  ┌──────┐ ┌──────┐ ┌───────┐ │        │
│              │  │ Data │ │Expiry│ │  LRU  │ │        │
│              │  │ Map  │ │ Map  │ │ Cache │ │        │
│              │  └──────┘ └──────┘ └───────┘ │        │
│              └──────────────────────────────┘        │
│                       │                              │
│                       ▼                              │
│              ┌──────────────────┐                    │
│              │   AOF (Disk)     │                    │
│              │ appendonly.aof   │                    │
│              └──────────────────┘                    │
└──────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────┐
│                     redis-client                     │
│                                                      │
│  REPL Loop: input → tokenize → buildRESP → send     │
│             recv → parseResponse → display           │
└──────────────────────────────────────────────────────┘
```

---

## 📂 Project Structure

```
RedisAntiGravity/
├── main.cpp                    # Entry point — AOF restore, expiry manager, server start
├── CMakeLists.txt              # Build configuration
├── include/
│   ├── server.h                # Server and client handler declarations
│   ├── store.h                 # Key-value store with LRU and expiry
│   ├── command_handler.h       # Command routing and RESP response helpers
│   ├── resp_parser.h           # RESP protocol decoder
│   ├── aof.h                   # Append-Only File persistence
│   ├── lru.h                   # LRU eviction cache (list + hashmap)
│   ├── expiry_manager.h        # Background TTL cleanup thread
│   ├── data_types.h            # List, Set, and Hash store declarations
│   └── stats.h                 # Server statistics tracker
├── src/
│   ├── server.cpp              # TCP listener, accept loop, thread spawning
│   ├── client.cpp              # Interactive CLI client with RESP encoding
│   ├── store.cpp               # Core key-value operations with mutex locking
│   ├── command_handler.cpp     # Routes commands to appropriate handlers
│   ├── resp_parser.cpp         # Parses RESP wire format into token vectors
│   ├── aof.cpp                 # Disk persistence — append and load
│   ├── lru.cpp                 # O(1) LRU cache using list + unordered_map
│   ├── expiry_manager.cpp      # Detached background thread for TTL cleanup
│   ├── data_types.cpp          # List, Set, and Hash data type implementations
│   └── stats.cpp               # Uptime, command count, connection tracking
└── data/
    └── appendonly.aof           # Persistent command log (auto-generated)
```

---

## 🛠️ Supported Commands

### String Commands
| Command | Usage | Description |
|---------|-------|-------------|
| `SET` | `SET key value` | Store a key-value pair |
| `GET` | `GET key` | Retrieve value by key |
| `DEL` | `DEL key` | Delete a key |
| `EXISTS` | `EXISTS key` | Check if key exists |
| `KEYS` | `KEYS` | List all keys |
| `SETEX` | `SETEX key seconds value` | Set key with expiry |
| `EXPIRE` | `EXPIRE key seconds` | Set TTL on existing key |
| `TTL` | `TTL key` | Check remaining TTL |

### List Commands
| Command | Usage | Description |
|---------|-------|-------------|
| `LPUSH` | `LPUSH key value` | Push to front of list |
| `RPUSH` | `RPUSH key value` | Push to back of list |
| `LPOP` | `LPOP key` | Pop from front |
| `RPOP` | `RPOP key` | Pop from back |
| `LRANGE` | `LRANGE key start end` | Get elements in range |
| `LLEN` | `LLEN key` | Get list length |

### Set Commands
| Command | Usage | Description |
|---------|-------|-------------|
| `SADD` | `SADD key member` | Add member to set |
| `SREM` | `SREM key member` | Remove member from set |
| `SMEMBERS` | `SMEMBERS key` | List all members |
| `SISMEMBER` | `SISMEMBER key member` | Check membership |
| `SCARD` | `SCARD key` | Get set size |

### Hash Commands
| Command | Usage | Description |
|---------|-------|-------------|
| `HSET` | `HSET key field value` | Set hash field |
| `HGET` | `HGET key field` | Get hash field value |
| `HDEL` | `HDEL key field` | Delete hash field |
| `HGETALL` | `HGETALL key` | Get all fields and values |
| `HLEN` | `HLEN key` | Get number of fields |

### Server Commands
| Command | Usage | Description |
|---------|-------|-------------|
| `PING` | `PING` | Health check (returns PONG) |
| `INFO` | `INFO` | Server statistics |
| `FLUSHALL` | `FLUSHALL` | Delete all keys |

---

## 🚀 Getting Started

### Prerequisites
- **C++17** compiler (MSVC / MinGW)
- **CMake 3.10+**
- **Windows OS** (uses Winsock2 for networking)

### Build
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
```

### Run

**Terminal 1 — Start the server:**
```bash
./Debug/redis-server.exe
```

**Terminal 2 — Connect the client:**
```bash
./Debug/redis-client.exe
```

### Example Session
```
Connected to server at 127.0.0.1:6379
127.0.0.1:6379> SET name Piyush
OK
127.0.0.1:6379> GET name
"Piyush"
127.0.0.1:6379> LPUSH fruits apple
(integer) 1
127.0.0.1:6379> LPUSH fruits banana
(integer) 2
127.0.0.1:6379> LRANGE fruits 0 -1
1) "banana"
2) "apple"
127.0.0.1:6379> HSET user name Piyush
OK
127.0.0.1:6379> HGETALL user
1) "name"
2) "Piyush"
127.0.0.1:6379> INFO
# Server
uptime_in_seconds:42

# Stats
total_commands_processed:7
connected_clients:1

# Keyspace
total_keys:1
```

---

## 🔑 Key Technical Decisions

| Decision | Rationale |
|----------|-----------|
| **Thread-per-Client** over Event Loop | Simpler to implement; each client gets isolated execution context |
| **`shared_mutex`** for Store | Allows multiple concurrent reads (`shared_lock`) while exclusive writes (`unique_lock`) prevent race conditions |
| **`std::list` + `std::unordered_map`** for LRU | O(1) insert, access, and eviction — the classic LRU interview solution |
| **`std::deque`** for Lists | O(1) push/pop on both ends AND O(1) random access by index |
| **AOF over RDB** for Persistence | Every write is logged immediately; safer against data loss on crash |
| **`flush()`** after every AOF write | Guarantees data reaches disk immediately, not stuck in OS buffer |

---


