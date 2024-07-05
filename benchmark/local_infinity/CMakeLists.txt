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
    newpfor
    fastpfor
    lz4.a
    atomic.a
    c++.a
    c++abi.a
    jma
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
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
    newpfor
    fastpfor
    lz4.a
    atomic.a
    c++.a
    c++abi.a
    jma
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
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
    newpfor
    fastpfor
    lz4.a
    atomic.a
    c++.a
    c++abi.a
    jma
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
)

# ########################################
# fulltext
# import benchmark
add_executable(fulltext_benchmark
    ./fulltext/fulltext_benchmark.cpp
)

target_include_directories(fulltext_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    fulltext_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    newpfor
    fastpfor
    lz4.a
    atomic.a
    c++.a
    c++abi.a
    jma
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
)

# ########################################
add_executable(sparse_benchmark
    ./sparse/sparse_benchmark.cpp
)

target_include_directories(sparse_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    sparse_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    newpfor
    fastpfor
    lz4.a
    atomic.a
    jma
    c++.a
    c++abi.a
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
)

add_executable(bmp_benchmark
    ./sparse/bmp_benchmark.cpp
)

target_include_directories(bmp_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    bmp_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    newpfor
    fastpfor
    lz4.a
    atomic.a
    jma
    c++.a
    c++abi.a
    opencc
)

add_executable(hnsw_benchmark
    ./knn/hnsw_benchmark.cpp
)

target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    hnsw_benchmark
    infinity_core
    benchmark_profiler
    sql_parser
    onnxruntime_mlas
    zsv_parser
    newpfor
    fastpfor
    lz4.a
    atomic.a
    jma
    c++.a
    c++abi.a
    opencc
    Arrow::arrow_static
    Parquet::parquet_static
)

if(ENABLE_JEMALLOC)
    target_link_libraries(infinity_benchmark jemalloc.a)
    target_link_libraries(knn_import_benchmark jemalloc.a)
    target_link_libraries(knn_query_benchmark jemalloc.a)
    target_link_libraries(fulltext_benchmark jemalloc.a)
    target_link_libraries(sparse_benchmark jemalloc.a)
    target_link_libraries(bmp_benchmark jemalloc.a)
    target_link_libraries(hnsw_benchmark jemalloc.a)
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
#         target_compile_options(infinity_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
#         target_compile_options(knn_import_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
#         target_compile_options(knn_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native)
# else()
#         message("Compiled by SSE")
#         target_compile_options(infinity_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
#         target_compile_options(knn_import_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
#         target_compile_options(knn_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()
