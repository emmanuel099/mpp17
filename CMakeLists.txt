project(SkipList LANGUAGES CXX)

set(CMAKE_VERSION 3.2)
set(GTEST_VERSION 1.8)
set(BOOST_VERSION 1.56.0)

cmake_minimum_required(VERSION ${CMAKE_VERSION})

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS thread)

find_program(CLANG_TIDY_PATH NAMES clang-tidy)
if(NOT CLANG_TIDY_PATH)
    message(FATAL_ERROR "Could not find clang-tidy")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(doc)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest ${GTEST_VERSION} REQUIRED)
    add_subdirectory(tests)
endif()

if(BUILD_BENCHMARKS)
    enable_testing()
    add_subdirectory(benchmarks)
endif()

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)
add_custom_target(format
    COMMAND clang-format -i ${SOURCE_FILES}
)
