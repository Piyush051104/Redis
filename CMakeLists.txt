cmake_minimum_required(VERSION 3.10)
project(redis-cpp)

set(CMAKE_CXX_STANDARD 17)

# Tell compiler where to find header files
include_directories(include)

# Server executable
add_executable(redis-server
    main.cpp
    src/server.cpp
    src/store.cpp
    src/resp_parser.cpp
    src/command_handler.cpp
    src/expiry_manager.cpp
)
target_link_libraries(redis-server ws2_32)

# Client executable
add_executable(redis-client
    src/client.cpp
)
target_link_libraries(redis-client ws2_32)