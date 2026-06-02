# CMake toolchain file for x86-64 Linux (GNU libc)
# Target: x86_64-linux-gnu-v4

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER "clang-20")
set(CMAKE_CXX_COMPILER "clang++-20")

set(CMAKE_C_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64-v4 -fPIC --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld")
set(CMAKE_CXX_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64-v4 -fPIC --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld")
set(CMAKE_ASM_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64-v4 --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld")

# list(APPEND CMAKE_FIND_ROOT_PATH "/usr/x86-64-linux-gnu")
list(APPEND CMAKE_FIND_ROOT_PATH "$ENV{SYS_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXE_LINKER_FLAGS_INIT "--gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld")
