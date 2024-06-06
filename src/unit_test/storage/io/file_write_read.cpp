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

import global_resource_usage;
import infinity_context;

class FileWriteReadTest : public BaseTest {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }
};

TEST_F(FileWriteReadTest, test1) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetTmpDir()) + "/test_file1.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (SizeT i = 0; i < 128; ++i) {
        String buffer = "abc";
        file_writer.Write(buffer.c_str(), buffer.size());
    }
    file_writer.Flush();
    EXPECT_EQ(file_writer.GetFileSize(), 128 * 3);
    EXPECT_EQ(file_writer.total_written_, (SizeT)128 * 3);

    FileReader file_reader(local_file_system, path, 128);
    String read_str;
    read_str.resize(4);
    file_reader.Read(read_str.data(), 4);
    EXPECT_STREQ(read_str.c_str(), "abca");
    EXPECT_FALSE(file_reader.Finished());
    local_file_system.DeleteFile(path);
}

TEST_F(FileWriteReadTest, test2) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetTmpDir()) + "/test_file2.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (u32 i = 0; i < 128; ++i) {
        file_writer.WriteVInt(i);
    }
    file_writer.Flush();

    FileReader file_reader(local_file_system, path, 128);
    for (u32 i = 0; i < 128; ++i) {
        u32 a = file_reader.ReadVInt();
        EXPECT_EQ(a, i);
    }
    local_file_system.DeleteFile(path);
}

TEST_F(FileWriteReadTest, test3) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetTmpDir()) + "/test_file3.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (u32 i = 0; i < 128; ++i) {
        file_writer.WriteVInt(i);
    }
    for (SizeT i = 0; i < 3; ++i) {
        String buffer = "abc";
        file_writer.Write(buffer.c_str(), buffer.size());
    }
    for (u32 i = 0; i < 128; ++i) {
        file_writer.WriteVInt(i);
    }
    file_writer.Flush();

    FileReader file_reader(local_file_system, path, 128);
    for (u32 i = 0; i < 128; ++i) {
        u32 a = file_reader.ReadVInt();
        EXPECT_EQ(a, i);
    }
    String read_str;
    read_str.resize(9);
    file_reader.Read(read_str.data(), 9);
    EXPECT_STREQ(read_str.c_str(), "abcabcabc");
    for (u32 i = 0; i < 128; ++i) {
        u32 a = file_reader.ReadVInt();
        EXPECT_EQ(a, i);
    }
    local_file_system.DeleteFile(path);
}
