add_executable(hnsw_benchmark2
    hnsw_benchmark2.cpp
    helper.cpp
    helper.h
)
target_include_directories(hnsw_benchmark2 PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
    hnsw_benchmark2
    infinity_core
    sql_parser
    benchmark_profiler
    c++.a
)

add_executable(ann_ivfflat_benchmark
        ann_ivfflat_benchmark.cpp
        helper.cpp
        helper.h
)
target_include_directories(ann_ivfflat_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/mlas")
target_include_directories(ann_ivfflat_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(
        ann_ivfflat_benchmark
        infinity_core
        sql_parser
        benchmark_profiler
        onnxruntime_mlas
        newpfor
        fastpfor
        atomic.a
        lz4.a
        c++.a
)

if(ENABLE_JEMALLOC)
    target_link_libraries(hnsw_benchmark2 jemalloc.a)
    target_link_libraries(ann_ivfflat_benchmark jemalloc.a)
endif()

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
#         target_compile_options(ann_ivfflat_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(ann_ivfflat_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()
