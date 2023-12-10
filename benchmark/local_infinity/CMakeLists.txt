# infinity benchmark
add_executable(infinity_benchmark
        infinity_benchmark.cpp
)

target_include_directories(infinity_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        infinity_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        iresearch
        lz4
        atomic
)

# import benchmark
add_executable(import_benchmark
        import_benchmark.cpp
)

target_include_directories(import_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        import_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        iresearch
        lz4
        atomic
)

# query benchmark
add_executable(query_benchmark
        query_benchmark.cpp
)

target_include_directories(query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        query_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        iresearch
        lz4
        atomic
)