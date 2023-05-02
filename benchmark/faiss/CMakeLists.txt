
file(GLOB_RECURSE
        source_files
        CONFIGURE_DEPENDS
        ./*.cpp
        ./*.h
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