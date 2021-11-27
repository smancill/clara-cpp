option(ENABLE_THREAD_SANITIZER "Enable thread-sanitizer support" OFF)

if(ENABLE_THREAD_SANITIZER)
  set(THREAD_SANITIZER -fsanitize=thread -fno-omit-frame-pointer)
endif()

set(XMSG_ZEROMQ_MIN_VERSION 4.0)
set(XMSG_PROTOBUF_MIN_VERSION 2.5)

find_package(ZeroMQ ${XMSG_ZEROMQ_MIN_VERSION} REQUIRED)
find_package(Protobuf ${XMSG_PROTOBUF_MIN_VERSION} REQUIRED)

include(EnsureProtobufTarget)

# main library
set(XMSG_FILES
  xmsg.cpp
  address.cpp
  context.cpp
  connection_driver.cpp
  connection_pool.cpp
  connection_setup.cpp
  proxy.cpp
  registration_driver.cpp
  topic.cpp
  subscription.cpp
  util.cpp
  zhelper.cpp
)

add_library(xmsg ${XMSG_FILES} $<TARGET_OBJECTS:xmsgproto_obj>)

target_link_libraries(xmsg xmsgproto ZeroMQ::libzmq)
target_include_directories(xmsg PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(xmsg SYSTEM PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/third_party/zmq>)
target_compile_options(xmsg PRIVATE -Wno-missing-braces)


if(ENABLE_THREAD_SANITIZER)
  target_compile_options(xmsg PUBLIC ${THREAD_SANITIZER})
  target_link_libraries(xmsg ${THREAD_SANITIZER})
endif()

add_library(xMsg::xmsg ALIAS xmsg)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/xmsg/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xmsg
  COMPONENT xMsg_Devel
)
install(TARGETS xmsg EXPORT xMsgTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT xMsg_Runtime
)

# proto files
add_subdirectory(proto)

# binary files
add_subdirectory(sys)

# package configuration
set(CMAKE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/xmsg")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/xmsg-config.cmake.in
    ${PROJECT_BINARY_DIR}/xmsg-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

install(FILES
    ${PROJECT_SOURCE_DIR}/xmsg-config.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Modules/FindZeroMQ.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Modules/EnsureProtobufTarget.cmake
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT xMsg_Devel)

# export targets
export(EXPORT xMsgTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/xmsg-targets.cmake
  NAMESPACE xMsg::)

install(EXPORT xMsgTargets
  FILE xmsg-targets.cmake
  NAMESPACE xMsg::
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT xMsg_Devel)

# pkg-config
string(CONCAT PKGCONF_REQ_PUB
  "libzmq >= ${XMSG_ZEROMQ_MIN_VERSION}, "
  "protobuf >= ${XMSG_PROTOBUF_MIN_VERSION}")

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/xmsg.pc.in
    ${PROJECT_BINARY_DIR}/xmsg.pc
  @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/xmsg.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT xMsg_Devel)
