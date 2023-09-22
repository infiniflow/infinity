cmake_minimum_required(VERSION 3.10)

project(infinity VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Print CMake version and project name
message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION}")

# Get current system time and print the build time
execute_process(COMMAND "date" +"%Y-%m-%d %H:%M.%S" OUTPUT_VARIABLE CURRENT_SYS_TIME)
string(REGEX REPLACE "\n" "" CURRENT_SYS_TIME ${CURRENT_SYS_TIME})
message(STATUS "Build time = ${CURRENT_SYS_TIME}")

# Get git branch name
execute_process(COMMAND "git" rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME)
string(REGEX REPLACE "\n" "" GIT_BRANCH_NAME ${GIT_BRANCH_NAME})
message(STATUS "Branch name = ${GIT_BRANCH_NAME}")

execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD OUTPUT_VARIABLE GIT_COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Commit-id = ${GIT_COMMIT_ID}")

set(TEST_DATA_PATH ${CMAKE_SOURCE_DIR}/test/data)
set(CSV_DATA_PATH ${CMAKE_SOURCE_DIR}/third_party/zsv/data)



# SET CLANG PATH AND TOOLS PATH
set(TOOLS_PATH "${CMAKE_SOURCE_DIR}/tools")

# add search path for clang, if it is not found in the default path
set(CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@18/bin" "/opt/homebrew/opt/llvm@18/bin")

# Make Sure you have clang-format and clang-tidy installed
find_program(CLANG_FORMAT_BIN clang-format
  HINTS ${CLANG_SEARCH_PATH}
  DOC "Path to clang-format executable")

if (NOT CLANG_FORMAT_BIN)
  message(FATAL_ERROR "clang-format not found.")
endif()

find_program(CLANG_TIDY_BIN clang-tidy
  HINTS ${CLANG_SEARCH_PATH}
  DOC "Path to clang-tidy executable")

if (NOT CLANG_TIDY_BIN)
  message(FATAL_ERROR "clang-tidy not found.")
endif()

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  MESSAGE("${CMAKE_CXX_COMPILER_ID}")
  MESSAGE("${CMAKE_CXX_COMPILER}")
  MESSAGE("CLANG")
  set (CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations" )
#  set (CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations -ftime-trace" )
else ()
  message("GCC")
endif ()

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

ADD_DEFINITIONS(-D INFINITY_DEBUG)

#set(DEFAULT_LIB_DIRS $ENV{HOME}/local /opt/local /usr/local /usr)
#find_package(Boost REQUIRED)

add_subdirectory(src)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

# set parameters for unit test coverage
# TODO: issue error "cannot merge previous GCDA file" when turn following switch.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Compile the unit test
add_subdirectory(test/unittest)

# Compile benchmark
add_subdirectory(benchmark)


#########################
# format and tidy
#########################
string(CONCAT FORMAT_DIR "${CMAKE_SOURCE_DIR}/src,"
                         "${CMAKE_SOURCE_DIR}/test,")   

add_custom_target(format ${TOOLS_PATH}/clang_format.py
  ${CLANG_FORMAT_BIN}
  ${TOOLS_PATH}/exclusion_files.txt
  --source_dirs ${FORMAT_DIR}
  --fix
  --quiet
)

add_custom_target(check-format ${TOOLS_PATH}/clang_format.py
  ${CLANG_FORMAT_BIN}
  ${TOOLS_PATH}/exclusion_files.txt
  --source_dirs ${FORMAT_DIR}
  --quiet
)

add_custom_target(tidy ${TOOLS_PATH}/run_clang_tidy.py
  -clang-tidy-binary ${CLANG_TIDY_BIN}
  -p ${CMAKE_BINARY_DIR}
  -extra-arg=-std=c++20
)

add_custom_target(fix-tidy ${TOOLS_PATH}/run_clang_tidy.py
  -clang-tidy-binary ${CLANG_TIDY_BIN}
  -p ${CMAKE_BINARY_DIR}
  -extra-arg=-std=c++20
  -fix
)

