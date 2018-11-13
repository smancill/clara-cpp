cmake_minimum_required(VERSION 3.5.1)
project(CLARA C CXX)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to RelWithDebInfo")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

if(NOT CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "bin" CACHE PATH "user executables (bin)")
endif()

if(NOT CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "object code libraries (lib)")
endif()

if(NOT CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "C header files (include)")
endif()

if(NOT CMAKE_INSTALL_CONFIGDIR)
  set(CMAKE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/clara"
    CACHE PATH "CMake project files (${CMAKE_INSTALL_LIBDIR}/cmake/clara")
endif()

mark_as_advanced(CMAKE_INSTALL_BINDIR)
mark_as_advanced(CMAKE_INSTALL_LIBDIR)
mark_as_advanced(CMAKE_INSTALL_INCLUDEDIR)
mark_as_advanced(CMAKE_INSTALL_CONFIGDIR)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(GCC_MIN_VERSION 4.9.2)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_MIN_VERSION)
    message(FATAL_ERROR "Expected GCC >= ${GCC_MIN_VERSION} but found ${CMAKE_CXX_COMPILER_VERSION}")
  endif()
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  endif()
endif()

find_package(xMsg CONFIG REQUIRED)

option(CLARA_BUILD_TESTS "Build tests" ON)


add_subdirectory("src")
add_subdirectory("doc")

if (CLARA_BUILD_TESTS)
  enable_testing()
  include("cmake/External/GMock.cmake")
  add_subdirectory("test")
endif (CLARA_BUILD_TESTS)


# export targets
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/clara-config.cmake.in
  ${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/clara-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

install(EXPORT ClaraTargets
  FILE clara-targets.cmake
  NAMESPACE Clara::
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR})

install(FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/clara-config.cmake
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR} COMPONENT Devel)


# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
