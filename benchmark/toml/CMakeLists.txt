
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/toml_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/toml_config.h)

add_executable(toml_benchmark
        toml_benchmark.cpp
)

target_link_libraries(toml_benchmark
        c++.a
)

target_include_directories(toml_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus/")
target_include_directories(toml_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/src")