cmake_minimum_required(VERSION 3.29)
project(Club)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(Club src/main.cpp
        src/Club.cpp
        include/Club/Club.h)
target_include_directories(Club PUBLIC include)
