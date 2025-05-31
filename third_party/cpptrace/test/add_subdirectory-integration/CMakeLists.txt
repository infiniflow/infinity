cmake_minimum_required(VERSION 3.8)

project(demo_project VERSION 0.0.1 LANGUAGES CXX)

add_executable(main main.cpp)

add_subdirectory(cpptrace)
target_link_libraries(main cpptrace::cpptrace)
target_compile_features(main PRIVATE cxx_std_11)

if(WIN32)
  add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:cpptrace::cpptrace>
    $<TARGET_FILE_DIR:main>
  )
endif()
