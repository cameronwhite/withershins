find_path(IBERTY_INCLUDE_DIRS libiberty.h
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES libiberty
)
find_library(IBERTY_LIBRARIES iberty)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iberty DEFAULT_MSG IBERTY_LIBRARIES IBERTY_INCLUDE_DIRS)

