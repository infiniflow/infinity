
add_library(infinity_client
        infinity_client.cpp
        infinity_client.h
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/InfinityService.cpp
        ${CMAKE_SOURCE_DIR}/src/network/infinity_thrift/infinity_types.cpp
        table_info.h
)

target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(infinity_client PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")

file(GLOB_RECURSE
        client_test_cpp
        CONFIGURE_DEPENDS
        test/*.cpp
)

add_executable(infinity_client_test
        ${client_test_cpp}
)

target_link_libraries(
        infinity_client_test
        infinity_client
        thrift.a
        thriftnb.a
)

target_include_directories(infinity_client_test PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_include_directories(infinity_client_test PUBLIC "${CMAKE_SOURCE_DIR}/client/cpp")
target_link_directories(infinity_client_test PUBLIC "${CMAKE_BINARY_DIR}/lib")

