cmake_minimum_required(VERSION 3.8)

project(module_load_test VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/cpptrace)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(mydll SHARED mydll.cpp)
target_include_directories(mydll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main PUBLIC cpptrace::cpptrace)
