cmake_minimum_required(VERSION 4.0.3...4.3.4)

cmake_policy(SET CMP0167 OLD)

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)

if (CMAKE_VERSION GREATER_EQUAL 4.0.3 AND CMAKE_VERSION LESS_EQUAL 4.2.5)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
else (CMAKE_VERSION GREATER 4.2.5 AND CMAKE_VERSION LESS_EQUAL 4.3.4)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
    # we use clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-module-identifier")
endif ()

# Auto-detect and set vcpkg toolchain if not already set
if (NOT CMAKE_TOOLCHAIN_FILE)
    # First try environment variable CMAKE_TOOLCHAIN_FILE (for container builds)
    if (DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_TOOLCHAIN_FILE}"
                CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using vcpkg toolchain from environment: ${CMAKE_TOOLCHAIN_FILE}")
        # Fallback to VCPKG_ROOT for backward compatibility
    elseif (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using vcpkg toolchain from VCPKG_ROOT: ${CMAKE_TOOLCHAIN_FILE}")
    endif ()
endif ()

# Overlay ports for patches not yet upstreamed in vcpkg
set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_SOURCE_DIR}/ports")

project(infinity VERSION 0.7.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_MODULE_STD ON)

if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "This project requires the Ninja generator. Refers to https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#generator-support")
endif ()

# find_program(CCACHE_FOUND ccache)

# if(CCACHE_FOUND)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
#     message("Find ccache")
# else()
#     message("Can not find ccache")
# endif()

message(STATUS "CXX: ${CMAKE_CXX_COMPILER}")
execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
if (CLANG_VERSION_STRING VERSION_GREATER_EQUAL 20)
    # Print CMake version and project name
    message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION} On CLANG-${CLANG_VERSION_STRING}")

    # add_compile_options(-ftime-trace)
    #    add_compile_options(-fmodule-header)
    if (NOT CMAKE_CROSSCOMPILING)
        add_link_options(-fuse-ld=mold)
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-private-field")

    add_link_options(-L/usr/local/lib)
    set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
    set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

else ()

    message(FATAL_ERROR "Please use clang version 20.0 and above, current version: ${CLANG_VERSION_STRING} ${CMAKE_CXX_COMPILER}")

endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm64.*")
    set(ARM64 TRUE)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64.*")
    set(ARM64 TRUE)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    set(X86_64 TRUE)
endif ()

if(ARM64)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSIMDE_ENABLE_NATIVE_ALIASES")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSIMDE_ENABLE_NATIVE_ALIASES")
endif ()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")

find_package(PkgConfig REQUIRED)

