cmake_minimum_required(VERSION 2.8.9)
project(withershins)

option(BUILD_TESTS "Build unit tests." ON)
option(ENABLE_WERROR "Fail and stop if a warning is triggered." ON)
option(OSX_ENABLE_LIBBFD "Use libbfd to find file names and line numbers." OFF)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR OSX_ENABLE_LIBBFD)
    add_definitions(-DWITHERSHINS_ENABLE_LIBBFD)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(src)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
    if (ENABLE_WERROR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()
    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()

    # Use libc++ when building with Clang.
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()

    # Print coloured error messages when compiling with Clang and using Ninja.
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
endif()

add_subdirectory(src)
if (BUILD_TESTS)
    add_subdirectory(test)
endif()
