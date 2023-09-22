

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

# Build roaring
add_subdirectory(croaring)

# Build mlas
add_subdirectory(mlas)

# Build faiss
add_subdirectory(faiss)

# Build hnsw
add_subdirectory(hnswlib)

# Build concurrentqueue
add_subdirectory(concurrentqueue)

#add_library(libhv INTERFACE)
#target_include_directories(libhv INTERFACE libhv/include)

# Build fastpfor
add_subdirectory(fastpfor)

# Build zsv
add_subdirectory(zsv)
