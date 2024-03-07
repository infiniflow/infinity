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

include(cmake/utils.cmake)

FetchContent_DeclareGitHubWithMirror(zstd
  facebook/zstd v1.5.5
  MD5=f336cde1961ee7e5d3a7f8c0c0f96987
)

FetchContent_GetProperties(zstd)
if(NOT zstd_POPULATED)
  FetchContent_Populate(zstd)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(APPLE_FLAG "CFLAGS=-isysroot ${CMAKE_OSX_SYSROOT}")
  endif()

  add_custom_target(make_zstd COMMAND make CC=${CMAKE_C_COMPILER} ${APPLE_FLAG} libzstd.a
    WORKING_DIRECTORY ${zstd_SOURCE_DIR}/lib
    BYPRODUCTS ${zstd_SOURCE_DIR}/lib/libzstd.a
  )
endif()

add_library(zstd INTERFACE)
target_include_directories(zstd INTERFACE $<BUILD_INTERFACE:${zstd_SOURCE_DIR}/lib>)
target_link_libraries(zstd INTERFACE $<BUILD_INTERFACE:${zstd_SOURCE_DIR}/lib/libzstd.a>)
add_dependencies(zstd make_zstd)
