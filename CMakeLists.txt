cmake_minimum_required(VERSION 3.10)
project(redis-cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(redis-server
    main.cpp
    src/server.cpp
    src/store.cpp
    src/resp_parser.cpp
    src/command_handler.cpp
    src/expiry_manager.cpp
    src/aof.cpp
    src/data_types.cpp
    src/lru.cpp
    src/stats.cpp
)
target_link_libraries(redis-server ws2_32)

add_executable(redis-client
    src/client.cpp
)
target_link_libraries(redis-client ws2_32)