
file(GLOB_RECURSE
        profiler_files
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

MESSAGE("${profiler_files}")

add_library(benchmark_profiler
        ${profiler_files}
)

target_include_directories(benchmark_profiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(benchmark_profiler PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")