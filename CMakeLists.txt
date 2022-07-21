cmake_minimum_required(VERSION 3.10)
project(infinity)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        src
        third_party/cxxopts/include
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/libhv/include
        /usr/local/include
)

#set(DEFAULT_LIB_DIRS $ENV{HOME}/local /opt/local /usr/local /usr)
#find_package(Boost REQUIRED)

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)