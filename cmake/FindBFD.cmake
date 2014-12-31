if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_package(Intl REQUIRED)
endif()
find_package(Iberty REQUIRED)
find_package(ZLIB REQUIRED)

find_path(BFD_INCLUDE_DIR bfd.h)
find_library(BFD_LIBRARY libbfd.a bfd)

set(BFD_INCLUDE_DIRS ${BFD_INCLUDE_DIR} ${IBERTY_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${INTL_INCLUDE_DIRS})
set(BFD_LIBRARIES ${BFD_LIBRARY} ${IBERTY_LIBRARIES} ${ZLIB_LIBRARIES} ${INTL_LIBRARIES} ${CMAKE_DL_LIBS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG BFD_LIBRARY BFD_INCLUDE_DIR)
