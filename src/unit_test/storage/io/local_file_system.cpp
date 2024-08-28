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
import file_system_type;

class LocalFileSystemTest : public BaseTest {};

TEST_F(LocalFileSystemTest, file_write) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file2.abc";

    auto [file_handler, status] =
        local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), len);
    file_handler->Sync();
    file_handler->Close();
    local_file_system.DeleteFile(path);
    EXPECT_FALSE(local_file_system.Exists(path));
}

TEST_F(LocalFileSystemTest, dir_ops) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String dir = String(GetFullTmpDir()) + "/unit_test";
    String path = dir + "/test_file.test";

    local_file_system.CreateDirectory(dir);

    auto [file_handler, status] =
            local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), len);
    file_handler->Sync();
    file_handler->Close();

    local_file_system.DeleteDirectory(dir);
    EXPECT_FALSE(local_file_system.Exists(path));
    EXPECT_FALSE(local_file_system.Exists(dir));
}

TEST_F(LocalFileSystemTest, TestRead) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file_read.abc";

    auto [file_handler, status] =
        local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> write_data = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++i) {
        write_data[i] = i + 1;
    }
    file_handler->Write(write_data.get(), len);
    file_handler->Sync();
    file_handler->Close();

    auto [read_handler, read_status] =
        local_file_system.OpenFile(path, FileFlags::READ_FLAG, FileLockType::kNoLock);
    if(!read_status.ok()) {
        UnrecoverableError(read_status.message());
    }

    UniquePtr<char[]> read_data = MakeUnique<char[]>(len);
    i64 read_len = read_handler->Read(read_data.get(), len);
    read_handler->Close();
    
    EXPECT_EQ(read_len, len);
    for(SizeT i = 0; i < len; ++i) {
        EXPECT_EQ(read_data[i], i + 1);
    }
    local_file_system.DeleteFile(path);
    EXPECT_FALSE(local_file_system.Exists(path));
}

TEST_F(LocalFileSystemTest, TestRename) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String old_path = String(GetFullTmpDir()) + "/test_file_old.abc";
    String new_path = String(GetFullTmpDir()) + "/test_file_new.abc";

    auto [file_handler, status] =
        local_file_system.OpenFile(old_path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), len);
    file_handler->Sync();
    file_handler->Close();

    local_file_system.Rename(old_path, new_path);
    
    EXPECT_FALSE(local_file_system.Exists(old_path));
    EXPECT_TRUE(local_file_system.Exists(new_path));

    local_file_system.DeleteFile(new_path);
    EXPECT_FALSE(local_file_system.Exists(new_path));
}

TEST_F(LocalFileSystemTest, TestTruncate) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String path = String(GetFullTmpDir()) + "/test_file_truncate.abc";

    auto [file_handler, status] =
        local_file_system.OpenFile(path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT initial_len = 20;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(initial_len);
    for(SizeT i = 0; i < initial_len; ++i) {
        data_array[i] = i + 1;
    }
    file_handler->Write(data_array.get(), initial_len);
    file_handler->Sync();
    file_handler->Close();

    local_file_system.Truncate(path, 10);

    auto [truncated_handler, truncate_status] =
        local_file_system.OpenFile(path, FileFlags::READ_FLAG, FileLockType::kNoLock);
    if(!truncate_status.ok()) {
        UnrecoverableError(truncate_status.message());
    }

    UniquePtr<char[]> truncated_data = MakeUnique<char[]>(10);
    i64 read_len = truncated_handler->Read(truncated_data.get(), 10);
    truncated_handler->Close();
    
    EXPECT_EQ(read_len, 10);
    for(SizeT i = 0; i < 10; ++i) {
        EXPECT_EQ(truncated_data[i], i + 1);
    }

    local_file_system.DeleteFile(path);
    EXPECT_FALSE(local_file_system.Exists(path));
}

TEST_F(LocalFileSystemTest, TestAppend) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String dst_path = String(GetFullTmpDir()) + "/test_file_append_dst.abc";
    String src_path = String(GetFullTmpDir()) + "/test_file_append_src.abc";

    auto [src_handler, src_status] =
        local_file_system.OpenFile(src_path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!src_status.ok()) {
        UnrecoverableError(src_status.message());
    }

    SizeT src_len = 10;
    UniquePtr<char[]> src_data = MakeUnique<char[]>(src_len);
    for(SizeT i = 0; i < src_len; ++i) {
        src_data[i] = i + 1;
    }
    src_handler->Write(src_data.get(), src_len);
    src_handler->Sync();
    src_handler->Close();

    auto [dst_handler, dst_status] =
        local_file_system.OpenFile(dst_path, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!dst_status.ok()) {
        UnrecoverableError(dst_status.message());
    }

    SizeT dst_len = 5;
    UniquePtr<char[]> dst_data = MakeUnique<char[]>(dst_len);
    for(SizeT i = 0; i < dst_len; ++i) {
        dst_data[i] = i + 10;
    }
    dst_handler->Write(dst_data.get(), dst_len);
    dst_handler->Sync();
    dst_handler->Close();

    local_file_system.AppendFile(dst_path, src_path);

    auto [appended_handler, append_status] =
        local_file_system.OpenFile(dst_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
    if(!append_status.ok()) {
        UnrecoverableError(append_status.message());
    }

    UniquePtr<char[]> combined_data = MakeUnique<char[]>(src_len + dst_len);
    i64 read_len = appended_handler->Read(combined_data.get(), src_len + dst_len);
    appended_handler->Close();
    
    EXPECT_EQ(read_len, (i64)(src_len + dst_len));
    for(SizeT i = 0; i < dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i + 10);
    }
    for(SizeT i = dst_len; i < src_len + dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i - dst_len + 1);
    }

    local_file_system.DeleteFile(src_path);
    local_file_system.DeleteFile(dst_path);
    EXPECT_FALSE(local_file_system.Exists(src_path));
    EXPECT_FALSE(local_file_system.Exists(dst_path));
}

TEST_F(LocalFileSystemTest, TestCleanDir) {
    using namespace infinity;
    LocalFileSystem local_file_system;
    String dir = String(GetFullTmpDir()) + "/cleanup_test_dir";
    String file_path1 = dir + "/file1.txt";
    String file_path2 = dir + "/file2.txt";

    local_file_system.CreateDirectory(dir);

    auto [file_handler1, status1] =
        local_file_system.OpenFile(file_path1, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status1.ok()) {
        UnrecoverableError(status1.message());
    }
    SizeT len1 = 10;
    UniquePtr<char[]> data_array1 = MakeUnique<char[]>(len1);
    for(SizeT i = 0; i < len1; ++i) {
        data_array1[i] = i + 1;
    }
    file_handler1->Write(data_array1.get(), len1);
    file_handler1->Sync();
    file_handler1->Close();

    auto [file_handler2, status2] =
        local_file_system.OpenFile(file_path2, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kWriteLock);
    if(!status2.ok()) {
        UnrecoverableError(status2.message());
    }
    SizeT len2 = 20;
    UniquePtr<char[]> data_array2 = MakeUnique<char[]>(len2);
    for(SizeT i = 0; i < len2; ++i) {
        data_array2[i] = i + 11;
    }
    file_handler2->Write(data_array2.get(), len2);
    file_handler2->Sync();
    file_handler2->Close();

    local_file_system.CleanupDirectory(dir);

    EXPECT_FALSE(local_file_system.Exists(file_path1));
    EXPECT_FALSE(local_file_system.Exists(file_path2));
    EXPECT_TRUE(local_file_system.Exists(dir));
}