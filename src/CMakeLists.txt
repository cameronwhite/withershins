cmake_minimum_required(VERSION 2.8.9)

set (SOURCE_FILES
    withershins.hpp
    withershins.cpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(SOURCE_FILES ${SOURCE_FILES} withershins_win.cpp)
else()
    set(SOURCE_FILES ${SOURCE_FILES} withershins_unix.cpp)
endif()

add_library(withershins ${SOURCE_FILES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # On Windows, we need to link against dbghelp.
    target_link_libraries(withershins dbghelp)
else()
    target_link_libraries(withershins bfd)
endif()
