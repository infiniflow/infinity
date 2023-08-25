
file(GLOB_RECURSE
        numeric_store_files
        CONFIGURE_DEPENDS
        test_numericstore.cpp
        )

add_executable(numericstore
        ${numeric_store_files}
        )

target_include_directories(numericstore PUBLIC "${CMAKE_SOURCE_DIR}/third_party/croaring/include")
target_include_directories(numericstore PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        numericstore
        infinity_core
        infinity_parser
        roaring
        leveldb
)