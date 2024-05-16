
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/csv_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/csv_config.h)

add_executable(csv_benchmark
        csv_benchmark.cpp
)

target_link_libraries(csv_benchmark
        zsv_parser
        c++.a
)

if(ENABLE_JEMALLOC)
    target_link_libraries(csv_benchmark jemalloc.a)
endif()

target_include_directories(csv_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")
target_include_directories(csv_benchmark
        PUBLIC "${CMAKE_SOURCE_DIR}/core")

# add_definitions(-march=native)
# add_definitions(-msse4.2 -mfma)
# add_definitions(-mavx2 -mf16c -mpopcnt)

# execute_process(COMMAND grep -q avx2 /proc/cpuinfo  
#                 RESULT_VARIABLE SUPPORT_AVX2  
#                 OUTPUT_QUIET  
#                 ERROR_QUIET)  

# execute_process(COMMAND grep -q avx512 /proc/cpuinfo  
# RESULT_VARIABLE SUPPORT_AVX512  
# OUTPUT_QUIET  
# ERROR_QUIET)

# if (SUPPORT_AVX2 EQUAL 0 OR SUPPORT_AVX512 EQUAL 0)
#         message("Compiled by AVX2 or AVX512")
#         target_compile_options(csv_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(csv_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()