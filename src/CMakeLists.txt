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

set(CLARA_STD_SRC
  stdlib/event_reader_service.cpp
  stdlib/event_writer_service.cpp
  stdlib/json_utils.cpp
        stdlib/streaming_service.cpp user/TriDASService.cpp user/TriDASService.hpp)

add_library(clara ${CLARA_SRC} ${CLARA_STD_SRC} $<TARGET_OBJECTS:json11>)
target_link_libraries(clara
  PRIVATE rapidjson concurrentqueue thread_pool ${CMAKE_DL_LIBS}
  PUBLIC xMsg::xmsg
)

get_target_property(JSON11_INCLUDEDIR json11 INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(clara PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(clara SYSTEM PUBLIC
  $<BUILD_INTERFACE:${JSON11_INCLUDEDIR}>
)

add_library(Clara::clara ALIAS clara)

install(TARGETS clara EXPORT ClaraTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Clara_Runtime
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/clara
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT Clara_Devel
)


add_executable(c_dpe run_dpe.cpp)
target_link_libraries(c_dpe optionparser clara)

install(TARGETS c_dpe
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/clara
  COMPONENT Clara_Runtime
)
install(FILES ${PROJECT_SOURCE_DIR}/scripts/unix/c_dpe
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Clara_Runtime
)
