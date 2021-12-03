cmake_minimum_required(VERSION 3.16.3)
project(CLARA LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release MinSizeRel RelWithDebInfo)
endif()

option(CLARA_BUILD_TESTS "Build tests" ON)
option(CLARA_BUILD_DOCS "Build docs" ON)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

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

option(ENABLE_THREAD_SANITIZER "Enable thread-sanitizer support" OFF)

if(ENABLE_THREAD_SANITIZER)
  set(THREAD_SANITIZER -fsanitize=thread -fno-omit-frame-pointer)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  file(RELATIVE_PATH relDir
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
  )
  set(CMAKE_INSTALL_RPATH @loader_path @loader_path/${relDir})
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/ThirdParty" CACHE PATH
  "Directory under which to collect all populated content")
include(FetchContent)

if(CLARA_BUILD_TESTS)
  enable_testing()
endif()


set(CLARA_MSG_ZEROMQ_MIN_VERSION 4.0)
set(CLARA_MSG_PROTOBUF_MIN_VERSION 3.6)

find_package(ZeroMQ ${CLARA_MSG_ZEROMQ_MIN_VERSION} REQUIRED)
find_package(Protobuf ${CLARA_MSG_PROTOBUF_MIN_VERSION} REQUIRED)

include(EnsureProtobufTarget)


add_subdirectory(src/third_party)

add_subdirectory(src)

if(CLARA_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(CLARA_BUILD_TESTS)
  add_subdirectory(test)
endif()


# package configuration
set(CMAKE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/clara")

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/clara-config.cmake.in
  ${PROJECT_BINARY_DIR}/clara-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

install(FILES
    ${PROJECT_BINARY_DIR}/clara-config.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Modules/FindZeroMQ.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Modules/EnsureProtobufTarget.cmake
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Clara_Devel)


# export targets
export(EXPORT ClaraTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/clara-targets.cmake
  NAMESPACE Clara::)

install(EXPORT ClaraTargets
  FILE clara-targets.cmake
  NAMESPACE Clara::
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Clara_Devel)


# pkg-config
string(CONCAT PKGCONF_REQ_PUB
  "libzmq >= ${CLARA_MSG_ZEROMQ_MIN_VERSION}, "
  "protobuf >= ${CLARA_MSG_PROTOBUF_MIN_VERSION}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clara.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/clara.pc
  @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/clara.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT Clara_Devel)


# uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
