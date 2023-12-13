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

# ########################################
# knn
# import benchmark
add_executable(knn_import_benchmark
    ./knn/import_phase.cpp
)

target_include_directories(knn_import_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    knn_import_benchmark
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
add_executable(knn_query_benchmark
    ./knn/query_phase.cpp
)

target_include_directories(knn_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    knn_query_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    iresearch
    lz4
    atomic
)
