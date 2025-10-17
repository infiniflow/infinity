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

module infinity_core:ut.block_version;

import :ut.base_test;
import :infinity;
import :infinity_exception;
import :status;
import third_party;
import :infinity_context;
import :block_version;
import :virtual_store;
import :fileworker_manager;
import :version_file_worker;
import :column_vector;
import :persistence_manager;
import :default_values;
import :local_file_handle;

import global_resource_usage;
import data_type;
import logical_type;

using namespace infinity;

class BlockVersionTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BlockVersionTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

// TEST_P(BlockVersionTest, SaveAndLoad) {
//     BlockVersion block_version(8192);
//     block_version.Append(10, 3);
//     block_version.Append(20, 6);
//     block_version.Delete(2, 30);
//     block_version.Delete(5, 40);
//     std::string version_path = std::string(GetFullDataDir()) + "/block_version_test";
//
//     {
//         auto [local_file_handle, status] = VirtualStore::Open(version_path, FileAccessMode::kWrite);
//         EXPECT_TRUE(status.ok());
//         block_version.SpillToFile(local_file_handle.get());
//     }
//
//     {
//         auto [local_file_handle, status] = VirtualStore::Open(version_path, FileAccessMode::kRead);
//         EXPECT_TRUE(status.ok());
//         auto block_version2 = BlockVersion::LoadFromFile(local_file_handle.get());
//         ASSERT_EQ(block_version, *block_version2);
//     }
// }

TEST_P(BlockVersionTest, DISABLED_XXX_SaveAndLoad2) {
    auto data_dir = std::make_shared<std::string>(std::string(GetFullDataDir()) + "/block_version_test");
    auto temp_dir = std::make_shared<std::string>(std::string(GetFullTmpDir()) + "/temp/block_version_test");
    auto persistence_dir = std::make_shared<std::string>(std::string(GetFullTmpDir()) + "/persistence/block_version_test");
    auto block_dir = std::make_shared<std::string>("block_version_test/block");
    auto version_file_name = std::make_shared<std::string>("block_version_test");

    {
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir, *version_file_name));
        auto file_worker = std::make_unique<VersionFileWorker>(rel_file_path, 8192);

        {
            auto *block_version = static_cast<BlockVersion *>(file_worker->GetData());

            block_version->Append(10, 3);
            block_version->Append(20, 6);
            block_version->Delete(2, 30);
            block_version->Delete(5, 40);
        }
        {
            [[maybe_unused]] bool foo{};
            file_worker->Write(foo, VersionFileWorkerSaveCtx(15));
        }
    }
    {
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir, *version_file_name));
        auto file_worker = std::make_unique<VersionFileWorker>(rel_file_path, 8192);
        {
            BlockVersion block_version1(8192);
            block_version1.Append(10, 3);

            {
                const auto *block_version = static_cast<const BlockVersion *>(file_worker->GetData());
                ASSERT_EQ(block_version1, *block_version);
            }
            auto *block_version = static_cast<BlockVersion *>(file_worker->GetData());
            block_version->Append(20, 6);
            block_version->Delete(2, 30);
            block_version->Delete(5, 40);
        }
        {
            [[maybe_unused]] bool foo{};
            file_worker->Write(foo, VersionFileWorkerSaveCtx(35));
        }
    }
    {
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir, *version_file_name));
        auto file_worker = std::make_unique<VersionFileWorker>(rel_file_path, 8192);
        {
            BlockVersion block_version1(8192);
            block_version1.Append(10, 3);
            block_version1.Append(20, 6);
            block_version1.Delete(2, 30);

            const auto *block_version = static_cast<const BlockVersion *>(file_worker->GetData());
            ASSERT_EQ(block_version1, *block_version);
        }
    }
}

TEST_P(BlockVersionTest, DISABLED_DEPRECATED_delete_test) {
    BlockVersion block_version(8192);
    block_version.Delete(2, 30);
    Status status = block_version.Delete(2, 30);
    EXPECT_FALSE(status.ok());
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
    auto res = std::make_shared<ColumnVector>(std::make_shared<DataType>(LogicalType::kTinyInt));
    res->Initialize();
    block_version.GetDeleteTS(2, 4, *res);
    EXPECT_EQ(res->ToString(0), "30");
    EXPECT_EQ(res->ToString(1), "0");
    EXPECT_EQ(res->ToString(2), "0");
    EXPECT_EQ(res->ToString(3), "40");
}