cmake_minimum_required(VERSION 2.8.9)

add_executable(tests
    catch.hpp
    test_basic.cpp
    test_main.cpp
)
target_link_libraries(tests withershins)

# Ensure that inlining is prevented and debug symbols are generated for these
# tests, so that we get consistent results regardless of how CMAKE_BUILD_TYPE
# is set.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties(tests PROPERTIES COMPILE_FLAGS "/Od /Zi")
    set_target_properties(tests PROPERTIES LINK_FLAGS "/DEBUG")
endif()
