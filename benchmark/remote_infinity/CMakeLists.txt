
# query benchmark
add_executable(remote_query_benchmark
        remote_query_benchmark.cpp
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/InfinityService.cpp
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/infinity_types.cpp
)

target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(remote_query_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/lib")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/third_party/minio-cpp/")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/third_party/curlpp/")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/third_party/curl/")
target_link_directories(remote_query_benchmark PUBLIC "${CMAKE_BINARY_DIR}/third_party/")

target_link_libraries(
        remote_query_benchmark
        infinity_core
        benchmark_profiler
        sql_parser
        onnxruntime_mlas
        zsv_parser
        newpfor
        fastpfor
        jma
        opencc
        dl
        thrift::thrift
        # thriftnb::thriftnb
        ${STDCXX15EXP_STATIC}
        Parquet::parquet_static
        Arrow::arrow_static
        ${JEMALLOC_STATIC_LIB}
        miniocpp.a
        re2::re2
        absl::any absl::log absl::base absl::bits
        PCRE2::8BIT
        pugixml::static
        curlpp_static
        unofficial::inih::libinih
        unofficial::inih::inireader
        libcurl_static
        OpenSSL::SSL
        OpenSSL::Crypto
        RocksDB::rocksdb
)

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
#         target_compile_options(remote_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
# else()
#         message("Compiled by SSE")
#         target_compile_options(remote_query_benchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
# endif()
