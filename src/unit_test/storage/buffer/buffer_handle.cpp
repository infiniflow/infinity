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

#include "gtest/gtest.h"
import base_test;
import stl;
import buffer_manager;
import data_file_worker;
import buffer_obj;
import infinity_exception;
import global_resource_usage;
import infinity_context;
import persistence_manager;
import default_values;

using namespace infinity;

class BufferHandleTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BufferHandleTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(BufferHandleTest, test1) {
    using namespace infinity;

    SizeT memory_limit = 1024;
    String data_dir(GetFullDataDir());
    auto temp_dir = MakeShared<String>(GetFullTmpDir());
    auto base_dir = MakeShared<String>(GetFullDataDir());
    auto persistence_dir = MakeShared<String>(GetFullPersistDir());

    UniquePtr<PersistenceManager> persistence_manager = MakeUnique<PersistenceManager>(*persistence_dir, *base_dir, DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT);
    BufferManager buffer_manager(memory_limit, base_dir, temp_dir, persistence_manager.get());

    SizeT test_size1 = 512;
    auto file_dir1 = MakeShared<String>("dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(base_dir, temp_dir, file_dir1, test_fname1, test_size1, buffer_manager.persistence_manager());
    auto buf1 = buffer_manager.AllocateBufferObject(std::move(file_worker1));

    SizeT test_size2 = 512;
    auto file_dir2 = MakeShared<String>("dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(base_dir, temp_dir, file_dir2, test_fname2, test_size2, buffer_manager.persistence_manager());
    auto buf2 = buffer_manager.AllocateBufferObject(std::move(file_worker2));

    SizeT test_size3 = 512;
    auto file_dir3 = MakeShared<String>("dir3");
    auto test_fname3 = MakeShared<String>("test3");
    auto file_worker3 = MakeUnique<DataFileWorker>(base_dir, temp_dir, file_dir3, test_fname3, test_size3, buffer_manager.persistence_manager());
    auto buf3 = buffer_manager.AllocateBufferObject(std::move(file_worker3));

    {
        auto buf_handle1 = buf1->Load();
        EXPECT_EQ(buf1->rc(), 1u);

        auto buf_handle2 = buf2->Load();

        // out of memory exception
        EXPECT_THROW({ auto buf_handle3 = buf3->Load(); }, UnrecoverableException);
        EXPECT_EQ(buf3->rc(), 0u);
        EXPECT_EQ(buf3->status(), BufferStatus::kNew);
        EXPECT_EQ(buf3->type(), BufferType::kEphemeral);
    }

    EXPECT_EQ(buf1->rc(), 0u);
    EXPECT_EQ(buf2->rc(), 0u);

    {
        auto buf_handle1 = buf1->Load();
        auto buf_handle1_1 = buf1->Load();
        EXPECT_EQ(buf1->rc(), 2u);
    }

    size_t write_size = 128;
    // 4 * 128 == 512
    {
        auto buf_handle1 = buf1->Load();

        auto data = static_cast<int *>(buf_handle1.GetDataMut());
        for (size_t i = 0; i < write_size; ++i) {
            data[i] = i;
        }
    }
    {
        auto buf_handle2 = buf2->Load();
        auto buf_handle3 = buf3->Load();
        // Flush buf1 to spill directory
    }
    {
        auto buf_handle1 = buf1->Load();
        auto data = static_cast<const int *>(buf_handle1.GetData());
        for (size_t i = 0; i < write_size; ++i) {
            EXPECT_EQ(data[i], (int)i);
        }

        buf1->Save();
    }
    {
        auto buf_handle2 = buf2->Load();
        auto buf_handle3 = buf3->Load();
        // Flush buf1 to spill directory
    }
    {
        auto buf_handle1 = buf1->Load();

        auto data = static_cast<int *>(buf_handle1.GetDataMut());
        for (size_t i = 0; i < write_size; ++i) {
            EXPECT_EQ(data[i], (int)i);
            data[i] = 2 * i;
        }
    }
    {
        auto buf_handle2 = buf2->Load();
        auto buf_handle3 = buf3->Load();
        // Flush buf1 to spill directory
    }
    {
        auto buf_handle1 = buf1->Load();

        auto data = static_cast<int *>(buf_handle1.GetDataMut());
        for (size_t i = 0; i < write_size; ++i) {
            EXPECT_EQ(data[i], int(2 * i));
        }
    }
}