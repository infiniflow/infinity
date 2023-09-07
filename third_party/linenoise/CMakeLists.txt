project(linenoise)

add_library(
        linenoise
        linenoise.c
)

target_compile_options(
        linenoise
        PRIVATE
        -O3 -Wno-sign-compare
        -fPIC
)