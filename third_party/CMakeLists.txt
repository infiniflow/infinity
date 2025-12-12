add_compile_options(-Wno-everything)

#add_library(libhv INTERFACE)
#target_include_directories(libhv INTERFACE libhv/include)

# Build newpfor
add_subdirectory(newpfor)

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

