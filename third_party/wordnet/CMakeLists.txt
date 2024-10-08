project(WordNet CXX C)

# library target
FILE(GLOB_RECURSE WORDNET_SRC
         "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
         )
add_library(wn STATIC
    ${WORDNET_SRC})
set_target_properties(wn PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
