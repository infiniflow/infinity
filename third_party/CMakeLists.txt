add_compile_options(-Wno-everything)

################################################################################
### snappy
################################################################################
add_subdirectory(snappy)

################################################################################
### arrow
################################################################################
add_subdirectory(arrow)

# Build doctest
add_subdirectory(googletest)

# Build magic_enum
add_subdirectory(magic_enum)

# Build spdlog
add_subdirectory(spdlog)

# Build mlas
add_subdirectory(mlas)

# Build concurrentqueue
add_subdirectory(concurrentqueue)

#add_library(libhv INTERFACE)
#target_include_directories(libhv INTERFACE libhv/include)

# Build newpfor
add_subdirectory(newpfor)

# Build zsv
add_subdirectory(zsv)

################################################################################
### sse2neon
### need this after highway and before simdcomp
################################################################################

add_library(sse2neon INTERFACE)

if (ARCH_AARCH64)
  set(SSE2NEON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/sse2neon)

  target_include_directories(
    sse2neon
    SYSTEM
    INTERFACE ${SSE2NEON_ROOT}
  )
endif ()

################################################################################
### thrift
################################################################################
add_subdirectory(thrift)

################################################################################
### thrift
################################################################################
add_subdirectory(oatpp)

################################################################################
### fastpfor
################################################################################
add_subdirectory(fastpfor)

################################################################################
### ijma
################################################################################
add_subdirectory(ijma)

################################################################################
### opencc
################################################################################
add_subdirectory(opencc)

add_subdirectory(minio-cpp)
add_subdirectory(curlpp)
add_subdirectory(pugixml)

add_library(inih
        inih/cpp/INIReader.cpp
        inih/ini.c
)

add_subdirectory(curl)