cmake_minimum_required (VERSION 2.6)
project(concurrentqueue
        LANGUAGES CXX)

add_library(concurrentqueue INTERFACE)
target_include_directories(concurrentqueue INTERFACE .)