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

#include "unit_test/base_test.h"

import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;

import file_system;
import local_file_system;
import file_writer;
import file_reader;
import infinity_context;
import file_system_type;

class LocalFileSystemTest : public BaseTest {};

TEST_F(LocalFileSystemTest, file_write) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetTmpDir()) + "/test_file2.abc";

    auto [file_handler, status] =
        local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), len);
    file_handler->Sync();
    file_handler->Close();
    local_file_system.DeleteFile(path);
    EXPECT_FALSE(local_file_system.Exists(path));
}

TEST_F(LocalFileSystemTest, dir_ops) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String dir = String(GetTmpDir()) + "/unit_test";
    String path = dir + "/test_file.test";

    local_file_system.CreateDirectory(dir);

    auto [file_handler, status] =
            local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), len);
    file_handler->Sync();
    file_handler->Close();

    local_file_system.DeleteDirectory(dir);
    EXPECT_FALSE(local_file_system.Exists(path));
    EXPECT_FALSE(local_file_system.Exists(dir));
}
