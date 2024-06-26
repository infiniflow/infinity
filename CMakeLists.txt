cmake_minimum_required(VERSION 3.28.1)

project(infinity VERSION 0.2.0)

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "This project requires the Ninja generator. Refers to https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#generator-support")
endif()

# find_program(CCACHE_FOUND ccache)

# if(CCACHE_FOUND)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
#     message("Find ccache")
# else()
#     message("Can not find ccache")
# endif()

set(CMAKE_CXX_STANDARD 20)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
if (CLANG_VERSION_STRING VERSION_GREATER 16)
    # Print CMake version and project name
    message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION} On CLANG-${CLANG_VERSION_STRING}")

else ()

    message(FATAL_ERROR, "Please use clang version 17.0 and above, current version: ", ${CLANG_VERSION_STRING})

endif ()

# Get current system time and print the build time
execute_process(COMMAND "date" +"%Y-%m-%d %H:%M.%S" OUTPUT_VARIABLE CURRENT_SYS_TIME)
string(REGEX REPLACE "\n" "" CURRENT_SYS_TIME ${CURRENT_SYS_TIME})
message(STATUS "Build time = ${CURRENT_SYS_TIME}")

# Get git information. WARNING: For functions which invoke execute_process, the variable populated by execute_process is visiable only inside the function!
find_package(Git)
if(NOT Git_FOUND)
    message(FATAL_ERROR "Git not found.")
endif()
execute_process(
        COMMAND "${GIT_EXECUTABLE}" symbolic-ref --short HEAD
        OUTPUT_VARIABLE GIT_BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        OUTPUT_VARIABLE GIT_COMMIT_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE)
if("${GIT_BRANCH_NAME}" STREQUAL "")
    message(WARNING "Branch name not found.")
else()
    message(STATUS "Branch name = ${GIT_BRANCH_NAME}")
endif()
if("${GIT_COMMIT_ID}" STREQUAL "")
    message(WARNING "Commit id not found.")
else()
    message(STATUS "Commit-id = ${GIT_COMMIT_ID}")
endif()

# attach additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(TEST_DATA_PATH ${CMAKE_SOURCE_DIR}/test/data)
set(CSV_DATA_PATH ${CMAKE_SOURCE_DIR}/third_party/zsv/data)
set(TMP_DATA_PATH ${CMAKE_SOURCE_DIR}/tmp)

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

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

    set(CMAKE_CXX_FLAGS "-Ofast -DNDEBUG")
    set(CMAKE_C_FLAGS  "-Ofast -DNDEBUG")

elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")

    set(CMAKE_CXX_FLAGS "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS  "-O2 -g -DNDEBUG")

elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

    set(CMAKE_CXX_FLAGS "-O0 -g")
    set(CMAKE_C_FLAGS "-O0 -g")

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-stack-protector -fno-var-tracking ")
    add_compile_options(-fsanitize=address -fsanitize-recover=all -fsanitize=leak)
    add_link_options(-fsanitize=address -fsanitize-recover=all -fsanitize=leak)

    add_compile_options("-fno-omit-frame-pointer")
    add_link_options("-fno-omit-frame-pointer")

#    add_compile_options("-fsanitize=undefined")
#    add_link_options("-fsanitize=undefined")

#    add_compile_options("-fsanitize=thread")
#    add_link_options("-fsanitize=thread")

    set(CMAKE_DEBUG_POSTFIX "")

else ()
    message(FATAL_ERROR "Only support CMake build type: Debug, RelWithDebInfo, and Release")
endif ()

if (CLANG_VERSION_STRING VERSION_EQUAL 17)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-asm-operand-widths -Wno-unused-command-line-argument -Wno-deprecated-declarations -Wno-read-modules-implicitly -Wextra -Wno-unused-parameter -Wno-unused-private-field -pthread -fcolor-diagnostics")
else ()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-asm-operand-widths -Wno-unused-command-line-argument -Wno-deprecated-declarations -Wextra -Wno-unused-parameter -Wno-unused-private-field -pthread -fcolor-diagnostics")
endif ()

MESSAGE(STATUS "C++ Compilation flags: " ${CMAKE_CXX_FLAGS})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc -z noexecstack")

#add_definitions(-march=native)
add_definitions(-DSIMDE_ENABLE_NATIVE_ALIASES)
if (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL "18.0")
    add_definitions(-mevex512)
endif ()

execute_process(
        COMMAND bash -c "zgrep CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y /proc/config.gz 2>/dev/null; grep CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y /boot/config-$(uname -r) 2>/dev/null"
        OUTPUT_VARIABLE HAVE_EFFICIENT_UNALIGNED_ACCESS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
if("${HAVE_EFFICIENT_UNALIGNED_ACCESS}" STREQUAL "CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y")
    message(STATUS "Found CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y")
    add_definitions(-DHAVE_EFFICIENT_UNALIGNED_ACCESS)
endif()

ADD_DEFINITIONS(-D INFINITY_DEBUG)

# find_package(Boost REQUIRED)
find_package(Lz4 REQUIRED)

# You can disable jemalloc by passing the `-DENABLE_JEMALLOC=OFF` option to CMake.
option(ENABLE_JEMALLOC "Enable jemalloc support" ON)
if(ENABLE_JEMALLOC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    find_package(jemalloc REQUIRED)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DENABLE_JEMALLOC_PROF")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DENABLE_JEMALLOC_PROF")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(Python 3.8
        REQUIRED COMPONENTS Interpreter Development.Module
        OPTIONAL_COMPONENTS Development.SABIModule)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/nanobind)

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

# set parameters for unit test coverage
# TODO: issue error "cannot merge previous GCDA file" when turn following switch.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Compile the unit test

# Compile benchmark
add_subdirectory(benchmark)
add_subdirectory(client)

# CPack settings
set(CPACK_PACKAGE_NAME "infinity")
set(CPACK_PACKAGE_VERSION "0.2.0")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Zhichang Yu <yuzhichang@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The AI-native database built for LLM applications, offering incredibly fast vector and full-text search.")
set(CPACK_PACKAGE_VENDOR "infiniflow")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

# https://cmake.org/cmake/help/latest/command/install.html
# Install target
# WANNING: If an absolute path is given, cpack will install the specific files on the host system (requires root permission) and then included in the package.
# If an relative path (interpreted relative to the value of the CMAKE_INSTALL_PREFIX variable) is given, cpack include specific files in the package without actually installing them on the host system.
# CMAKE_INSTALL_PREFIX defaults to "/usr/local"
set(CMAKE_INSTALL_PREFIX /usr)
install(TARGETS infinity DESTINATION bin)
install(FILES conf/infinity.service DESTINATION lib/systemd/system)
install(FILES conf/infinity_conf.toml DESTINATION etc)
install(DIRECTORY resource/jieba DESTINATION ../var/infinity/resource)
install(DIRECTORY resource/mecab DESTINATION ../var/infinity/resource)

# https://cmake.org/cmake/help/latest/cpack_gen/rpm.html
# Specify the post-install script for RPM
# CPackRPM needs the absolute path of the file as CPack does not know that script is relative to source tree.
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/conf/postinst")

# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
# Add custom script to the control.tar.gz. Typical usage is for conffiles, postinst, postrm, prerm.
# Note: DEB requires the file name be one of postinst, postrm, prerm and the "+x" permission, while rpm doesn't require that.
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/conf/postinst")

# Specify the package generators
set(CPACK_GENERATOR "RPM;DEB;TGZ")

# Enable CPack debug output
set(CPACK_PACKAGE_DEBUG True)

# https://cmake.org/cmake/help/latest/variable/CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION.html
set(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION "ON")
include(CPack)
