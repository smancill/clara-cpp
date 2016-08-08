set(CLARA_SRC
  base.cpp
  component.cpp
  composition.cpp
  container.cpp
  data_utils.cpp
  dpe.cpp
  engine_data.cpp
  engine_data_type.cpp
  service.cpp
  service_engine.cpp
  utils.cpp
)

add_library(clara SHARED ${CLARA_SRC})
target_link_libraries(clara ${XMSG_LIBRARIES} ${CMAKE_DL_LIBS})

target_include_directories(clara PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(clara SYSTEM PUBLIC
  ${XMSG_INCLUDE_DIRS}
)

install(TARGETS clara
  ARCHIVE  DESTINATION lib
  LIBRARY  DESTINATION lib
  RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/clara DESTINATION include)


add_executable(c_dpe run_dpe.cpp)
target_link_libraries(c_dpe clara)

install(TARGETS c_dpe DESTINATION ${CMAKE_INSTALL_BINDIR})
