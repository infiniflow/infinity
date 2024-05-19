
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

# add_definitions(-march=native)
# add_definitions(-msse4.2 -mfma)
# add_definitions(-mavx2 -mf16c -mpopcnt)

# execute_process(COMMAND grep -q avx2 /proc/cpuinfo  
#                 RESULT_VARIABLE SUPPORT_AVX2  
#                 OUTPUT_QUIET  
#                 ERROR_QUIET)  

# execute_process(COMMAND grep -q avx512 /proc/cpuinfo  
# RESULT_VARIABLE SUPPORT_AVX512  
# OUTPUT_QUIET  
# ERROR_QUIET)

# if (SUPPORT_AVX2 EQUAL 0 OR SUPPORT_AVX512 EQUAL 0)
#         message("Compiled by AVX2 or AVX512")
#         target_compile_options(benchmark_profiler PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(benchmark_profiler PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()