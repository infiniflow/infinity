project(NewPFOR CXX C)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    add_definitions(-march=armv8-a)
endif()
# For x86_64, rely on the parent project's march flags instead of -march=native

# library target
add_library(newpfor STATIC
    decompress_sse4.cpp
    new_pfordelta_compressor.cpp)
set_target_properties(newpfor PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
