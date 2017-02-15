cmake_minimum_required(VERSION 3.6)
project(Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        server.cpp
        server.h)

add_executable(Server ${SOURCE_FILES})