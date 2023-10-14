include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)
include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include)
include_directories(BEFORE SYSTEM ./)
include_directories(AFTER SYSTEM /usr/local/include)

file(GLOB_RECURSE
        common_test_src
        CONFIGURE_DEPENDS
        common/*.cpp
)

file(GLOB_RECURSE
        executor_test_src
        CONFIGURE_DEPENDS
        executor/*.cpp
)

file(GLOB_RECURSE
        function_test_src
        CONFIGURE_DEPENDS
        function/*.cpp
)

file(GLOB_RECURSE
        main_test_src
        CONFIGURE_DEPENDS
        main/*.cpp
)

file(GLOB_RECURSE
        parser_test_src
        CONFIGURE_DEPENDS
        parser/*.cpp
)

file(GLOB_RECURSE
        planner_test_src
        CONFIGURE_DEPENDS
        planner/*.cpp
)

file(GLOB_RECURSE
        storage_test_src
        CONFIGURE_DEPENDS
        storage/*.cpp
)

file(GLOB_RECURSE
        test_helper_src
        CONFIGURE_DEPENDS
        test_helper/*.cpp
)

file(GLOB_RECURSE
        third_party_src
        CONFIGURE_DEPENDS
        third_party/*.cpp
)

add_executable(test_main
        main.cpp
        ${common_test_src}
        ${executor_test_src}
        ${function_test_src}
        ${main_test_src}
        ${planner_test_src}
        ${storage_test_src}
        ${parser_test_src}
        ${test_helper_src}
        ${third_party_src}
)

target_precompile_headers(test_main REUSE_FROM infinity_parser)

target_link_libraries(test_main
        gtest
        gtest_main
        gmock
        gmock_main
        infinity_core
        tlsf
        zsv_parser
        roaring
        faiss
        gfortran
        fastpfor
        newpfor
)

target_include_directories(test_main PUBLIC "${CMAKE_SOURCE_DIR}/third_party/simdjson")
target_include_directories(test_main PUBLIC "${CMAKE_SOURCE_DIR}/third_party/nlohmann")
