cmake_minimum_required(VERSION 2.8.9)

add_executable(tests
    catch.hpp
    test_basic.cpp
    test_main.cpp
)
target_link_libraries(tests withershins dbghelp)
