include_directories(../third_party/googletest/googletest/include)
include_directories(../third_party/googletest/googlemock/include)
include_directories(./)



file(GLOB_RECURSE
        common_test_src
        CONFIGURE_DEPENDS
        common/*.cpp
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
        bin_src
        CONFIGURE_DEPENDS
        bin/*.cpp
        )

file(GLOB_RECURSE
        executor_src
        CONFIGURE_DEPENDS
        executor/*.cpp
        )

file(GLOB_RECURSE
        storage_src
        CONFIGURE_DEPENDS
        storage/*.cpp
        )

add_executable(test_main
        main.cpp
        ${common_test_src}
        ${function_test_src}
        ${main_test_src}
        ${bin_src}
        ${executor_src}
        ${storage_src}
        )

target_link_libraries(test_main
        gtest
        gtest_main
        gmock
        gmock_main
        infinity_core
        sql-parser
        )