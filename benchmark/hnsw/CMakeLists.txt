
file(GLOB_RECURSE
        source_files
        CONFIGURE_DEPENDS
        ./*.cpp
        ./*.h
        )

add_executable(hnsw_benchmark
        ${source_files}
        )

target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")
target_include_directories(hnsw_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/hnswlib")

target_link_libraries(
        hnsw_benchmark
        infinity_core
)