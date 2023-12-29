cmake_minimum_required(VERSION 3.28)

project(infinity VERSION 0.1.0)

set(CMAKE_GENERATOR "Ninja")

# find_program(CCACHE_FOUND ccache)

# if(CCACHE_FOUND)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
#     message("Find ccache")
# else()
#     message("Can not find ccache")
# endif()

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
if (CLANG_VERSION_STRING VERSION_GREATER 16)
    # Print CMake version and project name
    message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION} On CLANG-${CLANG_VERSION_STRING}")

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-read-modules-implicitly -Wno-unused-parameter -Wno-unused-private-field")

else ()
    message(FATAL_ERROR, "Please use clang version 17.0 and above")
endif ()

# Get current system time and print the build time
execute_process(COMMAND "date" +"%Y-%m-%d %H:%M.%S" OUTPUT_VARIABLE CURRENT_SYS_TIME)
string(REGEX REPLACE "\n" "" CURRENT_SYS_TIME ${CURRENT_SYS_TIME})
message(STATUS "Build time = ${CURRENT_SYS_TIME}")

# Get git information
function(gitInformation)
    find_package(Git)
    if(NOT Git_FOUND)
        message(FATAL_ERROR "Git not found.")
    endif()

    execute_process(
            COMMAND "${GIT_EXECUTABLE}" symbolic-ref --short HEAD
            OUTPUT_VARIABLE BRANCH_NAME
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
            OUTPUT_VARIABLE COMMIT_ID
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if("${BRANCH_NAME}" STREQUAL "")
        message(WARNING "Branch name not found.")
    else()
        message(STATUS "Branch name = ${BRANCH_NAME}")
    endif()

    if("${COMMIT_ID}" STREQUAL "")
        message(WARNING "Commit id not found.")
    else()
        message(STATUS "Commit-id = ${COMMIT_ID}")
    endif()
endfunction()

gitInformation()

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

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_DEBUG_POSTFIX "")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

MESSAGE(STATUS "C++ Compilation flags: " ${CMAKE_CXX_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc -z noexecstack")

#add_definitions(-march=native)
if (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL "18.0")
    add_definitions(-mevex512)
endif ()

ADD_DEFINITIONS(-D INFINITY_DEBUG)

# find_package(Boost REQUIRED)
find_package(Lz4 REQUIRED)

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

# set parameters for unit test coverage
# TODO: issue error "cannot merge previous GCDA file" when turn following switch.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Compile the unit test

# Compile benchmark
add_subdirectory(benchmark)


# CPack settings
set(CPACK_PACKAGE_NAME "infinity")
set(CPACK_PACKAGE_VERSION "0.1.0-dev")
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
