

file(GLOB_RECURSE
        client_cpp
        CONFIGURE_DEPENDS
        *.cpp
)

file(GLOB_RECURSE
        client_header
        CONFIGURE_DEPENDS
        *.h
)

add_library(infinity_client
        ${client_cpp}
        ${client_header}
)

target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/src/network/infinity_thrift")
target_include_directories(infinity_client PUBLIC "${CMAKE_SOURCE_DIR}/third_party/thrift/lib/cpp/src")
target_include_directories(infinity_client PUBLIC "${CMAKE_BINARY_DIR}/third_party/thrift/")


