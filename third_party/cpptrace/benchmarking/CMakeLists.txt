include(CTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(
  warning_options
  ${warning_options} $<$<CXX_COMPILER_ID:GNU>:-Wno-infinite-recursion>
)

include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(
  googlebench
  GIT_REPOSITORY "https://github.com/google/benchmark.git"
  GIT_TAG        12235e24652fc7f809373e7c11a5f73c5763fc4c # v1.9.0
)
FetchContent_MakeAvailable(googlebench)

add_executable(benchmark_unwinding unwinding.cpp)
target_compile_features(benchmark_unwinding PRIVATE cxx_std_20)
target_link_libraries(benchmark_unwinding PRIVATE ${target_name} benchmark::benchmark)
