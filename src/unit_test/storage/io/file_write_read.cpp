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

class FileWriteReadTest : public BaseTest {};

TEST_F(FileWriteReadTest, test1) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = "/tmp/test_file1.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (SizeT i = 0; i < 128; ++i) {
        String buffer = "abc";
        file_writer.Write(buffer.c_str(), buffer.size());
    }
    file_writer.Flush();
    EXPECT_EQ(file_writer.GetFileSize(), 128 * 3);
    EXPECT_EQ(file_writer.total_written_, 128 * 3);

    FileReader file_reader(local_file_system, path, 128);
    String read_str;
    read_str.resize(4);
    file_reader.Read(read_str.data(), 4);
    EXPECT_STREQ(read_str.c_str(), "abca");
    EXPECT_FALSE(file_reader.Finished());
    local_file_system.DeleteFile(path);
}
