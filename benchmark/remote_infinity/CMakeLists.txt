
# query benchmark
add_executable(remote_query_benchmark
        remote_query_benchmark.cpp
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/InfinityService.cpp
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/infinity_types.cpp
)

target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/lib")

target_link_libraries(
        remote_query_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        newpfor
        fastpfor
        lz4.a
        atomic.a
        thrift.a
        c++.a
        c++abi.a
        jma
        opencc
        Arrow::arrow_static
        Parquet::parquet_static
)

if(ENABLE_JEMALLOC)
    target_link_libraries(remote_query_benchmark jemalloc.a)
endif()

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
#         target_compile_options(remote_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(remote_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()
