cmake_minimum_required(VERSION 3.10)

project(infinity VERSION 0.1.0)

# Print CMake version and project name
message(STATUS "Building ${PROJECT_NAME} with CMake version: ${CMAKE_VERSION}")

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

set(CMAKE_CXX_STANDARD 23)

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