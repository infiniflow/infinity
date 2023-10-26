cmake_minimum_required(VERSION 3.28)

project(infinity VERSION 0.1.0)

find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
    message("Find ccache")
else()
    message("Can not find ccache")
endif()

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 23)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
if(CLANG_VERSION_STRING VERSION_GREATER 16)
    # Print CMake version and project name
    message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION} On CLANG-${CLANG_VERSION_STRING}")
else ()
    message(FATAL_ERROR, "Please use clang version 17.0 and above")
endif ()

# Get current system time and print the build time
execute_process(COMMAND "date" +"%Y-%m-%d %H:%M.%S" OUTPUT_VARIABLE CURRENT_SYS_TIME)
string(REGEX REPLACE "\n" "" CURRENT_SYS_TIME ${CURRENT_SYS_TIME})
message(STATUS "Build time = ${CURRENT_SYS_TIME}")

# Get git branch name
execute_process(COMMAND "git" symbolic-ref --short HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME)
string(REGEX REPLACE "\n" "" GIT_BRANCH_NAME ${GIT_BRANCH_NAME})
message(STATUS "Branch name = ${GIT_BRANCH_NAME}")

execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD OUTPUT_VARIABLE GIT_COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Commit-id = ${GIT_COMMIT_ID}")

set(TEST_DATA_PATH ${CMAKE_SOURCE_DIR}/test/data)
set(CSV_DATA_PATH ${CMAKE_SOURCE_DIR}/third_party/zsv/data)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#    set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations")
    set(CMAKE_CXX_FLAGS "-Wno-read-modules-implicitly")
#    set(CMAKE_CXX_FLAGS_DEBUG "-Wno-unused-variable -Wno-unused-result -Wno-read-modules-implicitly")
# set (CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations -ftime-trace" )
else()
    message(FATAL_ERROR "Please, use Clang. Current compiler is ${CMAKE_CXX_COMPILER_ID}!")
endif()

if(NOT CMAKE_BUILD_TYPE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
        set(default_build_type "RelWithDebInfo")
    else()
        set(default_build_type "Debug")
    endif()

    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING
        "Default BUILD_TYPE is ${default_build_type}" FORCE)
endif()

add_definitions(-march=native)
if (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL "18.0")
    add_definitions(-mevex512)
endif ()

ADD_DEFINITIONS(-D INFINITY_DEBUG)

# set(DEFAULT_LIB_DIRS $ENV{HOME}/local /opt/local /usr/local /usr)
# find_package(Boost REQUIRED)
add_subdirectory(core)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

# set parameters for unit test coverage
# TODO: issue error "cannot merge previous GCDA file" when turn following switch.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Compile the unit test

# Compile benchmark
add_subdirectory(benchmark)
