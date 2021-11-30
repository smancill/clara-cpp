# SPDX-FileCopyrightText: © The Clara Framework Authors
#
# SPDX-License-Identifier: Apache-2.0

#----------------------------------------------------------------------
# Settings
#
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if(ENABLE_THREAD_SANITIZER)
  add_compile_definitions(THREAD_SANITIZER)
endif()

#----------------------------------------------------------------------
# Public interface tests
#
set(CLARA_MSG_PUBLIC_TESTS
  context
  message
  topic
  utils
)

foreach(name IN LISTS CLARA_MSG_PUBLIC_TESTS)
  add_executable(test_msg_${name} ${name}_test.cpp)
  add_test(NAME test_msg_${name} COMMAND test_${name} CONFIGURATIONS $<CONFIG> "")
  set_target_properties(test_msg_${name} PROPERTIES OUTPUT_NAME test_${name})
  set_tests_properties(test_msg_${name} PROPERTIES LABELS "unit;public")
  target_link_libraries(test_msg_${name} PRIVATE clara-msg GTest::GMock)
endforeach()

#----------------------------------------------------------------------
# Internal implementation tests
#
set(CLARA_MSG_INTERNAL_TESTS
  connection_pool
  regdis
  zhelper
)

foreach(name IN LISTS CLARA_MSG_INTERNAL_TESTS)
  add_executable(test_msg_${name} ${name}_test.cpp)
  add_test(NAME test_msg_${name} COMMAND test_${name} CONFIGURATIONS $<CONFIG> "")
  set_target_properties(test_msg_${name} PROPERTIES OUTPUT_NAME test_${name})
  set_tests_properties(test_msg_${name} PROPERTIES LABELS "unit;private")
  target_include_directories(test_msg_${name} PRIVATE "${PROJECT_SOURCE_DIR}/src/msg")
  target_link_libraries(test_msg_${name} PRIVATE clara-msg GTest::GMock)
endforeach()

#----------------------------------------------------------------------
# Slow integration tests
#
set(CLARA_MSG_INTEGRATION_TESTS
  subscription
)

foreach(name IN LISTS CLARA_MSG_INTEGRATION_TESTS)
  add_executable(test_msg_${name} ${name}_test.cpp)
  add_test(NAME test_msg_${name} COMMAND test_${name} CONFIGURATIONS Integration)
  set_target_properties(test_msg_${name} PROPERTIES OUTPUT_NAME test_${name})
  set_tests_properties(test_msg_${name} PROPERTIES LABELS "integration;slow" RUN_SERIAL TRUE)
  target_link_libraries(test_msg_${name} PRIVATE clara-msg GTest::GMock)
endforeach()

set_tests_properties(test_msg_subscription PROPERTIES TIMEOUT 25)

add_custom_target(integration_test
  COMMAND ${CMAKE_CTEST_COMMAND} -C Integration)

#----------------------------------------------------------------------
# Standalone executable tests
#
add_executable(test_registration registrar_test.cpp)
target_link_libraries(test_registration PRIVATE clara-msg)
target_include_directories(test_registration PRIVATE "${PROJECT_SOURCE_DIR}/src/msg")

add_executable(test_addr_resolution addr_resolution_test.cpp)
target_link_libraries(test_addr_resolution PRIVATE clara-msg)
