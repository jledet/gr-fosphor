INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_FOSPHOR fosphor)

FIND_PATH(
    FOSPHOR_INCLUDE_DIRS
    NAMES fosphor/api.h
    HINTS $ENV{FOSPHOR_DIR}/include
        ${PC_FOSPHOR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FOSPHOR_LIBRARIES
    NAMES gnuradio-fosphor
    HINTS $ENV{FOSPHOR_DIR}/lib
        ${PC_FOSPHOR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/fosphorTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FOSPHOR DEFAULT_MSG FOSPHOR_LIBRARIES FOSPHOR_INCLUDE_DIRS)
MARK_AS_ADVANCED(FOSPHOR_LIBRARIES FOSPHOR_INCLUDE_DIRS)
