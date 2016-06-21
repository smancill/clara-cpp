set(CLARA_SRC
  engine_data.cpp
  engine_data_type.cpp
)

add_library(clara SHARED ${CLARA_SRC})
target_link_libraries(clara ${XMSG_LIBRARIES} ${CMAKE_DL_LIBS})

target_include_directories(clara PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${XMSG_INCLUDE_DIRS}
)

# add_executable(clara-cpp-dpe runDPE.cpp)
# target_link_libraries(clara-cpp-dpe clara)

# install(FILES ${CLARA_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# install(TARGETS clara DESTINATION ${CMAKE_INSTALL_LIBDIR})
# install(TARGETS clara-cpp-dpe DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS clara
  ARCHIVE  DESTINATION lib
  LIBRARY  DESTINATION lib
  RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/clara DESTINATION include)
