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

#pragma once

#include "gtest/gtest.h"
#include <type_traits>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;

template <typename T>
class BaseTestWithParam : public std::conditional_t<std::is_same_v<T, void>, ::testing::Test, ::testing::TestWithParam<T>> {
public:
    BaseTestWithParam() {
        const char *infinity_home_ = GetHomeDir();
        bool ok = ValidateDirPermission(infinity_home_);
        if (!ok) {
            std::cerr << "Please create directory " << infinity_home_ << " and ensure current user has RWX permission of it." << std::endl;
            abort();
        }
        CleanupTmpDir();
    }

    ~BaseTestWithParam() override = default;

    void SetUp() override {}
    void TearDown() override {}

protected:
    const char *GetHomeDir() { return "/var/infinity"; }

    const char *GetDataDir() { return "/var/infinity/data"; }

    const char *GetWalDir() { return "/var/infinity/wal"; }

    const char *GetLogDir() { return "/var/infinity/log"; }

    const char *GetTmpDir() { return "/var/infinity/tmp"; }

    void CleanupDbDirs() {
        const char *infinity_db_dirs[] = {GetDataDir(), GetWalDir(), GetLogDir(), GetTmpDir()};
        for (auto &dir : infinity_db_dirs) {
            CleanupDirectory(dir);
        }
    }

    void CleanupTmpDir() { CleanupDirectory(GetTmpDir()); }

    void RemoveDbDirs() {
        const char *infinity_db_dirs[] = {GetDataDir(), GetWalDir(), GetLogDir(), GetTmpDir()};
        for (auto &dir : infinity_db_dirs) {
            RemoveDirectory(dir);
        }
    }

private:
    // Validate if given path satisfy all of following:
    // - Current user is root, or the owner of the path.
    // - The path is a directory.
    // - Owner has read, write, and execute permission of the path.
    bool ValidateDirPermission(const char *path) {
        uid_t current_uid = geteuid();
        struct stat sb;
        if (stat(path, &sb) != 0) {
            return false;
        }
        return ((sb.st_uid == current_uid || current_uid == 0) && (sb.st_mode & S_IFMT) == S_IFDIR && (sb.st_mode & S_IRWXU) == S_IRWXU);
    }

    void CleanupDirectory(const char *dir) {
        std::error_code error_code;
        fs::path p(dir);
        if (!fs::exists(dir)) {
            std::filesystem::create_directories(p, error_code);
            if (error_code.value() != 0) {
                std::cerr << "Failed to create directory " << dir << std::endl;
                abort();
            }
        }
        try {
            std::ranges::for_each(std::filesystem::directory_iterator{dir},
                                  [&](const auto &dir_entry) { std::filesystem::remove_all(dir_entry.path(), error_code); });
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Failed to cleanup " << dir << ", exception: " << e.what() << std::endl;
            abort();
        }
    }

    void RemoveDirectory(const char *dir) {
        std::error_code error_code;
        fs::path p(dir);
        try {
            std::filesystem::remove_all(p, error_code);
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Failed to remove " << dir << ", exception: " << e.what() << std::endl;
            abort();
        }
    }
};

using BaseTest = BaseTestWithParam<void>;
