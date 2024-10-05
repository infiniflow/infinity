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

#include <gtest/gtest.h>

import base_test;
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

using namespace infinity;

class FileWriteReadTest : public BaseTest {};

// write in abcabcabc...for 128 times, then read first 4 bytes
TEST_F(FileWriteReadTest, test1) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file1.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (SizeT i = 0; i < 128; ++i) {
        String buffer = "abc";
        file_writer.Write(buffer.c_str(), buffer.size());
    }
    file_writer.Flush();
    EXPECT_EQ(file_writer.GetFileSize(), 128 * 3);
    EXPECT_EQ(file_writer.total_written_, (SizeT)128 * 3);

    FileReader file_reader(path, 128);
    String read_str;
    read_str.resize(4);
    file_reader.Read(read_str.data(), 4);
    EXPECT_STREQ(read_str.c_str(), "abca");
    EXPECT_FALSE(file_reader.Finished());
    local_file_system.DeleteFile(path);
}

//write vint then read vint
TEST_F(FileWriteReadTest, test2) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file2.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (u32 i = 0; i < 128; ++i) {
        file_writer.WriteVInt(i);
    }
    file_writer.Flush();

    FileReader file_reader(path, 128);
    for (u32 i = 0; i < 128; ++i) {
        u32 a = file_reader.ReadVInt();
        EXPECT_EQ(a, i);
    }
    local_file_system.DeleteFile(path);
}

//hybrid datatype
TEST_F(FileWriteReadTest, test3) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file3.abc";
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

    FileReader file_reader(path, 128);
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


//test total written bytes and GetFileSize()
//plus exceed case for reader/writer buffer
TEST_F(FileWriteReadTest, TestExceedWriterTotalSize) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file_write_bytes.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for(i32 i = 0; i < 1024; ++i) {
        file_writer.WriteInt(i);
    }

    String buffer = "abcabcabc";
    file_writer.Write(buffer.c_str(), buffer.size());
    file_writer.Sync();

    FileReader file_reader(path, 128);
    for (i32 i = 0; i < 1024; ++i) {
        i32 a = file_reader.ReadInt();
        EXPECT_EQ(a, i);
    }
    String read_str;
    read_str.resize(9);
    file_reader.Read(read_str.data(), 9);
    EXPECT_STREQ(read_str.c_str(), "abcabcabc");

    EXPECT_EQ(file_writer.GetFileSize(), 4 * 1024 + buffer.size());
    EXPECT_EQ(file_writer.TotalWrittenBytes(), 4 * 1024 + buffer.size());
}

//write byte in '0', '1'...'1023'
//read to '254', get pointer a, finish the read
//seek a, finish
TEST_F(FileWriteReadTest, TestFilePointerSeek) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file_write_bytes.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for(i32 i = 0; i < 1024; i++) {
        file_writer.WriteInt(i);
    }
    file_writer.Sync();

    FileReader file_reader(path, 128);
    u64 a;
    for (i32 i = 0; i < 1024; ++i) {
        if(i == 254) {
            a = file_reader.GetFilePointer();
        }
        EXPECT_EQ(file_reader.ReadInt(), i);
    }
    EXPECT_TRUE(file_reader.Finished());

    file_reader.Seek(a);
    i32 exp = 254;
    while(!file_reader.Finished()) { 
        EXPECT_EQ(file_reader.ReadInt(), exp);
        exp++;
    }
    EXPECT_EQ(exp, 1024);
}

//test if ReFill works fine.
TEST_F(FileWriteReadTest, TestFileReadOverflowBuffer) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file_write_bytes.abc";
    FileWriter file_writer(local_file_system, path, 128);

    String s;
    for(i32 i = 0; i < 1000; i++) {
        s += "abc";
    }

    file_writer.Write(s.c_str(), s.size());
    file_writer.Sync();

    String read_s;
    read_s.resize(s.size());
    FileReader file_reader(path, 128);
    file_reader.Read(read_s.data(), read_s.size());

    EXPECT_STREQ(s.c_str(), read_s.c_str());
}

//test all types of data of reader and writer
TEST_F(FileWriteReadTest, TestFileIODataTypes) {

    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_io_alltypes.abc";
    FileWriter file_writer(local_file_system, path, 128);

    file_writer.WriteByte('a');
    file_writer.WriteInt(4);
    file_writer.WriteVInt(23);
    file_writer.WriteLong(4566);
    file_writer.WriteVLong(45639);
    file_writer.WriteShort(328);

    String s = "linelineline";
    file_writer.Write(s.data(), s.size());
    file_writer.Sync();

    FileReader file_reader(path, 128);
    EXPECT_EQ(file_reader.ReadByte(), 'a');
    EXPECT_EQ(file_reader.ReadInt(), 4);
    EXPECT_EQ(file_reader.ReadVInt(), 23);
    EXPECT_EQ(file_reader.ReadLong(), 4566);
    EXPECT_EQ(file_reader.ReadVLong(), 45639);
    EXPECT_EQ(file_reader.ReadShort(), 328);
}