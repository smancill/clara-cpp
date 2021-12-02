FetchContent_Declare(
  cxxopts
  URL https://github.com/jarro2783/cxxopts/archive/v3.0.0.tar.gz
  URL_HASH SHA256=36f41fa2a46b3c1466613b63f3fa73dc24d912bc90d667147f1e43215a8c6d00
)

FetchContent_GetProperties(cxxopts)

if(NOT cxxopts_POPULATED)
  FetchContent_Populate(cxxopts)

  add_library(cxxopts INTERFACE IMPORTED GLOBAL)
  set_target_properties(cxxopts PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cxxopts_SOURCE_DIR}/include)
endif()
