cmake_minimum_required(VERSION 3.16)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/Main.cpp
    src/Server.cpp
    src/Connection.cpp
    src/HTTPServer.cpp
    src/HTTPConnection.cpp
    src/HTTPCommon.cpp
    src/Utils.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)