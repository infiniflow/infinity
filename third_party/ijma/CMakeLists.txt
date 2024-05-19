include_directories(./include)
include_directories(./src)
include_directories(./src/libmecab)

find_package(Iconv REQUIRED)

# include config.h
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

add_subdirectory(src)
