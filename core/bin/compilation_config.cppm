//
// Created by JinHai on 2022/9/5.
//

module;

import stl;

export module compilation_config;

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#define BUILD_TIME "2023-10-15 16:10.14"
#define GIT_BRANCH_NAME "main"
#define GIT_COMMIT_ID ""
#define BUILD_TYPE "Release"

#define TEST_DATA_PATH "/home/jinhai/Documents/development/infinity/test/data"
#define CSV_DATA_PATH "/home/jinhai/Documents/development/infinity/third_party/zsv/data"

namespace infinity {

export i32 version_major() {
    return VERSION_MAJOR;
}

export i32 version_minor() {
    return VERSION_MINOR;
}

export i32 version_patch() {
    return VERSION_PATCH;
}

export char* current_system_time() {
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

}
