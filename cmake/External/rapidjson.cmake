INCLUDE(ExternalProject)
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

ExternalProject_Add(
  miloyip-rapidjson
  URL https://github.com/miloyip/rapidjson/archive/v1.1.0.tar.gz
  URL_HASH SHA1=a3e0d043ad3c2d7638ffefa3beb30a77c71c869f
  TIMEOUT 120
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE OFF
  LOG_BUILD OFF
)

ExternalProject_Get_Property(miloyip-rapidjson SOURCE_DIR)

# avoid error with set_target_properties
file(MAKE_DIRECTORY "${SOURCE_DIR}/include")

add_library(rapidjson INTERFACE IMPORTED)
set_target_properties(rapidjson PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/include"
)
