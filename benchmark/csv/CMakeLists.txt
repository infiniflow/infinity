
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/csv_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/csv_config.h)

add_executable(csv_benchmark
        csv_benchmark.cpp
)

target_link_libraries(csv_benchmark
        zsv_parser
)

target_include_directories(csv_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")
target_include_directories(csv_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/core")