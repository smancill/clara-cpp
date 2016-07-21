include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_engine_data_type engine_data_type_test.cpp)
add_test(test_engine_data_type test_engine_data_type)
target_link_libraries(test_engine_data_type clara gmock)

add_executable(test_engine_data engine_data_test.cpp)
add_test(test_engine_data test_engine_data)
target_link_libraries(test_engine_data clara gmock)

add_executable(test_data_utils data_utils_test.cpp)
add_test(test_data_utils test_data_utils)
target_link_libraries(test_data_utils clara gmock)
