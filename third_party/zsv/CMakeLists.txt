project(zsv_parser)

file(GLOB_RECURSE
    zsv_src
    CONFIGURE_DEPENDS
    src/zsv.c
    src/zsv_internal.c)

# Use C instead of CXX. zsv_parser cannot compile by CXX.
set_source_files_properties(${zsv_src} PROPERTIES LANGUAGE C)

add_library(zsv_parser STATIC
    ${zsv_src})

target_include_directories(zsv_parser PUBLIC "./include")

target_compile_options(
    zsv_parser
    PRIVATE
    -O3 -Wno-sign-compare
    -fPIC
)