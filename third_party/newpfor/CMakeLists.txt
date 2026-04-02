project(NewPFOR CXX C)

if(NOT DEFINED INFINITY_MARCH_FLAG)
    set(INFINITY_MARCH_FLAG "-march=native")
endif()

add_definitions(${INFINITY_MARCH_FLAG})

# library target
add_library(newpfor STATIC
    decompress_sse4.cpp
    new_pfordelta_compressor.cpp)
set_target_properties(newpfor PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
