# Copyright 2015 The RE2 Authors.  All Rights Reserved. Use of this source code
# is governed by a BSD-style license that can be found in the LICENSE file.

cmake_minimum_required(VERSION 3.5...3.29)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

project(RE2 CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include(CTest)

# CMake seems to have no way to enable/disable testing per subproject, so we
# provide an option similar to BUILD_TESTING, but just for RE2.
option(RE2_BUILD_TESTING "enable testing for RE2" OFF)

set(EXTRA_TARGET_LINK_LIBRARIES)

add_definitions(-DRE2_ON_VALGRIND)

if(WIN32)
  add_definitions(-DUNICODE
                  -D_UNICODE
                  -DSTRICT
                  -DNOMINMAX)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
elseif(UNIX)
  # add_compile_options(-pthread) list(APPEND EXTRA_TARGET_LINK_LIBRARIES
  # -pthread)
endif()

set(RE2_SOURCES
    re2/bitmap256.cc
    re2/compile.cc
    re2/bitstate.cc
    re2/dfa.cc
    re2/filtered_re2.cc
    re2/mimics_pcre.cc
    re2/nfa.cc
    re2/onepass.cc
    re2/parse.cc
    re2/perl_groups.cc
    re2/prefilter.cc
    re2/prefilter_tree.cc
    re2/prog.cc
    re2/re2.cc
    re2/regexp.cc
    re2/set.cc
    re2/simplify.cc
    re2/stringpiece.cc
    re2/tostring.cc
    re2/unicode_casefold.cc
    re2/unicode_groups.cc
    util/rune.cc
    util/strutil.cc
)

add_library(re2 STATIC ${RE2_SOURCES})

target_include_directories(
  re2
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

