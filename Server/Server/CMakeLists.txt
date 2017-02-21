cmake_minimum_required(VERSION 3.6)
project(Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        server.cpp
        server.h)

find_package (Threads)

add_executable(Server ${SOURCE_FILES})
target_link_libraries (Server ${CMAKE_THREAD_LIBS_INIT})