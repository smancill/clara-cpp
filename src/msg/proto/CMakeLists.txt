set(PROTO_FILES
  data.proto
  meta.proto
  registration.proto
)

protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTO_FILES})

add_library(clara-proto-obj OBJECT ${PROTO_SOURCES})
target_compile_options(clara-proto-obj PRIVATE -Wno-extra)
set_target_properties(clara-proto-obj PROPERTIES CXX_CLANG_TIDY "")
if(BUILD_SHARED_LIBS)
  set_target_properties(clara-proto-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(clara-proto-obj SYSTEM PUBLIC
  $<TARGET_PROPERTY:clara-proto,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(clara-proto INTERFACE)
target_link_libraries(clara-proto INTERFACE protobuf::libprotobuf)
target_include_directories(clara-proto SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(FILES ${PROTO_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clara/msg/proto
  COMPONENT Clara_Devel
)
install(TARGETS clara-proto EXPORT ClaraTargets)
