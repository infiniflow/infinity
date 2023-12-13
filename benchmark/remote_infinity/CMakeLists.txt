
# query benchmark
add_executable(remote_query_benchmark
        remote_query_benchmark.cpp
        ../../src/network/infinity_thrift/InfinityService.cpp
        ../../src/network/infinity_thrift/infinity_types.cpp
)

target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_link_libraries(
        remote_query_benchmark
        thrift
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        iresearch
        lz4
        atomic
)