include("${CMAKE_SOURCE_DIR}/cmake/External/rapidjson.cmake")

set(CLARA_SRC
  base.cpp
  component.cpp
  composition.cpp
  container.cpp
  container_report.cpp
  data_utils.cpp
  dpe.cpp
  dpe_report.cpp
  engine_data.cpp
  engine_data_type.cpp
  json_report.cpp
  service.cpp
  service_engine.cpp
  service_report.cpp
  utils.cpp
)

add_definitions(-DRAPIDJSON_HAS_STDSTRING)
add_library(clara SHARED ${CLARA_SRC})
add_dependencies(clara miloyip-rapidjson)
target_link_libraries(clara xMsg::xmsg rapidjson ${CMAKE_DL_LIBS})

target_include_directories(clara PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS clara EXPORT ClaraTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/clara
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT Devel
)


add_executable(c_dpe run_dpe.cpp)
target_link_libraries(c_dpe clara)

install(TARGETS c_dpe DESTINATION ${CMAKE_INSTALL_BINDIR})
