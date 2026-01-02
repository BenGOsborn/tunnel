cmake_minimum_required(VERSION 3.16)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/main.cpp
    src/Server.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)