add_executable(infinity_benchmark
        infinity_benchmark.cpp
)
target_include_directories(infinity_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        infinity_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        faiss
        onnxruntime_mlas
        zsv_parser
)