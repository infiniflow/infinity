# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

include_guard()

include(FetchContent)

macro(parse_var arg key value)
  string(REGEX REPLACE "^(.+)=(.+)$" "\\1;\\2" REGEX_RESULT ${arg})
  list(GET REGEX_RESULT 0 ${key})
  list(GET REGEX_RESULT 1 ${value})
endmacro()

function(FetchContent_MakeAvailableWithArgs dep)
  if(NOT ${dep}_POPULATED)
    FetchContent_Populate(${dep})

    foreach(arg IN LISTS ARGN)
      parse_var(${arg} key value)
      set(${key}_OLD ${${key}})
      set(${key} ${value} CACHE INTERNAL "")
    endforeach()

    add_subdirectory(${${dep}_SOURCE_DIR} ${${dep}_BINARY_DIR} EXCLUDE_FROM_ALL)

    foreach(arg IN LISTS ARGN)
      parse_var(${arg} key value)
      set(${key} ${${key}_OLD} CACHE INTERNAL "")
    endforeach()
  endif()
endfunction()

function(FetchContent_DeclareWithMirror dep url hash)
  FetchContent_Declare(${dep}
    URL ${DEPS_FETCH_PROXY}${url}
    URL_HASH ${hash}
  )
endfunction()

function(FetchContent_DeclareGitHubWithMirror dep repo tag hash)
  FetchContent_DeclareWithMirror(${dep}
    https://github.com/${repo}/archive/${tag}.zip
    ${hash}
  )
endfunction()

macro(SET_FIND_LIBRARY_OPTIONS _prefixes _suffixes)
  set(_CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
  set(_CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")

  set(CMAKE_FIND_LIBRARY_PREFIXES "${_prefixes}" CACHE INTERNAL "" FORCE)
  set(CMAKE_FIND_LIBRARY_SUFFIXES "${_suffixes}" CACHE INTERNAL "" FORCE)
endmacro()

macro(RESTORE_FIND_LIBRARY_OPTIONS)
  set(CMAKE_FIND_LIBRARY_PREFIXES "${_CMAKE_FIND_LIBRARY_PREFIXES}" CACHE INTERNAL "" FORCE)
  set(CMAKE_FIND_LIBRARY_SUFFIXES "${_CMAKE_FIND_LIBRARY_SUFFIXES}" CACHE INTERNAL "" FORCE)
endmacro()

macro(ADD_OPTION_GPROF DEFAULT)
  if (CMAKE_COMPILER_IS_GNUCXX)
    option(USE_GPROF "Compile using -pg for gprof output" ${DEFAULT})

    if (USE_GPROF)
      message(STATUS "Using gprof output for ${CMAKE_PROJECT_NAME}")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    else ()
      message(STATUS "NOT using gprof output for ${CMAKE_PROJECT_NAME}")
    endif ()
  else ()
    message(STATUS "gprof generation NOT AVAILABLE - Not a GNU compiler")
  endif ()
endmacro()

# expands ~ to user home directory
#
# usage:
# expand_path("~/code" x)
function(expand_path in outvar)
  string(SUBSTRING ${in} 0 1 first)
  if (NOT ${first} STREQUAL "~")
    set(${outvar} ${in} PARENT_SCOPE)
    return()
  endif ()

  if (WIN32 AND NOT CYGWIN)
    set(home $ENV{USERPROFILE})
  else ()
    set(home $ENV{HOME})
  endif ()

  if (NOT home)
    set(${outvar} ${in} PARENT_SCOPE)
    return()
  endif ()

  string(SUBSTRING ${in} 1 -1 tail)
  if (CMAKE_VERSION VERSION_LESS 3.20)
    file(TO_CMAKE_PATH ${home}${tail} out)
  else ()
    cmake_path(CONVERT ${home}${tail} TO_CMAKE_PATH_LIST out)
  endif ()

  set(${outvar} ${out} PARENT_SCOPE)
endfunction(expand_path)
