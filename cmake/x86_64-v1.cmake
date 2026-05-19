# CMake toolchain file for x86-64 Linux (GNU libc)
# Target: x86_64-linux-gnu-v1
find_program(x86_64_GCC x86_64-linux-gnu-gcc REQUIRED)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64")
set(CMAKE_CXX_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64")
set(CMAKE_ASM_FLAGS_INIT "--target=x86_64-linux-gnu -march=x86-64")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")
