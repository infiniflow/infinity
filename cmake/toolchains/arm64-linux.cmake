# CMake toolchain file for ARM64 Linux (GNU libc)
# Target: aarch64-linux-gnu
#find_program(AARCH64_GCC aarch64-linux-gnu-gcc REQUIRED)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER "clang-20")
set(CMAKE_CXX_COMPILER "clang++-20")

set(CMAKE_C_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a -fPIC --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN}")
set(CMAKE_CXX_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a -fPIC --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN}")
set(CMAKE_ASM_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN}")

# list(APPEND CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu")
list(APPEND CMAKE_FIND_ROOT_PATH "$ENV{SYS_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXE_LINKER_FLAGS_INIT "--gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld-20")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--gcc-toolchain=$ENV{GCC_TOOLCHAIN} -fuse-ld=lld-20")

# CMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY prevents check_library_exists
# from actually linking, so it falsely detects libraries like libsocket.
# Pre-set these to avoid false positives:
set(HAVE_LIBSOCKET CACHE INTERNAL "0")

