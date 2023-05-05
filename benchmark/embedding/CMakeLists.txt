
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

target_include_directories(faiss_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")
target_include_directories(faiss_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/faiss")

target_link_libraries(
        faiss_benchmark
        faiss
        infinity_core
)

add_executable(hnsw_benchmark
        hnsw_benchmark.cpp
        helper.cpp
        helper.h
        )

target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")
target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/hnswlib")
target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        hnsw_benchmark
        infinity_core
)