project(OpenCC CXX C)

add_definitions(-march=native)

# library target
FILE(GLOB_RECURSE OPENCC_SRC
         "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
         "${CMAKE_CURRENT_SOURCE_DIR}/*.c"         
         )
add_library(opencc STATIC
    ${OPENCC_SRC})
set_target_properties(opencc PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
