
file(GLOB_RECURSE
        scheduler_benchmark_files
        CONFIGURE_DEPENDS
        scheduler_benchmark.cpp
#        new_scheduler.cpp
#        new_scheduler.h
        task.cpp
        task.h
        pipeline.cpp
        fragment.h
        fragment.cpp
        )

add_executable(blocking_scheduler_benchmark
        ${scheduler_benchmark_files}
        )

target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(blocking_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        blocking_scheduler_benchmark
        benchmark_profiler
        c++.a
)