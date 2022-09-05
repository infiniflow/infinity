include_directories(../third_party/googletest/googletest/include)
include_directories(../third_party/googletest/googlemock/include)
include_directories(./)



file(GLOB_RECURSE
        common_test_src
        CONFIGURE_DEPENDS
        common/utility/*.cpp
        )

add_executable(test_main
        main.cpp
        ${common_test_src}
        )

target_link_libraries(test_main
        gtest
        gtest_main
        gmock
        gmock_main
        infinity_core
        )