# fst benchmark
add_executable(fst
    fst.cpp
)
target_include_directories(fst PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
    fst
    infinity_core
    benchmark_profiler
    c++.a
)
