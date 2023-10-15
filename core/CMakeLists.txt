
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm.in ${CMAKE_CURRENT_SOURCE_DIR}/bin/compilation_config.cppm)

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
        common_cppm
        CONFIGURE_DEPENDS
        common/*.cppm
)


add_executable(infinity
        ${main_cpp}
        ${infinity_cpp}
        ${network_cpp}
)

target_sources(infinity
        PRIVATE
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
        ${main_cppm}
        ${infinity_cppm}
        ${network_cppm}
        ${common_cppm}
)

target_include_directories(infinity PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(infinity PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")

### unit test

file(GLOB_RECURSE
        ut_main_cpp
        CONFIGURE_DEPENDS
        unit_test/main.cpp
)

file(GLOB_RECURSE
        ut_common_cpp
        CONFIGURE_DEPENDS
        unit_test/common/*.cpp
)

add_executable(unit_test
        ${ut_main_cpp}
        ${ut_common_cpp}
#        ${executor_test_src}
#        ${function_test_src}
#        ${main_test_src}
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
)

#add_executable(unittest
#        ${main_cpp}
#        ${infinity_cpp}
#        ${network_cpp}
#)

target_sources(unit_test
        PRIVATE
        FILE_SET core_cxx_modules TYPE CXX_MODULES FILES
#        ${main_cppm}
#        ${infinity_cppm}
#        ${network_cppm}
        ${common_cppm}
)

target_include_directories(unit_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/ctpl")
target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/third_party/tomlplusplus")

target_include_directories(unit_test PUBLIC "${CMAKE_SOURCE_DIR}/unit_test")

