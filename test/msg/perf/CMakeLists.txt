add_executable(remote_thr remote_thr.cpp)
target_link_libraries(remote_thr clara-msg)

add_executable(local_thr local_thr.cpp)
target_link_libraries(local_thr clara-msg)
