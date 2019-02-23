FetchContent_Declare(
  xmsg
  GIT_REPOSITORY https://github.com/JeffersonLab/xmsg-cpp.git
  GIT_TAG origin/master
)

FetchContent_GetProperties(xmsg)

if(NOT xmsg_POPULATED)
  FetchContent_Populate(xmsg)

  set(CMAKE_CXX_CLANG_TIDY "")

  add_subdirectory(${xmsg_SOURCE_DIR} ${xmsg_BINARY_DIR})
endif()
