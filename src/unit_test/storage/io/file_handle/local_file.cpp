// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <gtest/gtest.h>
import base_test;
// import infinity_exception;
//
 import stl;
 import virtual_storage;
 import virtual_storage_type;
// import global_resource_usage;
// import third_party;
// import logger;
//
// import file_system;
// import local_file_system;
// import file_writer;
// import file_reader;
// import infinity_context;
// import stream_io;
// import file_system_type;
import local_file;
import abstract_file_handle;

using namespace infinity;

class LocalFileTest : public BaseTest {};

TEST_F(LocalFileTest, test1) {
    using namespace infinity;
    String dir = String(GetFullTmpDir());
    String path = dir + "/test_file.test";

    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);
    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    local_file_handle->Open(path, FileAccessMode::kWrite);

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    //    local_file_system.DeleteFile(path);
    //    EXPECT_FALSE(local_file_system.Exists(path));

    virtual_storage.UnInit();
}