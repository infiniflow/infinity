include_directories(./)

include_directories("${CMAKE_SOURCE_DIR}/src")

add_subdirectory(embedding)
add_subdirectory(blocking_scheduler)
add_subdirectory(polling_scheduler)
add_subdirectory(numericstore)
add_subdirectory(csv)
