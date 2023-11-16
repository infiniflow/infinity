# This code is released under the
# Apache License Version 2.0 http://www.apache.org/licenses/.
#
# Copyright (c) 2012 Louis Dionne
#
set (CMAKE_CXX_STANDARD 11) # for constexpr specifier and other goodies

if (NOT CMAKE_BUILD_TYPE)
		message(STATUS "No build type selected, default to Release")
		set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
include(AppendCompilerFlags)


project(FastPFOR CXX C)
set(PROJECT_URL "https://github.com/lemire/FastPFOR")
set(PROJECT_DESCRIPTION "The FastPFOR C++ library: Fast integer compression")
# Need to bump this when we release a new version.
set(PROJECT_VERSION "0.1.9")

include(DetectCPUFeatures)
#
# Runs compiler with "-dumpversion" and parses major/minor
# version with a regex.
#
# Taken&Modified from Boost.cmake
#
set (CMAKE_CXX_FLAGS_RELEASE "-Wall -Wcast-align -O3 -DNDEBUG -std=c++11 -DHAVE_CXX0X -msse4.1 -march=native")
set (CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wcast-align -ggdb  -std=c++11 -DHAVE_CXX0X -msse4.1 -march=native")
set (CMAKE_C_FLAGS_RELEASE "-Wall -Wcast-align -O3 -DNDEBUG -std=c99 -msse4.1 -march=native")
set (CMAKE_C_FLAGS_DEBUG   "-Wall -Wcast-align -ggdb  -std=c99 -msse4.1 -march=native")


MESSAGE( STATUS "CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE} )
MESSAGE( STATUS "CMAKE_C_FLAGS_DEBUG: " ${CMAKE_C_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_C_FLAGS_RELEASE: " ${CMAKE_C_FLAGS_RELEASE} )

add_definitions(-march=native)

# library target
include_directories(headers)
add_library(fastpfor STATIC
    src/bitpacking.cpp
    src/bitpackingaligned.cpp
    src/bitpackingunaligned.cpp
    src/horizontalbitpacking.cpp
    src/simdunalignedbitpacking.cpp
    src/simdbitpacking.cpp
    src/varintdecode.c
    src/streamvbyte.c)
set_target_properties(fastpfor PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
