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
#include <string>

import infinity;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import infinity_context;
import block_version;
import file_system;
import local_file_system;
import file_system_type;
import buffer_manager;
import version_file_worker;

using namespace infinity;

class BlockVersionTest : public BaseTestParamStr {
protected:
    void SetUp() override {
        RemoveDbDirs();
        system(("mkdir -p " + String(GetFullPersistDir())).c_str());
        system(("mkdir -p " + String(GetFullDataDir())).c_str());
        system(("mkdir -p " + String(GetFullTmpDir())).c_str());

        std::string config_path_str = GetParam();
        std::shared_ptr<std::string> config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        InfinityContext::instance().Init(config_path);
    }

    void TearDown() override { InfinityContext::instance().UnInit(); }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BlockVersionTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH,
                                           BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(BlockVersionTest, SaveAndLoad) {
    BlockVersion block_version(8192);
    block_version.created_.emplace_back(10, 3);
    block_version.created_.emplace_back(20, 6);
    block_version.deleted_[2] = 30;
    block_version.deleted_[5] = 40;
    String version_path = String(GetFullDataDir()) + "/block_version_test";
    LocalFileSystem fs;

    {
        auto [file_handler, status] = fs.OpenFile(version_path, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        block_version.SpillToFile(*file_handler);
    }

    {
        auto [file_handler, status] = fs.OpenFile(version_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        auto block_version2 = BlockVersion::LoadFromFile(*file_handler);
        ASSERT_EQ(block_version, *block_version2);
    }
}

TEST_P(BlockVersionTest, SaveAndLoad2) {
    auto data_dir = MakeShared<String>(String(GetFullDataDir()) + "/block_version_test");
    auto temp_dir = MakeShared<String>(String(GetFullTmpDir()) + "/temp/block_version_test");
    auto block_dir = MakeShared<String>(*data_dir + "/block");
    auto version_file_name = MakeShared<String>("block_version_test");

    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir);

        auto file_worker = MakeUnique<VersionFileWorker>(block_dir, version_file_name, 8192);
        auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));

        {
            auto block_version_handle = buffer_obj->Load();
            auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());

            block_version->created_.emplace_back(10, 3);
            block_version->created_.emplace_back(20, 6);
            block_version->deleted_[2] = 30;
            block_version->deleted_[5] = 40;
        }
        {
            auto *file_worker = static_cast<VersionFileWorker *>(buffer_obj->file_worker());
            file_worker->SetCheckpointTS(15);
            buffer_obj->Save();
        }
    }
    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir);

        auto file_worker = MakeUnique<VersionFileWorker>(block_dir, version_file_name, 8192);
        auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

        {
            BlockVersion block_version1(8192);
            block_version1.created_.emplace_back(10, 3);

            auto block_version_handle = buffer_obj->Load();
            {
                const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
                ASSERT_EQ(block_version1, *block_version);
            }
            auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());
            block_version->created_.emplace_back(20, 6);
            block_version->deleted_[2] = 30;
            block_version->deleted_[5] = 40;
        }
        {
            auto *file_worker = static_cast<VersionFileWorker *>(buffer_obj->file_worker());
            file_worker->SetCheckpointTS(35);
            buffer_obj->Save();
        }
    }
    {
        BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir);

        auto file_worker = MakeUnique<VersionFileWorker>(block_dir, version_file_name, 8192);
        auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

        {
            BlockVersion block_version1(8192);
            block_version1.created_.emplace_back(10, 3);
            block_version1.created_.emplace_back(20, 6);
            block_version1.deleted_[2] = 30;

            auto block_version_handle = buffer_obj->Load();
            const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
            ASSERT_EQ(block_version1, *block_version);
        }
    }
}
