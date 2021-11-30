# SPDX-FileCopyrightText: © The Clara Framework Authors
#
# SPDX-License-Identifier: Apache-2.0

#----------------------------------------------------------------------
# Dependencies
#
add_subdirectory(third_party)

#----------------------------------------------------------------------
# Messaging tests
#
add_subdirectory(msg)
add_subdirectory(msg/perf)

#----------------------------------------------------------------------
# Settings
#
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

#----------------------------------------------------------------------
# Unit tests
#
set(CLARA_UNIT_TESTS
  composition_compiler
  data_utils
  engine_data
  engine_data_type
  utils
)

foreach(name IN LISTS CLARA_UNIT_TESTS)
  add_executable(test_${name} ${name}_test.cpp)
  add_test(NAME test_${name} COMMAND test_${name} CONFIGURATIONS $<CONFIG> "")
  set_tests_properties(test_${name} PROPERTIES LABELS "unit")
  target_include_directories(test_${name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
  target_link_libraries(test_${name} PRIVATE Clara::clara GTest::GMock)
endforeach()

#----------------------------------------------------------------------
# Standalone executable tests
#
add_executable(test_service_loader service_loader_test.cpp)
target_link_libraries(test_service_loader PRIVATE Clara::clara ${CMAKE_DL_LIBS} GTest::GMock)
target_include_directories(test_service_loader PRIVATE "${PROJECT_SOURCE_DIR}/src")
