

add_executable(add_benchmark
        add.cpp
        )

target_include_directories(add_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nanobench/src/include")