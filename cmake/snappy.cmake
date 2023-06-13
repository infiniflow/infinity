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

FetchContent_DeclareGitHubWithMirror(snappy
  google/snappy f725f6766bfc62418c6491b504c8e5865ec99412
  MD5=17a982c9b0c667b3744e1fecba0046f7
)

FetchContent_MakeAvailableWithArgs(snappy
  CMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/cmake/modules
  SNAPPY_BUILD_TESTS=OFF
  SNAPPY_BUILD_BENCHMARKS=OFF
  BUILD_SHARED_LIBS=OFF
)
