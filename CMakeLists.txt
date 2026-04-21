cmake_minimum_required(VERSION 3.10)
project(redis-cpp)

set(CMAKE_CXX_STANDARD 17)

# Server executable
add_executable(redis-server
    main.cpp
    server.cpp
    store.cpp
    resp_parser.cpp
    command_handler.cpp
)
target_link_libraries(redis-server ws2_32)

# Client executable
add_executable(redis-client
    client.cpp
)
target_link_libraries(redis-client ws2_32)