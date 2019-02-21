cmake_minimum_required(VERSION 3.5.1)
project(CLARA LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release MinSizeRel RelWithDebInfo)
endif()

option(CLARA_BUILD_TESTS "Build tests" ON)
option(CLARA_BUILD_DOCS "Build docs" ON)
option(CLARA_BUILD_SHARED_LIBS "Build shared libs" ON)

mark_as_advanced(CLARA_BUILD_SHARED_LIBS)
set(BUILD_SHARED_LIBS ${CLARA_BUILD_SHARED_LIBS})

include(GNUInstallDirs)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE PATH "")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

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


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(xMsg CONFIG REQUIRED)
include(cmake/External/rapidjson.cmake)

string(CONCAT PKGCONF_REQ_PUB "xmsg")


add_subdirectory(src)

if(CLARA_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(CLARA_BUILD_TESTS)
  enable_testing()
  include(cmake/External/GTest.cmake)
  add_subdirectory(test)
endif()


# package configuration
set(CMAKE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/clara")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clara-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/clara-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/clara-config.cmake
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Devel)


# export targets
export(EXPORT ClaraTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/clara-targets.cmake
  NAMESPACE Clara::)

install(EXPORT ClaraTargets
  FILE clara-targets.cmake
  NAMESPACE Clara::
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Devel)


# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clara.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/clara.pc
  @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/clara.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT Devel)


# uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
