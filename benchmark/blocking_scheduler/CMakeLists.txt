
file(GLOB_RECURSE
        scheduler_benchmark_files
        CONFIGURE_DEPENDS
        scheduler_benchmark.cpp
#        new_scheduler.cpp
#        new_scheduler.h
        task.cpp
        task.h
        pipeline.cpp
        fragment.h
        fragment.cpp
        )

add_executable(blocking_scheduler_benchmark
        ${scheduler_benchmark_files}
        )

target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        blocking_scheduler_benchmark
        benchmark_profiler
        c++.a
)

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
#         target_compile_options(blocking_scheduler_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(blocking_scheduler_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()
