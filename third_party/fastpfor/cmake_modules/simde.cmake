include(FetchContent)
FetchContent_Declare(
        simde
        GIT_REPOSITORY https://github.com/simd-everywhere/simde.git
        GIT_TAG master
)
FetchContent_MakeAvailable(simde)

add_library(simde INTERFACE IMPORTED GLOBAL)
target_include_directories(simde INTERFACE "${simde_SOURCE_DIR}")

# Enables native aliases. Not ideal but makes it easier to convert old code.
target_compile_definitions(simde INTERFACE SIMDE_ENABLE_NATIVE_ALIASES)