FILE(GLOB_RECURSE jma_SRC "*.cpp")

# include modules
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckIncludeFiles)

# check libmecab configurations
INCLUDE(libmecab/ConfigureChecks.cmake)

# preprocessor definitions for libiconv
add_definitions(-DLIBDIR="/usr/local/lib")

add_library(jma STATIC ${jma_SRC})
target_link_libraries(jma
    z
)
