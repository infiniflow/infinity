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
import virtual_storage_type;
import virtual_storage;
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
        CleanupDbDirs();
    }
};

TEST_F(MinioFileTest, TestFileOpen) {
    using namespace infinity; 
    infinity::InfinityContext::instance().Init(MinioFileTest::config_path());

    VirtualStorage VS;
    VS.Init(StorageType::kMinio, default_config);
    auto [minio_file, status] = VS.BuildFileHandle();
    EXPECT_TRUE(status.ok());
    String tmp_dir(GetFullTmpDir());
    tmp_dir += "/minio_open_test.txt";
    Path p{tmp_dir};
    std::filesystem::remove(p);
    auto status1 = minio_file->Open(tmp_dir, FileAccessMode::kWrite);
    EXPECT_TRUE(status1.ok());
    auto status2 = minio_file->Append("hello world", 11);
    EXPECT_TRUE(status2.ok());
    auto status3 = minio_file->Sync();
    EXPECT_TRUE(status3.ok());
    auto status4 = minio_file->Close();
    EXPECT_TRUE(status4.ok());

    auto [minio_file2, status5] = VS.BuildFileHandle();
    EXPECT_TRUE(status5.ok());
    auto status6 = minio_file2->Open(tmp_dir, FileAccessMode::kRead);
    EXPECT_TRUE(status6.ok());
    String buffer;
    auto [read_n, status7] = minio_file2->Read(buffer, 11);
    EXPECT_TRUE(status7.ok());
    EXPECT_STREQ(buffer.c_str(), "hello world");
    auto status8 = minio_file2->Close();
    EXPECT_TRUE(status8.ok());

    //open after delete local file
    std::filesystem::remove(p);
    auto [minio_file3, status10] = VS.BuildFileHandle();
    EXPECT_TRUE(status10.ok());
    auto status11 = minio_file3->Open(tmp_dir, FileAccessMode::kRead);
    EXPECT_TRUE(status11.ok());
    buffer.clear();
    auto [read_n2, status12] = minio_file3->Read(buffer, 11);
    EXPECT_TRUE(status12.ok());
    EXPECT_STREQ(buffer.c_str(), "hello world");
    auto status13 = minio_file3->Close();
    EXPECT_TRUE(status13.ok());

    std::filesystem::remove(p);

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(MinioFileTest, TestFileAppendAndRead) {
    using namespace infinity;
    infinity::InfinityContext::instance().Init(MinioFileTest::config_path());

    VirtualStorage VS;
    VS.Init(StorageType::kMinio, default_config);
    auto [minio_file, status] = VS.BuildFileHandle();
    EXPECT_TRUE(status.ok());
    String tmp_dir(GetFullTmpDir());
    tmp_dir += "/minio_append_test.txt";
    Path p{tmp_dir};
    std::filesystem::remove(p);
    auto status1 = minio_file->Open(tmp_dir, FileAccessMode::kWrite);
    EXPECT_TRUE(status1.ok());
    //char *
    auto status2 = minio_file->Append("infinity", 8);
    EXPECT_TRUE(status2.ok());
    String str("infinity");
    //string
    auto status3 = minio_file->Append(str, 8);
    EXPECT_TRUE(status3.ok());
    auto status4 = minio_file->Sync();
    EXPECT_TRUE(status4.ok());
    auto status5 = minio_file->Close();
    EXPECT_TRUE(status5.ok());

    auto [minio_file2, status6] = VS.BuildFileHandle();
    EXPECT_TRUE(status6.ok());
    auto status7 = minio_file2->Open(tmp_dir, FileAccessMode::kRead);
    EXPECT_TRUE(status7.ok());
    String buffer;
    auto [read_n, status8] = minio_file2->Read(buffer, 16);
    EXPECT_TRUE(status8.ok());
    EXPECT_STREQ(buffer.c_str(), "infinityinfinity");
    auto status9 = minio_file2->Close();
    EXPECT_TRUE(status9.ok());

    auto [minio_file3, status10] = VS.BuildFileHandle();
    EXPECT_TRUE(status10.ok());
    auto status11 = minio_file3->Open(tmp_dir, FileAccessMode::kRead);
    EXPECT_TRUE(status11.ok());
    char *buffer1 = new char[17];
    auto [read_n1, status12] = minio_file3->Read(buffer1, 16);
    EXPECT_TRUE(status12.ok());
    buffer1[16] = '\0';
    EXPECT_STREQ(buffer1, "infinityinfinity");
    delete[] buffer1;
    auto status13 = minio_file3->Close();
    EXPECT_TRUE(status13.ok());

    std::filesystem::remove(p);

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(MinioFileTest, TestFileSize) {
    using namespace infinity;
    infinity::InfinityContext::instance().Init(MinioFileTest::config_path());

    VirtualStorage VS;
    VS.Init(StorageType::kMinio, default_config);
    auto [minio_file, status] = VS.BuildFileHandle();
    EXPECT_TRUE(status.ok());
    String tmp_dir(GetFullTmpDir());
    tmp_dir += "/minio_append_test.txt";
    Path p{tmp_dir};
    std::filesystem::remove(p);
    auto status1 = minio_file->Open(tmp_dir, FileAccessMode::kRead);
    EXPECT_TRUE(status1.ok());
    SizeT len =  minio_file->FileSize();
    EXPECT_EQ(len, 16);
    auto status2 = minio_file->Close();
    EXPECT_TRUE(status2.ok());
    std::filesystem::remove(p);

    auto [minio_file1, status3] = VS.BuildFileHandle();
    EXPECT_TRUE(status3.ok());
    String tmp_dir1(GetFullTmpDir());
    tmp_dir1 += "/minio_open_test.txt";
    Path p1{tmp_dir1};
    std::filesystem::remove(p1);
    auto status4 = minio_file1->Open(tmp_dir1, FileAccessMode::kRead);
    EXPECT_TRUE(status4.ok());
    len =  minio_file1->FileSize();
    EXPECT_EQ(len, 11);
    auto status5 = minio_file1->Close();
    EXPECT_TRUE(status5.ok());
    std::filesystem::remove(p1);

    infinity::InfinityContext::instance().UnInit();
}