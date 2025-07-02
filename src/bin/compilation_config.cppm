// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module compilation_config;

#define VERSION_MAJOR 0
#define VERSION_MINOR 6
#define VERSION_PATCH 0

#define BUILD_TIME "2025-07-02 18:14.14"
#define GIT_BRANCH_NAME "main"
#define GIT_COMMIT_ID "nightly-64-b30d223b6"
#define BUILD_TYPE "Debug"

#define TEST_DATA_PATH "/home/infiniflow/zhengql/infinity/test/data"
#define CSV_DATA_PATH "/home/infiniflow/zhengql/infinity/third_party/zsv/data"
#define TMP_DATA_PATH "/home/infiniflow/zhengql/infinity/tmp"

#define ENABLE_JEMALLOC "OFF"

namespace infinity {

export int version_major() {
    return VERSION_MAJOR;
}

export int version_minor() {
    return VERSION_MINOR;
}

export int version_patch() {
    return VERSION_PATCH;
}

export char* system_build_time() {
    return (char*)BUILD_TIME;
}

export char* build_type() {
    return (char*)BUILD_TYPE;
}

export char* git_branch_name() {
    return (char*)GIT_BRANCH_NAME;
}

export char* git_commit_id() {
    return (char*)GIT_COMMIT_ID;
}

export char* test_data_path() {
    return (char*)TEST_DATA_PATH;
}

export char* csv_data_path() {
    return (char*)CSV_DATA_PATH;
}

export char* tmp_data_path() {
    return (char*)TMP_DATA_PATH;
}

}
