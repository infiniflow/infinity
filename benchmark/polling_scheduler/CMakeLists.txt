
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
        poller_queue.h
        )

add_executable(polling_scheduler_benchmark
        ${scheduler_benchmark_files}
        )

target_include_directories(polling_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")
target_include_directories(polling_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(polling_scheduler_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        polling_scheduler_benchmark
        infinity_core
)