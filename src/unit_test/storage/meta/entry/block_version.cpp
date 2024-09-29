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

import infinity;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import infinity_context;
import block_version;
import file_system;
import virtual_store;
import virtual_storage_type;
import abstract_file_handle;
import file_system_type;
import buffer_manager;
import version_file_worker;
import column_vector;
import data_type;
import logical_type;
import persistence_manager;
import default_values;

using namespace infinity;

class BlockVersionTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BlockVersionTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(BlockVersionTest, SaveAndLoad) {
    BlockVersion block_version(8192);
    block_version.Append(10, 3);
    block_version.Append(20, 6);
    block_version.Delete(2, 30);
    block_version.Delete(5, 40);
    String version_path = String(GetFullDataDir()) + "/block_version_test";

    {
        auto [local_file_handle, status] = LocalStore::Open(version_path, FileAccessMode::kWrite);
        EXPECT_TRUE(status.ok());
        block_version.SpillToFile(local_file_handle.get());
    }

    {
        auto [local_file_handle, status]  = LocalStore::Open(version_path, FileAccessMode::kRead);
        EXPECT_TRUE(status.ok());
        auto block_version2 = BlockVersion::LoadFromFile(local_file_handle.get());
        ASSERT_EQ(block_version, *block_version2);
    }
}

TEST_P(BlockVersionTest, SaveAndLoad2) {
    auto data_dir = MakeShared<String>(String(GetFullDataDir()) + "/block_version_test");
    auto temp_dir = MakeShared<String>(String(GetFullTmpDir()) + "/temp/block_version_test");
    auto persistence_dir = MakeShared<String>(String(GetFullTmpDir()) + "/persistence/block_version_test");
    auto block_dir = MakeShared<String>("block_version_test/block");
    auto version_file_name = MakeShared<String>("block_version_test");

    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

        auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
                                                         MakeShared<String>(String(GetFullTmpDir())),
                                                         block_dir,
                                                         version_file_name,
                                                         8192,
                                                         nullptr);
        auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));

        {
            auto block_version_handle = buffer_obj->Load();
            auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());

            block_version->Append(10, 3);
            block_version->Append(20, 6);
            block_version->Delete(2, 30);
            block_version->Delete(5, 40);
        }
        {
            buffer_obj->Save(VersionFileWorkerSaveCtx(15));
        }
    }
    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

        auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
                                                         MakeShared<String>(String(GetFullTmpDir())),
                                                         block_dir,
                                                         version_file_name,
                                                         8192,
                                                         nullptr);
        auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

        {
            BlockVersion block_version1(8192);
            block_version1.Append(10, 3);

            auto block_version_handle = buffer_obj->Load();
            {
                const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
                ASSERT_EQ(block_version1, *block_version);
            }
            auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());
            block_version->Append(20, 6);
            block_version->Delete(2, 30);
            block_version->Delete(5, 40);
        }
        {
            buffer_obj->Save(VersionFileWorkerSaveCtx(35));
        }
    }
    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

        auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
                                                         MakeShared<String>(String(GetFullTmpDir())),
                                                         block_dir,
                                                         version_file_name,
                                                         8192,
                                                         nullptr);
        auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

        {
            BlockVersion block_version1(8192);
            block_version1.Append(10, 3);
            block_version1.Append(20, 6);
            block_version1.Delete(2, 30);

            auto block_version_handle = buffer_obj->Load();
            const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
            ASSERT_EQ(block_version1, *block_version);
        }
    }
}

TEST_P(BlockVersionTest, delete_test) {
    BlockVersion block_version(8192);
    block_version.Delete(2, 30);
    EXPECT_THROW(block_version.Delete(2, 30), UnrecoverableException);
}

TEST_P(BlockVersionTest, check_delete_test) {
    BlockVersion block_version(8192);
    block_version.Delete(2, 30);
    EXPECT_TRUE(block_version.CheckDelete(2, 30));
    EXPECT_TRUE(block_version.CheckDelete(2, 40));
    EXPECT_FALSE(block_version.CheckDelete(2, 29));
    EXPECT_FALSE(block_version.CheckDelete(3, 30));
    EXPECT_FALSE(block_version.CheckDelete(8193, 30));
}

TEST_P(BlockVersionTest, get_delete_ts_test) {
    BlockVersion block_version(8192);
    block_version.Delete(2, 30);
    block_version.Delete(5, 40);
    auto res = MakeShared<ColumnVector>(MakeShared<DataType>(LogicalType::kTinyInt));
    res->Initialize();
    block_version.GetDeleteTS(2, 4, *res);
    EXPECT_EQ(res->ToString(0), "30");
    EXPECT_EQ(res->ToString(1), "0");
    EXPECT_EQ(res->ToString(2), "0");
    EXPECT_EQ(res->ToString(3), "40");
}
