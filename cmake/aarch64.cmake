# CMake toolchain file for ARM64 Linux (GNU libc)
# Target: aarch64-linux-gnu
find_program(AARCH64_GCC aarch64-linux-gnu-gcc REQUIRED)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a")
set(CMAKE_CXX_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a")
set(CMAKE_ASM_FLAGS_INIT "--target=aarch64-linux-gnu -march=armv8-a")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")

# Use aarch64 cross-compilation libstdc++ modules JSON instead of host's
set(CMAKE_CXX_STDLIB_MODULES_JSON "${CMAKE_CURRENT_LIST_DIR}/aarch64-libstdc++.modules.json")
