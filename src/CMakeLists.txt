configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm)

### Infinity

### WARNING: DONT MOVE FOLLOWING COMMAND TO OTHER PLACE.

# add_definitions(-march=native)
# add_definitions(-msse4.2 -mfma)
# add_definitions(-mavx2 -mf16c -mpopcnt)


if (APPLE)
    if (X86_64)
        execute_process(COMMAND sh -c "sysctl -a machdep.cpu.features | grep FMA"
                RESULT_VARIABLE SUPPORT_FMA
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND sh -c "sysctl -a machdep.cpu.features | grep SSE4.2"
                RESULT_VARIABLE SUPPORT_SSE42
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND sh -c "sysctl -a machdep.cpu.features | grep AVX2"
                RESULT_VARIABLE SUPPORT_AVX2
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND sh -c "sysctl -a machdep.cpu.features | grep AVX512"
                RESULT_VARIABLE SUPPORT_AVX512
                OUTPUT_QUIET
                ERROR_QUIET)
    endif ()
else ()
    #Linux
    if (X86_64)
        execute_process(COMMAND grep -q fma /proc/cpuinfo
                RESULT_VARIABLE SUPPORT_FMA
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND grep -q sse4_2 /proc/cpuinfo
                RESULT_VARIABLE SUPPORT_SSE42
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND grep -q avx2 /proc/cpuinfo
                RESULT_VARIABLE SUPPORT_AVX2
                OUTPUT_QUIET
                ERROR_QUIET)

        execute_process(COMMAND grep -q avx512 /proc/cpuinfo
                RESULT_VARIABLE SUPPORT_AVX512
                OUTPUT_QUIET
                ERROR_QUIET)
    endif ()
endif ()


file(GLOB_RECURSE
        main_cpp
        CONFIGURE_DEPENDS
        bin/infinity_main.cpp
)