find_package(absl CONFIG REQUIRED)
find_package(Arrow CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio)
find_package(CLI11 CONFIG REQUIRED)
#find_package(CURL REQUIRED) # wait for s3
#darts-clone # wait for fix
#eigen3 fuck this package
#find_package(FastPFOR CONFIG REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
#find_package(miniocpp CONFIG REQUIRED) # wait for s3
#nanobind   don't need it
find_package(nlohmann_json CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)
#if (X86_64)
#    find_package(opencc CONFIG REQUIRED) # wait for fix
#endif ()
find_package(OpenSSL REQUIRED)
find_package(pcre2 CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(Parquet CONFIG REQUIRED)
#find_path(PARALLEL_HASHMAP_INCLUDE_DIRS "parallel_hashmap/btree.h")
find_package(re2 CONFIG REQUIRED)
find_package(roaring CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
# find_package(tomlplusplus CONFIG REQUIRED)
#find_package(turbobase64 CONFIG REQUIRED)
find_package(Thrift CONFIG REQUIRED)
#find_package(unofficial-curlpp CONFIG REQUIRED) # for s3
find_package(unofficial-inih CONFIG REQUIRED)
#find_path(VIT_VIT_CTPL_INCLUDE_DIRS "ctpl_stl.h")
find_package(ZLIB REQUIRED)

set(VCPKG_PROJECT_ROOT "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed")
file(GLOB VCPKG_INCLUDE_DIRS
        "${VCPKG_PROJECT_ROOT}/${VCPKG_TARGET_TRIPLET}/include"
)

set(FASTPFOR_STATIC_LIB "${VCPKG_PROJECT_ROOT}/${VCPKG_TARGET_TRIPLET}/lib/libFastPFOR.a")
message(STATUS "Found vcpkg include directories: ${VCPKG_INCLUDE_DIRS}")
include_directories(${VCPKG_INCLUDE_DIRS})

# vcpkg oatpp bug
file(GLOB OATPP_INCLUDE_DIRS
        "${VCPKG_PROJECT_ROOT}/${VCPKG_TARGET_TRIPLET}/include/oatpp-1.3.0/oatpp"
)
message(STATUS "Found oatpp include directories: ${OATPP_INCLUDE_DIRS}")
include_directories(${OATPP_INCLUDE_DIRS})

# vcpkg simde header
find_path(GLOB SIMDE_INCLUDE_DIRS
        "${VCPKG_PROJECT_ROOT}/${VCPKG_TARGET_TRIPLET}/include/simde/arm/neon.h")
include_directories(${SIMDE_INCLUDE_DIRS})

message(STATUS "Using vcpkg toolchain - automatic package discovery enabled")

# Get current system time and print the build time
execute_process(COMMAND "date" +"%Y-%m-%d %H:%M.%S" OUTPUT_VARIABLE CURRENT_SYS_TIME)
string(REGEX REPLACE "\n" "" CURRENT_SYS_TIME ${CURRENT_SYS_TIME})
message(STATUS "Build time = ${CURRENT_SYS_TIME}")

# Get git information. WARNING: For functions which invoke execute_process, the variable populated by execute_process is visiable only inside the function!
find_package(Git)
if (NOT Git_FOUND)
    message(FATAL_ERROR "Git not found.")
endif ()
execute_process(
        COMMAND "${GIT_EXECUTABLE}" symbolic-ref --short HEAD
        OUTPUT_VARIABLE GIT_BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-list --count "${GIT_TAG}..HEAD"
        OUTPUT_VARIABLE COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        OUTPUT_VARIABLE HEAD_COMMIT_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(GIT_COMMIT_ID "${GIT_TAG}-${COMMIT_COUNT}-${HEAD_COMMIT_ID}")
if ("${GIT_BRANCH_NAME}" STREQUAL "")
    message(WARNING "Branch name not found.")
else ()
    message(STATUS "Branch name = ${GIT_BRANCH_NAME}")
endif ()
if ("${HEAD_COMMIT_ID}" STREQUAL "")
    message(WARNING "Commit id not found.")
else ()
    message(STATUS "Commit-id = ${GIT_COMMIT_ID}")
endif ()

# attach additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# You can disable jemalloc by passing the `-DENABLE_JEMALLOC=OFF` option to CMake.
option(ENABLE_JEMALLOC "Enable jemalloc support" ON)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Disable jemalloc in Debug mode")
    set(ENABLE_JEMALLOC OFF)
endif ()

if (ENABLE_JEMALLOC)
    pkg_check_modules(JEMALLOC REQUIRED jemalloc)
    set(JEMALLOC_STATIC_LIB "${VCPKG_PROJECT_ROOT}/${VCPKG_TARGET_TRIPLET}/lib/libjemalloc_pic.a")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_JEMALLOC_PROF")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_JEMALLOC_PROF")
endif ()

if (NOT CMAKE_BUILD_TYPE)
    if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
        set(default_build_type "RelWithDebInfo")
    else ()
        set(default_build_type "Debug")
    endif ()

    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING
            "Default BUILD_TYPE is ${default_build_type}" FORCE)
endif ()

message(STATUS "Build type = ${CMAKE_BUILD_TYPE}")

option(ENABLE_SANITIZER_THREAD "Enable multi-thread sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED "Enable undefined behavior sanitizer" OFF)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DNDEBUG")

elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -DNDEBUG")

elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

    if ("${CODE_COVERAGE}" STREQUAL "ON")
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    else ()
        set(CODE_COVERAGE "OFF")
    endif ()

    message(STATUS "Code Coverage = ${CODE_COVERAGE}")

    if (NOT ENABLE_JEMALLOC)
        if (ENABLE_SANITIZER_THREAD AND ENABLE_SANITIZER_ADDRESS)
            message(FATAL_ERROR "Could not enable THREAD-SANITIZER and ADDRESS-SANITIZER simultaneously.")
        endif ()
        if (ENABLE_SANITIZER_ADDRESS)
            add_compile_options(-fsanitize=address -fsanitize=leak)
            add_link_options(-fsanitize=address -fsanitize=leak)
            message(STATUS "Enable Address Sanitizer in target: infinity")
        endif ()
        if (ENABLE_SANITIZER_THREAD)
            add_compile_options(-fsanitize=thread)
            add_link_options(-fsanitize=thread)
            message(STATUS "Enable Thread Sanitizer in target: infinity")
        endif ()

        if (ENABLE_SANITIZER_UNDEFINED)
            add_compile_options(-fsanitize=undefined)
            add_link_options(-fsanitize=undefined)
            message(STATUS "Enable Undefined Behavior Sanitizer in target: infinity")
        endif ()

        message(STATUS "Enable Sanitizer in target: infinity")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector -fno-var-tracking ")
        add_compile_options(-fsanitize-recover=all)
        add_link_options(-fsanitize-recover=all)

        add_compile_options("-fno-omit-frame-pointer")
        add_link_options("-fno-omit-frame-pointer")
    else ()
        message(STATUS "Disable AddressSanitizer because jemalloc")
    endif ()

    set(CMAKE_DEBUG_POSTFIX "")

else ()
    message(FATAL_ERROR "Only support CMake build type: Debug, RelWithDebInfo, and Release")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-asm-operand-widths -Wno-unused-command-line-argument -Wno-deprecated-declarations -Wextra -Wno-unused-parameter -Wno-unused-private-field -pthread -fcolor-diagnostics")

MESSAGE(STATUS "C++ Compilation flags: " ${CMAKE_CXX_FLAGS})

if (NOT CMAKE_CROSSCOMPILING)
    add_compile_options(-march=native)
endif ()

execute_process(
        COMMAND bash -c "zgrep CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y /proc/config.gz 2>/dev/null; grep CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y /boot/config-$(uname -r) 2>/dev/null"
        OUTPUT_VARIABLE HAVE_EFFICIENT_UNALIGNED_ACCESS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
if ("${HAVE_EFFICIENT_UNALIGNED_ACCESS}" STREQUAL "CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y")
    message(STATUS "Found CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y")
    add_definitions(-DHAVE_EFFICIENT_UNALIGNED_ACCESS)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D INFINITY_DEBUG)
endif ()

if (NOT INDEX_BUILD_METHOD STREQUAL "Variant")
    message(STATUS "Add Environment Variable INDEX_HANDLER")
    ADD_DEFINITIONS(-D INDEX_HANDLER)
else ()
    message(STATUS "NOT USING INDEX_HANDLER")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

# set parameters for unit test coverage
# TODO: issue error "cannot merge previous GCDA file" when turn following switch.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Compile the unit test

# Compile benchmark
add_subdirectory(benchmark)
add_subdirectory(client)

# quit early for scikit-build-core
# does not need CPack settings
# does not need to install the infinity target, config files and resources
if (SKBUILD)
    return()
endif ()

include(Packaging)
