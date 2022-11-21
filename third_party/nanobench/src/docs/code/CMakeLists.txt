cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

project(
    CMakeNanobenchExample
    VERSION 1.0
    LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.1.0
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(nanobench)

add_executable(MyExample my_example.cpp)
target_link_libraries(MyExample PRIVATE nanobench)
