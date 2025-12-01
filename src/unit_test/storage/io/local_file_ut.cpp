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

#include "unit_test/gtest_expand.h"

module infinity_core:ut.local_file;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :file_writer;
import :file_reader;
import :infinity_context;
import :virtual_store;

import global_resource_usage;

using namespace infinity;

class LocalFileTest : public BaseTest {};

TEST_F(LocalFileTest, test1) {
    std::string path = fmt::format("{}/{}", GetFullTmpDir(), "test_local_file.txt");
    auto [file_handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    ASSERT_TRUE(status.ok());

    status = file_handle->Append("abcdefghijklmnopqrstuvwxyz", 26);
    ASSERT_TRUE(status.ok());

    file_handle->Seek(0);

    {
        auto buffer = std::make_unique<char[]>(26);
        auto [read_size, status] = file_handle->Read(buffer.get(), 26);
        ASSERT_TRUE(status.ok());
        ASSERT_EQ(read_size, 26);
        for (auto i = 0; i < 26; i++) {
            ASSERT_EQ(buffer[i], 'a' + i);
        }
    }

    file_handle->Seek(0);

    {
        std::string buffer(64, '\0');
        auto [read_size, status] = file_handle->Read(buffer, 26);
        ASSERT_TRUE(status.ok());
        ASSERT_EQ(read_size, 26);
        for (auto i = 0; i < 26; i++) {
            ASSERT_EQ(buffer[i], 'a' + i);
        }
    }

    std::string mmap_path = fmt::format("{}/{}", GetFullTmpDir(), "test_mmap.txt");
    file_handle->MmapRead(mmap_path);
    file_handle->Unmmap(mmap_path);

    // file_handle->Close();
}