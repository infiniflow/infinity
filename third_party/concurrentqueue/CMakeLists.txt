cmake_minimum_required (VERSION 3.2)
project(concurrentqueue
        LANGUAGES CXX)

add_library(concurrentqueue INTERFACE)
target_include_directories(concurrentqueue INTERFACE .)