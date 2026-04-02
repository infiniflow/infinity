project(OpenCC CXX C)

if(NOT DEFINED INFINITY_MARCH_FLAG)
    set(INFINITY_MARCH_FLAG "-march=native")
endif()

add_definitions(${INFINITY_MARCH_FLAG})

# library target
FILE(GLOB_RECURSE OPENCC_SRC
         "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
         "${CMAKE_CURRENT_SOURCE_DIR}/*.c"         
         )
add_library(opencc STATIC
    ${OPENCC_SRC})
set_target_properties(opencc PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
