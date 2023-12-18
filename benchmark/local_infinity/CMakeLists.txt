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
    lz4.a
    atomic.a
)

# ########################################
# knn
# import benchmark
add_executable(knn_import_benchmark
    ./knn/knn_import_benchmark.cpp
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
    lz4.a
    atomic.a
)

# query benchmark
add_executable(knn_query_benchmark
    ./knn/knn_query_benchmark.cpp
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
    lz4.a
    atomic.a
)

# ########################################
# fulltext
# import benchmark
add_executable(fulltext_import_benchmark
    ./fulltext/fulltext_import_benchmark.cpp
)

target_include_directories(fulltext_import_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    fulltext_import_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    iresearch
    lz4.a
    atomic.a
)

# query benchmark
add_executable(fulltext_query_benchmark
    ./fulltext/fulltext_query_benchmark.cpp
)

target_include_directories(fulltext_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    fulltext_query_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    iresearch
    lz4.a
    atomic.a
)

# ########################################
# command query
add_executable(query_benchmark
        ./query/query_benchmark.cpp
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
        lz4.a
        atomic.a
)