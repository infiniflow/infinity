### Suppressing all warnings from external libs
add_compile_options(-Wno-all)

set(HIGHWAY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/highway)
set(HIGHWAY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/highway PARENT_SCOPE)

set(OPENFST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/openfst)
set(OPENFST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/openfst PARENT_SCOPE)

set(FROZEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/frozen/include)
set(FROZEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/frozen/include PARENT_SCOPE)

set(JIEBA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/cppjieba/include)
set(JIEBA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/cppjieba/include PARENT_SCOPE)
set(JIEBA_DEPS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/cppjieba/deps/limonp/include)
set(JIEBA_DEPS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/cppjieba/deps/limonp/include PARENT_SCOPE)

set(GPHM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/parallel-hashmap)
set(GPHM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/parallel-hashmap PARENT_SCOPE)

################################################################################
### SIMDCOMP library
################################################################################
add_subdirectory(simdcomp)


################################################################################
### Boost::Text library
################################################################################
# Force C++17 for boost::text
set(CXX_STD 17 CACHE STRING "Set to 14, 17, etc., to enable C++14, C++17, etc." FORCE)
add_subdirectory(text)
# force c++17
set_target_properties(text
  PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  )
