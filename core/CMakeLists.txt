
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm)

add_subdirectory(parser)

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

file(GLOB_RECURSE
        executor_cppm
        CONFIGURE_DEPENDS
        executor/*.cppm
)

file(GLOB_RECURSE
        planner_cpp
        CONFIGURE_DEPENDS
        planner/*.cpp
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


add_executable(infinity
        ${main_cpp}
        ${infinity_cpp}
        ${network_cpp}
        ${planner_cpp}
        ${scheduler_cpp}
        ${storage_cpp}
        ${function_cpp}
        ${expression_cpp}
        ${executor_cpp}
        ${common_cpp}
)

target_sources(infinity
        PRIVATE
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
        ${main_cppm}
        ${infinity_cppm}
        ${network_cppm}
        ${common_cppm}
        ${planner_cppm}
        ${scheduler_cppm}
        ${storage_cppm}
        ${parser_cppm}
        ${function_cppm}
        ${expression_cppm}
        ${executor_cppm}
)

target_link_libraries(infinity
        sql_parser
        faiss
        onnxruntime_mlas
)

target_include_directories(infinity PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/faiss")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")

target_compile_options(infinity PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mavx2 -mfma -mf16c -mpopcnt>)

### unit test

file(GLOB_RECURSE
        ut_test_main_cpp
        CONFIGURE_DEPENDS
        unit_test/main.cpp
)

file(GLOB_RECURSE
        ut_common_cpp
        CONFIGURE_DEPENDS
        unit_test/common/*.cpp
)

file(GLOB_RECURSE
        ut_parser_cpp
        CONFIGURE_DEPENDS
        unit_test/parser/*.cpp
)

file(GLOB_RECURSE
        ut_storage_cpp
        CONFIGURE_DEPENDS
        unit_test/storage/*.cpp
)

file(GLOB_RECURSE
        ut_executor_cpp
        CONFIGURE_DEPENDS
        unit_test/executor/*.cpp
)

file(GLOB_RECURSE
        ut_test_helper_cpp
        CONFIGURE_DEPENDS
        unit_test/test_helper/*.cpp
)

file(GLOB_RECURSE
        ut_main_cpp
        CONFIGURE_DEPENDS
        unit_test/main/*.cpp
)

file(GLOB_RECURSE
        ut_thirdparty_cpp
        CONFIGURE_DEPENDS
        unit_test/third_party/*.cpp
)

file(GLOB_RECURSE
        sql_runner_cppm
        CONFIGURE_DEPENDS
        unit_test/test_helper/sql_runner.cppm
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
#        ${executor_test_src}
#        ${function_test_src}
        ${infinity_cpp}
        ${network_cpp}
        ${planner_cpp}
        ${scheduler_cpp}
        ${storage_cpp}
        ${expression_cpp}
        ${function_cpp}
        ${common_cpp}
        ${executor_cpp}
#        ${planner_test_src}
#        ${storage_test_src}
#        ${parser_test_src}
#        ${test_helper_src}
#        ${third_party_src}
)

set_target_properties(unit_test PROPERTIES OUTPUT_NAME test_main)

target_link_libraries(unit_test
        gtest
        gtest_main
        gmock
        gmock_main
        sql_parser
        faiss
        onnxruntime_mlas
)

#add_executable(unittest
#        ${main_cpp}
#        ${infinity_cpp}
#        ${network_cpp}
#)

target_sources(unit_test
        PRIVATE
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
        ${main_cppm}
        ${infinity_cppm}
        ${network_cppm}
        ${common_cppm}
        ${planner_cppm}
        ${scheduler_cppm}
        ${storage_cppm}
        ${parser_cppm}
        ${function_cppm}
        ${expression_cppm}
        ${executor_cppm}
        ${sql_runner_cppm}
)

target_include_directories(unit_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/magic_enum/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/unit_test")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/concurrentqueue")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/faiss")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/zsv/include")

target_compile_options(unit_test PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mavx2 -mfma -mf16c -mpopcnt>)
