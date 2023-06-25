
file(GLOB_RECURSE
        bkd_files
        CONFIGURE_DEPENDS
        test_bkd.cpp
        )

add_executable(bkdtree
        ${bkd_files}
        )

target_include_directories(bkdtree PUBLIC "${CMAKE_SOURCE_DIR}/third_party/croaring/include")
target_include_directories(bkdtree PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
        bkdtree
        infinity_core
        infinity_parser
        roaring
)