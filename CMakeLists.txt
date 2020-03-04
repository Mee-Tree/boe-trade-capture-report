cmake_minimum_required(VERSION 3.0)

set(CMAKE_CONFIGURATION_TYPES "ASAN;MSAN;USAN" CACHE STRING "" FORCE)

set(PROJECT_NAME boe_trade_capture_report)
project(${PROJECT_NAME})

# Set up the compiler flags
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inlcude directories
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

# Source files
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Separate executable: main
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)

# Compile source files into a library
add_library(boe_trade_capture_report_lib ${SRC_FILES})

# Main is separate
add_executable(boe_trade_capture_report ${PROJECT_SOURCE_DIR}/src/main.cpp)

# linking Main against the library
target_link_libraries(boe_trade_capture_report boe_trade_capture_report_lib)

# google test is a git submodule
add_subdirectory(./googletest)

enable_testing()

# Include the gtest library
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Test files
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

# Unit tests
add_executable(runUnitTests ${TEST_SRC_FILES})

# Standard linking to gtest stuff
target_link_libraries(runUnitTests gtest gtest_main)

# Extra linking for the project
target_link_libraries(runUnitTests boe_trade_capture_report_lib)

# Sanitizers modes
if (CMAKE_BUILD_TYPE MATCHES ASAN)
    target_compile_options(runUnitTests PRIVATE -fsanitize=address -fno-omit-frame-pointer -fno-sanitize-recover)
    target_link_options(runUnitTests PRIVATE -fsanitize=address)
endif()
if (CMAKE_BUILD_TYPE MATCHES MSAN)
    target_compile_options(runUnitTests PRIVATE -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins=2 -fno-sanitize-recover)
    target_link_options(runUnitTests PRIVATE -fsanitize=memory -fsanitize-memory-track-origins=2)
endif()
if (CMAKE_BUILD_TYPE MATCHES USAN)
    target_compile_options(runUnitTests PRIVATE -fsanitize=undefined -fno-omit-frame-pointer -fno-sanitize-recover)
    target_link_options(runUnitTests PRIVATE -fsanitize=undefined)
endif()
