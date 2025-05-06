cmake_minimum_required(VERSION 3.29)
project(Club)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable and library
add_library(ClubLib
        src/Club.cpp
        src/Events.cpp
        src/Utils.cpp
)
target_include_directories(ClubLib PUBLIC include)

add_executable(Club
        src/main.cpp
)
target_link_libraries(Club PRIVATE ClubLib)

# Google Test integration
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Test executable configuration
add_executable(ClubTests tests/tests.cpp)

# Use generator expression for executable path
target_compile_definitions(ClubTests PRIVATE
        "CLUB_EXE_PATH=\"$<TARGET_FILE:Club>\""
)

target_link_libraries(ClubTests
        PRIVATE
        ClubLib
        GTest::gtest_main
)

# Build dependencies
add_dependencies(ClubTests Club)

# Test data handling
add_custom_command(TARGET ClubTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tests/data
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/data
        ${CMAKE_CURRENT_BINARY_DIR}/tests/data
        COMMENT "Copying test data files"
)

# Test configuration
enable_testing()
add_test(
        NAME ClubTests
        COMMAND ClubTests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)