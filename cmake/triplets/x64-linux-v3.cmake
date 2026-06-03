set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Vcpkg only use triplet to specify toolchain file.
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/x86_64-linux-v3.cmake")

set(VCPKG_MAKE_BUILD_TRIPLET "--host=x86_64-linux-gnu")

set(VCPKG_MAKE_OPTIONS "CC='clang-20 --target=x86_64-linux-gnu --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN}'"
    "CXX='clang++-20 --target=x86_64-linux-gnu --sysroot=$ENV{SYS_ROOT} --gcc-toolchain=$ENV{GCC_TOOLCHAIN}'")
