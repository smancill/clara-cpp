include("${CMAKE_SOURCE_DIR}/cmake/External/rapidjson.cmake")

set(CLARA_SRC
  base.cpp
  component.cpp
  composition.cpp
  condition.cpp
  container.cpp
  container_report.cpp
  data_utils.cpp
  dpe.cpp
  dpe_report.cpp
  engine_data.cpp
  engine_data_type.cpp
  instruction.cpp
  json_report.cpp
  service.cpp
  service_engine.cpp
  service_report.cpp
  service_state.cpp
  third_party/dropbox/json11.cpp
  statement.cpp
  utils.cpp
)

set(CLARA_STD_SRC
  stdlib/event_reader_service.cpp
  stdlib/event_writer_service.cpp
  stdlib/json_utils.cpp
)

add_definitions(-DRAPIDJSON_HAS_STDSTRING)
add_library(clara SHARED ${CLARA_SRC} ${CLARA_STD_SRC})
add_dependencies(clara miloyip-rapidjson)
target_link_libraries(clara PUBLIC xMsg::xmsg PRIVATE ${CMAKE_DL_LIBS} rapidjson)

target_include_directories(clara PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/third_party>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS clara EXPORT ClaraTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES ${CMAKE_SOURCE_DIR}/src/third_party/dropbox/json11.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clara/third_party/dropbox
  COMPONENT Devel
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/clara
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT Devel
)


add_executable(c_dpe run_dpe.cpp)
target_link_libraries(c_dpe clara)

install(TARGETS c_dpe DESTINATION ${CMAKE_INSTALL_LIBDIR}/clara)
install(FILES ${CMAKE_SOURCE_DIR}/scripts/unix/c_dpe
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
  DESTINATION ${CMAKE_INSTALL_BINDIR})
