set( headers
    withershins.hpp
)

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( platform_srcs withershins_win.cpp )
else()
    set( platform_srcs withershins_unix.cpp )
endif()

set( srcs
    withershins.cpp
    ${platform_srcs}
)

add_library( withershins ${srcs} ${headers} )

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    target_link_libraries( withershins dbghelp )
else ()
    include_directories( ${BFD_INCLUDE_DIRS} )
    target_link_libraries( withershins ${BFD_LIBRARIES} )
endif ()

install( TARGETS withershins DESTINATION lib )
install( FILES ${headers} DESTINATION include )
