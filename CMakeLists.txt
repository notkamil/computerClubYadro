cmake_minimum_required(VERSION 3.29)
project(Club)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(Club src/main.cpp
        src/Club.cpp
        include/Club/Club.h
        src/Events.cpp
        include/Club/Events.h
        src/Utils.cpp
        include/Club/Utils.h)
target_include_directories(Club PUBLIC include)
