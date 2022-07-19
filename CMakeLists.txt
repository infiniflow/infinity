cmake_minimum_required(VERSION 3.10)
project(infinity)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        src
        third_party/cxxopts/include
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/libhv/include
)

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)