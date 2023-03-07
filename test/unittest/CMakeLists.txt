include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)
include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include)
include_directories(BEFORE SYSTEM ./)
include_directories(AFTER SYSTEM /usr/local/include)

file(GLOB_RECURSE
        bin_src
        CONFIGURE_DEPENDS
        bin/*.cpp
        )

file(GLOB_RECURSE
        common_test_src
        CONFIGURE_DEPENDS
        common/*.cpp
        )

file(GLOB_RECURSE
        executor_src
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
        storage_src
        CONFIGURE_DEPENDS
        storage/*.cpp
        )

add_executable(test_main
        main.cpp
        ${bin_src}
        ${common_test_src}
        ${executor_src}
        ${function_test_src}
        ${main_test_src}
        ${planner_test_src}
        ${storage_src}
        )

target_link_libraries(test_main
        gtest
        gtest_main
        gmock
        gmock_main
        infinity_core
        tlsf
        )


add_executable(test_parser
        main.cpp
        ${parser_test_src}
        )

target_link_libraries(test_parser
        gtest
        gtest_main
        gmock
        gmock_main
        infinity_core
        )