file(GLOB_RECURSE
        main_cppm
        CONFIGURE_DEPENDS
        bin/*.cppm
)

file(GLOB_RECURSE
        infinity_cpp
        CONFIGURE_DEPENDS
        main/*.cpp
)
list(FILTER infinity_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        infinity_impl_cpp
        CONFIGURE_DEPENDS
        main/*_impl.cpp
)

file(GLOB_RECURSE
        infinity_cppm
        CONFIGURE_DEPENDS
        main/*.cppm
)

file(GLOB_RECURSE
        network_cpp
        CONFIGURE_DEPENDS
        network/*.cpp
)
list(FILTER network_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        network_impl_cpp
        CONFIGURE_DEPENDS
        network/*_impl.cpp
)

file(GLOB_RECURSE
        network_cppm
        CONFIGURE_DEPENDS
        network/*.cppm
)

file(GLOB_RECURSE
        function_cpp
        CONFIGURE_DEPENDS
        function/*.cpp
)
list(FILTER function_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        function_impl_cpp
        CONFIGURE_DEPENDS
        function/*_impl.cpp
)

file(GLOB_RECURSE
        function_cppm
        CONFIGURE_DEPENDS
        function/*.cppm
)

file(GLOB_RECURSE
        expression_cpp
        CONFIGURE_DEPENDS
        expression/*.cpp
)
list(FILTER expression_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        expression_impl_cpp
        CONFIGURE_DEPENDS
        expression/*_impl.cpp
)

file(GLOB_RECURSE
        expression_cppm
        CONFIGURE_DEPENDS
        expression/*.cppm
)

file(GLOB_RECURSE
        executor_cpp
        CONFIGURE_DEPENDS
        executor/*.cpp
)
list(FILTER executor_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        executor_impl_cpp
        CONFIGURE_DEPENDS
        executor/*_impl.cpp
)

file(GLOB_RECURSE
        executor_cppm
        CONFIGURE_DEPENDS
        executor/*.cppm
)

file(GLOB_RECURSE
        metrics_cpp
        CONFIGURE_DEPENDS
        metrics/*.cpp
)
list(FILTER metrics_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        metrics_impl_cpp
        CONFIGURE_DEPENDS
        metrics/*_impl.cpp
)

file(GLOB_RECURSE
        metrics_cppm
        CONFIGURE_DEPENDS
        metrics/*.cppm
)

file(GLOB_RECURSE
        planner_cpp
        CONFIGURE_DEPENDS
        planner/*.cpp
)
list(FILTER planner_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        planner_impl_cpp
        CONFIGURE_DEPENDS
        planner/*_impl.cpp
)

file(GLOB_RECURSE
        planner_cppm
        CONFIGURE_DEPENDS
        planner/*.cppm
)

file(GLOB_RECURSE
        scheduler_cpp
        CONFIGURE_DEPENDS
        scheduler/*.cpp
)
list(FILTER scheduler_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        scheduler_impl_cpp
        CONFIGURE_DEPENDS
        scheduler/*_impl.cpp
)

file(GLOB_RECURSE
        scheduler_cppm
        CONFIGURE_DEPENDS
        scheduler/*.cppm
)

file(GLOB_RECURSE
        storage_cpp
        CONFIGURE_DEPENDS
        storage/*.cpp
)
list(FILTER storage_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        storage_impl_cpp
        CONFIGURE_DEPENDS
        storage/*_impl.cpp
)

file(GLOB_RECURSE
        storage_cppm
        CONFIGURE_DEPENDS
        storage/*.cppm
)

file(GLOB_RECURSE
        common_cpp
        CONFIGURE_DEPENDS
        common/*.cpp
)
list(FILTER common_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        common_impl_cpp
        CONFIGURE_DEPENDS
        common/*_impl.cpp
)

file(GLOB_RECURSE
        common_cppm
        CONFIGURE_DEPENDS
        common/*.cppm
)

file(GLOB_RECURSE
        parser_cppm
        CONFIGURE_DEPENDS
        parser/*.cppm
)

file(GLOB_RECURSE
        third_party_cpp
        CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/third_party/simdjson/*.cpp
)

file(GLOB_RECURSE
        admin_cpp
        CONFIGURE_DEPENDS
        admin/*.cpp
)
list(FILTER admin_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        admin_impl_cpp
        CONFIGURE_DEPENDS
        admin/*_impl.cpp
)

file(GLOB_RECURSE
        admin_cppm
        CONFIGURE_DEPENDS
        admin/*.cppm
)

add_library(infinity_core
        ${infinity_cpp}
        ${planner_cpp}
        ${scheduler_cpp}
        ${storage_cpp}
        ${function_cpp}
        ${expression_cpp}
        ${executor_cpp}
        ${metrics_cpp}
        ${common_cpp}
        ${admin_cpp}
        ${network_cpp}
        ${third_party_cpp}
)

target_sources(infinity_core
        PUBLIC
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
        ${main_cppm}
        ${infinity_cppm}
        ${common_cppm}
        ${planner_cppm}
        ${scheduler_cppm}
        ${storage_cppm}
        ${parser_cppm}
        ${function_cppm}
        ${expression_cppm}
        ${executor_cppm}
        ${metrics_cppm}
        ${admin_cppm}
        ${network_cppm}

        ${infinity_impl_cpp}
        ${planner_impl_cpp}
        ${scheduler_impl_cpp}
        ${storage_impl_cpp}
        ${function_impl_cpp}
        ${expression_impl_cpp}
        ${executor_impl_cpp}
        ${metrics_impl_cpp}
        ${common_impl_cpp}
        ${admin_impl_cpp}
        ${network_impl_cpp}
)

add_dependencies(infinity_core thrift thriftnb
        parquet_static
        snappy re2 pcre2-8-static rocksdb)
target_include_directories(infinity_core PUBLIC ${Python3_INCLUDE_DIRS})
target_include_directories(infinity_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinity_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/parser")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/rocksdb/include/")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cli11")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/croaring")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/newpfor")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/fastpfor/headers")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cppjieba/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cppjieba/deps/limonp/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ijma/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/parallel-hashmap")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/pgm/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/mlas")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/xor_singleheader/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/base64/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/oatpp/src")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/eigen-3.4.0")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/opencc")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/arrow/src")
target_include_directories(infinity_core PUBLIC "${CMAKE_BINARY_DIR}/third_party/snappy/")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(infinity_core PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/minio-cpp/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/curlpp/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/curl/include")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/darts/")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/re2")
target_include_directories(infinity_core PUBLIC "${CMAKE_SOURCE_DIR}/third_party/simdjson")
target_include_directories(infinity_core PUBLIC "${CMAKE_BINARY_DIR}/third_party/pcre2")


if (X86_64)
    if (NOT SUPPORT_FMA EQUAL 0)
        message(FATAL_ERROR "This project requires the processor support fused multiply-add (FMA) instructions.")
    endif ()

    if (NOT SUPPORT_SSE42 EQUAL 0)
        message(FATAL_ERROR "This project requires the processor support sse4_2 instructions.")
    endif ()

    if (SUPPORT_AVX2 EQUAL 0 OR SUPPORT_AVX512 EQUAL 0)
        message("Compiled by AVX2 or AVX512")
        add_definitions(-march=native)
        target_compile_options(infinity_core PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
    else ()
        message("Compiled by SSE")
        add_definitions(-msse4.2 -mfma)
        target_compile_options(infinity_core PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
    endif ()
endif ()

add_executable(infinity
        ${main_cpp}
)

#target_sources(infinity
#        PUBLIC
#        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
#        ${network_cppm}
#)

### Parser (built after infinity_core so it can access modules)
add_subdirectory(parser)

add_dependencies(infinity oatpp miniocpp pugixml-static curlpp_static inih libcurl_static)

target_link_libraries(infinity
        infinity_core
        sql_parser
        onnxruntime_mlas
        zsv_parser
        newpfor
        fastpfor
        jma
        opencc
        dl
        thrift.a
        thriftnb.a
        lz4.a
        event.a
        oatpp.a
        parquet.a
        arrow.a
        snappy.a
        ${JEMALLOC_STATIC_LIB}
        miniocpp.a
        pugixml-static
        curlpp_static
        inih.a
        libcurl_static
        ssl.a
        crypto.a
        re2.a
        pcre2-8-static
        rocksdb.a
        ${STDCXX15EXP_STATIC}
)
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/lib")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/oatpp/src/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/arrow/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/snappy/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/minio-cpp/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/pugixml/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/curlpp/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/curl/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/re2/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/pcre2/")
target_link_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/rocksdb/")
target_link_directories(infinity PUBLIC "/usr/local/openssl30/lib64")

target_include_directories(infinity PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(infinity PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")

# if (SKBUILD)
#    add_library(embedded_infinity_ext MODULE
#            embedded_infinity_ext.cpp
#            embedded_infinity/wrap_infinity.cpp
#    )
#
#    target_sources(embedded_infinity_ext
#            PRIVATE
#            FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
#            embedded_infinity/wrap_infinity.cppm
#    )

#    nanobind_compile_options(embedded_infinity_ext)
#    nanobind_link_options(embedded_infinity_ext)
#    set_target_properties(embedded_infinity_ext PROPERTIES LINKER_LANGUAGE CXX)
#    set(libname "nanobind-static-abi3")
#    nanobind_build_library(${libname})
#    nanobind_extension_abi3(embedded_infinity_ext)
#    add_dependencies(embedded_infinity_ext oatpp miniocpp pugixml-static curlpp_static inih libcurl_static re2 pcre2-8-static)

#    target_link_libraries(embedded_infinity_ext PRIVATE
#            ${libname}
#            infinity_core
#            sql_parser
#            onnxruntime_mlas
#            zsv_parser
#            newpfor
#            fastpfor
#            jma
#            opencc
#            dl
#            lz4.a
#            event.a
#            oatpp.a
#            parquet.a
#            arrow.a
#            thrift.a
#            thriftnb.a
#            snappy.a
#            ${JEMALLOC_STATIC_LIB}
#            miniocpp.a
#            pugixml-static
#            curlpp_static
#            inih.a
#            libcurl_static
#            ssl.a
#            crypto.a
#            re2.a
#            pcre2-8-static
#            rocksdb.a
#            ${STDCXX15EXP_STATIC}
#    )

# WARN: python modules shall not link to static libstdc++!!!
#    target_link_options(embedded_infinity_ext PRIVATE -static-libgcc)
#    set_target_properties(embedded_infinity_ext PROPERTIES CXX_VISIBILITY_PRESET hidden)
#
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/lib")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/oatpp/src/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/arrow/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/snappy/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/minio-cpp/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/pugixml/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/curlpp/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/curl/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/re2/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/pcre2/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/")
#    target_link_directories(embedded_infinity_ext PUBLIC "${CMAKE_BINARY_DIR}/third_party/rocksdb/")
#    target_link_directories(embedded_infinity_ext PUBLIC "/usr/local/openssl30/lib64")
#    nanobind_disable_stack_protector(embedded_infinity_ext)
#    nanobind_opt_size(embedded_infinity_ext)
#    nanobind_strip(embedded_infinity_ext)
#
#    install(TARGETS embedded_infinity_ext LIBRARY DESTINATION infinity_embedded)
# endif ()

### unit test

file(GLOB_RECURSE
        ut_test_main_cpp
        CONFIGURE_DEPENDS
        unit_test/main.cpp
)

file(GLOB_RECURSE
        ut_common_cpp1
        CONFIGURE_DEPENDS
        unit_test/common/*_ut.cpp
)
file(GLOB_RECURSE
        ut_common_cpp2
        CONFIGURE_DEPENDS
        unit_test/common/*_impl.cpp
)
file(GLOB_RECURSE
        ut_common_cpp
        CONFIGURE_DEPENDS
        unit_test/common/*.cpp
)
list(FILTER ut_common_cpp EXCLUDE REGEX .*_ut.cpp)
list(FILTER ut_common_cpp EXCLUDE REGEX .*_impl.cpp)

file(GLOB_RECURSE
        ut_parser_cpp1
        CONFIGURE_DEPENDS
        unit_test/parser/*_ut.cpp
)
file(GLOB_RECURSE
        ut_parser_cpp
        CONFIGURE_DEPENDS
        unit_test/parser/*.cpp
)
list(FILTER ut_parser_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_storage_cpp1
        CONFIGURE_DEPENDS
        unit_test/storage/*_ut.cpp
)
file(GLOB_RECURSE
        ut_storage_cpp
        CONFIGURE_DEPENDS
        unit_test/storage/*.cpp
)
list(FILTER ut_storage_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_executor_cpp1
        CONFIGURE_DEPENDS
        unit_test/executor/*_ut.cpp
)
file(GLOB_RECURSE
        ut_executor_cpp
        CONFIGURE_DEPENDS
        unit_test/executor/*.cpp
)
list(FILTER ut_executor_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_test_helper_cpp
        CONFIGURE_DEPENDS
        unit_test/test_helper/*.cpp
)

file(GLOB_RECURSE
        ut_main_cpp1
        CONFIGURE_DEPENDS
        unit_test/main/*_ut.cpp
)
file(GLOB_RECURSE
        ut_main_cpp
        CONFIGURE_DEPENDS
        unit_test/main/*.cpp
)
list(FILTER ut_main_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_planner_cpp1
        CONFIGURE_DEPENDS
        unit_test/planner/*_ut.cpp
)
file(GLOB_RECURSE
        ut_planner_cpp
        CONFIGURE_DEPENDS
        unit_test/planner/*.cpp
)
list(FILTER ut_planner_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_function_cpp1
        CONFIGURE_DEPENDS
        unit_test/function/*_ut.cpp
)
file(GLOB_RECURSE
        ut_function_cpp
        CONFIGURE_DEPENDS
        unit_test/function/*.cpp
)
list(FILTER ut_function_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_parallel_cpp1
        CONFIGURE_DEPENDS
        unit_test/parallel/*_ut.cpp
)
file(GLOB_RECURSE
        ut_parallel_cpp
        CONFIGURE_DEPENDS
        unit_test/parallel/*.cpp
)
list(FILTER ut_parallel_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_thirdparty_cpp1
        CONFIGURE_DEPENDS
        unit_test/third_party/*_ut.cpp
)
file(GLOB_RECURSE
        ut_thirdparty_cpp
        CONFIGURE_DEPENDS
        unit_test/third_party/*.cpp
)
list(FILTER ut_thirdparty_cpp EXCLUDE REGEX .*_ut.cpp)

file(GLOB_RECURSE
        ut_dependent_cppm
        CONFIGURE_DEPENDS
        unit_test/base_test.cppm
        unit_test/test_helper/sql_runner.cppm
        unit_test/*.cppm
)

add_executable(unit_test
        ${ut_test_main_cpp}
        ${ut_common_cpp}
        ${ut_storage_cpp}
        ${ut_main_cpp}
        ${ut_parser_cpp}
        ${ut_function_cpp}
        ${ut_thirdparty_cpp}
        ${ut_executor_cpp}
        ${ut_test_helper_cpp}
        ${ut_planner_cpp}
        ${ut_function_cpp}
        ${ut_parallel_cpp}
)

set_target_properties(unit_test PROPERTIES OUTPUT_NAME test_main)
add_dependencies(unit_test oatpp miniocpp pugixml-static curlpp_static inih libcurl_static re2 pcre2-8-static rocksdb infinity_core)

target_link_libraries(unit_test
        infinity_core
        gtest
        gtest_main
        sql_parser
        onnxruntime_mlas
        zsv_parser
        newpfor
        fastpfor
        jma
        opencc
        dl
        lz4.a
        parquet.a
        arrow.a
        snappy.a
        oatpp.a
        ${JEMALLOC_STATIC_LIB}
        thrift.a
        thriftnb.a
        event.a
        miniocpp.a
        re2.a
        pcre2-8-static
        pugixml-static
        curlpp_static
        inih.a
        libcurl_static
        ssl.a
        crypto.a
        rocksdb.a
        ${STDCXX15EXP_STATIC}
)

target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/lib")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/arrow/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/snappy/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/oatpp/src/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/minio-cpp/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/pugixml/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/curlpp/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/curl/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/re2/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/pcre2/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/rocksdb/")
target_link_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/")
target_link_directories(unit_test PUBLIC "/usr/local/openssl30/lib64")

target_sources(unit_test
        PRIVATE
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
        ${ut_dependent_cppm}

        ${ut_common_cpp1}
        ${ut_common_cpp2}
        ${ut_parser_cpp1}
        ${ut_storage_cpp1}
        ${ut_executor_cpp1}
        ${ut_planner_cpp1}
        ${ut_function_cpp1}
        ${ut_parralel_cpp1}
        ${ut_thirdparty_cpp1}
)

target_include_directories(unit_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/rocksdb/include/")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/unit_test")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/pgm/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/opencc")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/curlpp/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/curl/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/darts")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/re2")
target_include_directories(unit_test PUBLIC "${CMAKE_BINARY_DIR}/third_party/pcre2")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/simdjson")

# target_compile_options(unit_test PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mavx2 -mfma -mf16c -mpopcnt>)
if (X86_64)
    if (SUPPORT_AVX2 EQUAL 0 OR SUPPORT_AVX512 EQUAL 0)
        message("Compiled by AVX2 or AVX512")
        add_definitions(-mavx2 -mfma -mf16c -mpopcnt)
        target_compile_options(unit_test PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mavx2 -mfma -mf16c -mpopcnt>)
    else ()
        message("Compiled by SSE")
        add_definitions(-msse4.2 -mfma)
        target_compile_options(unit_test PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-msse4.2 -mfma>)
    endif ()
else ()
    add_definitions(-march=native)
endif ()

