
file(GLOB_RECURSE
        source_files
        CONFIGURE_DEPENDS
        faiss_benchmark.cpp
        helper.cpp
        helper.h
        scheduler.cpp
        scheduler.h
        )

add_executable(faiss_benchmark
        ${source_files}
        )

target_include_directories(faiss_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(faiss_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/hnswlib")
target_include_directories(faiss_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/faiss")
target_include_directories(faiss_benchmark PUBLIC "/usr/include")

target_link_libraries(
        faiss_benchmark
        infinity_core
        faiss
        roaring
        zsv_parser
        benchmark_profiler
)

add_executable(hnsw_benchmark
        hnsw_benchmark.cpp
        helper.cpp
        helper.h
        )

target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/hnswlib")
target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        hnsw_benchmark
        infinity_core
        roaring
        zsv_parser
        benchmark_profiler
)
