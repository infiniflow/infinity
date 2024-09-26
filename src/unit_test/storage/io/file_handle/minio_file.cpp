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
import stl;
import status;
import infinity_context;
import compilation_config;
import virtual_storage_system_type;
import virtual_storage_system;
import abstract_file_handle;

import minio_file;

using namespace infinity;

Map<String, String> default_config = {
    {"url", "http://127.0.0.1:9000"},
    {"access_key", "minioadmin"},
    {"secret_key", "minioadmin"},
    {"enable_https", "false"},
    {"disk_cache_dir", ""},
    {"disk_cache_limit", "0"},
    {"disk_cache_lru_count", "0"},
};

class MinioFileTest : public BaseTest {
protected:
    static std::shared_ptr<std::string> config_path() {
        return std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_minio_s3_storage.toml");
    }

    void SetUp() override {
        CleanupDbDirs();
    }

    void TearDown() override {

    }
};

TEST_F(MinioFileTest, TestFileOpen) {
    using namespace infinity; 
    infinity::InfinityContext::instance().Init(MinioFileTest::config_path());

    VirtualStorageSystem VSS;
    VSS.Init(StorageType::kMinio, default_config);
    auto [minio_file, status] = VSS.BuildFileHandle();
    EXPECT_TRUE(status.ok());
    String home_dir(GetHomeDir());
    auto status1 = minio_file->Open(home_dir + "/minio_test.txt", FileAccessMode::kWrite);
    EXPECT_TRUE(status1.ok());
    auto status2 = minio_file->Append("hello world", 11);
    EXPECT_TRUE(status2.ok());
    auto status3 = minio_file->Sync();
    EXPECT_TRUE(status3.ok());
    auto status4 = minio_file->Close();
    EXPECT_TRUE(status4.ok());

    infinity::InfinityContext::instance().UnInit();
}