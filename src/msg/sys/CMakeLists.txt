add_executable(c_proxy proxy.cpp)
target_link_libraries(c_proxy clara-msg)

install(TARGETS c_proxy
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Clara_Runtime
)
