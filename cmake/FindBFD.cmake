find_path(BFD_INCLUDE_DIRS bfd.h)
find_library(BFD_LIBRARIES libbfd.a bfd)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG BFD_LIBRARIES BFD_INCLUDE_DIRS)
