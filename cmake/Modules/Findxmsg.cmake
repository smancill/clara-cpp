# - try to find xmsg library
#
# Non-cache variables you might use in your CMakeLists.txt:
#  XMSG_FOUND
#  XMSG_INCLUDE_DIRS
#  XMSG_LIBRARIES

set(XMSG
  "${XMSG}"
  CACHE
  PATH
  "Directory to search")

find_package(Protobuf REQUIRED)

find_library(XMSG_LIBRARY
  NAMES
  xmsg
  HINTS
  "${XMSG}")

find_path(XMSG_INCLUDE_DIR
  NAMES
  xmsg/xmsg.h
  HINTS
  "${_libdir}/.."
  PATHS
  "${XMSG}"
  PATH_SUFFIXES
  "include")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(xmsg
  DEFAULT_MSG
  XMSG_LIBRARY
  XMSG_INCLUDE_DIR)

if(XMSG_FOUND)
  set(XMSG_LIBRARIES
    ${XMSG_LIBRARY}
    ${PROTOBUF_LIBRARIES}
  )
  set(XMSG_INCLUDE_DIRS
    ${XMSG_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
  )
  mark_as_advanced(XMSG)
endif()
