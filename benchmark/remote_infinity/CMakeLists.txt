
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
        iresearch
        lz4.a
        atomic.a
        thrift.a
)