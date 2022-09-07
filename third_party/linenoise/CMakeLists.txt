project(linenoise)

cmake_minimum_required(VERSION 3.0)

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