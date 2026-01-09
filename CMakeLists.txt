cmake_minimum_required(VERSION 3.16)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/Main.cpp
    src/core/Server.cpp
    src/core/Connection.cpp
    src/core/HTTPServer.cpp
    src/core/HTTPCommon.cpp
    src/core/Utils.cpp
    src/handler/Handler.cpp
    src/tpool/Queue.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)