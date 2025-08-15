add_compile_options(-Wno-everything)

# Build mlas
add_subdirectory(mlas)

#add_library(libhv INTERFACE)
#target_include_directories(libhv INTERFACE libhv/include)

# Build newpfor
add_subdirectory(newpfor)

# Build zsv
add_subdirectory(zsv)

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
#if (ARM64)
add_subdirectory(opencc)
#endif ()

add_subdirectory(minio-cpp)
add_subdirectory(curlpp)

add_subdirectory(curl)

