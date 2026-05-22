project(OpenCC CXX C)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    add_definitions(-march=armv8-a)
else()
    add_definitions(-march=native)
endif()

# library target
FILE(GLOB_RECURSE OPENCC_SRC
         "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
         "${CMAKE_CURRENT_SOURCE_DIR}/*.c"         
         )
add_library(opencc STATIC
    ${OPENCC_SRC})
set_target_properties(opencc PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
