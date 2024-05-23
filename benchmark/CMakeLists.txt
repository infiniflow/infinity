include_directories(./)

include_directories("${CMAKE_SOURCE_DIR}/benchmark/common")
include_directories("${CMAKE_SOURCE_DIR}/src")

add_definitions(-march=native)

add_subdirectory(common)
add_subdirectory(local_infinity)
add_subdirectory(remote_infinity)