
file(GLOB_RECURSE
        scheduler_benchmark_files
        CONFIGURE_DEPENDS
        scheduler_benchmark.cpp
        scheduler.cpp
        scheduler.h
        pipeline.cpp
        fragment.h
        )

add_executable(scheduler_benchmark
        ${scheduler_benchmark_files}
        )

target_include_directories(scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")
target_include_directories(scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")

target_link_libraries(
        scheduler_benchmark
        infinity_core
)