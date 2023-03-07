

# Build cxxopts
add_subdirectory(cxxopts)

# Build doctest
add_subdirectory(googletest)

# Build linenoise
add_subdirectory(linenoise)

# Build magic_enum
add_subdirectory(magic_enum)

# Build spdlog
add_subdirectory(spdlog)

set(TLSF_DIR tlsf)
add_library(
        tlsf
        ${TLSF_DIR}/tlsf.cpp
)


#add_library(libhv INTERFACE)
#target_include_directories(libhv INTERFACE libhv/include)