# SPDX-FileCopyrightText: © The Clara Framework Authors
#
# SPDX-License-Identifier: Apache-2.0

# main library
set(CLARA_MSG_FILES
  actor.cpp
  address.cpp
  context.cpp
  connection_driver.cpp
  connection_pool.cpp
  connection_setup.cpp
  proxy.cpp
  registration_driver.cpp
  topic.cpp
  subscription.cpp
  utils.cpp
  zhelper.cpp
)

add_library(clara-msg ${CLARA_MSG_FILES} $<TARGET_OBJECTS:clara-proto-obj>)

target_compile_features(clara-msg PRIVATE cxx_thread_local)
target_link_libraries(clara-msg clara-proto ZeroMQ::libzmq)
target_include_directories(clara-msg PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(clara-msg SYSTEM PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/third_party/zmq>)

if(ENABLE_THREAD_SANITIZER)
  target_compile_options(clara-msg PUBLIC ${THREAD_SANITIZER})
  target_link_libraries(clara-msg ${THREAD_SANITIZER})
endif()

add_library(Clara::msg ALIAS clara-msg)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/clara/msg
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clara
  COMPONENT Clara_Devel
)
install(TARGETS clara-msg EXPORT ClaraTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Clara_Runtime
)

# proto files
add_subdirectory(proto)

# binary files
add_subdirectory(sys)
