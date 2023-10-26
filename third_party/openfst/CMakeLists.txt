add_library(openfst STATIC
  compat.cc
  encode.cc
  flags.cc
  fst.cc
  fst-types.cc
  mapped-file.cc
  properties.cc
  symbol-table.cc
  symbol-table-ops.cc
  util.cc
  weight.cc
  )
target_include_directories(openfst PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
