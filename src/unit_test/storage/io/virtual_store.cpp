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

import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;

import file_writer;
import file_reader;
import infinity_context;
import virtual_store;
import local_file_handle;

using namespace infinity;

class VirtualStoreTest : public BaseTest {};

TEST_F(VirtualStoreTest, file_write) {
    using namespace infinity;
    String path = String(GetFullTmpDir()) + "/test_file2.abc";

    auto [file_handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }
    file_handle->Append(data_array.get(), len);
    file_handle->Sync();
    VirtualStore::DeleteFile(path);
    EXPECT_FALSE(VirtualStore::Exists(path));
}

TEST_F(VirtualStoreTest, dir_ops) {
    using namespace infinity;
    String dir = String(GetFullTmpDir()) + "/unit_test";
    String path = dir + "/test_file.test";

    VirtualStore::MakeDirectory(dir);

    auto [file_handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }
    file_handle->Append(data_array.get(), len);
    file_handle->Sync();

    VirtualStore::RemoveDirectory(dir);
    EXPECT_FALSE(VirtualStore::Exists(path));
    EXPECT_FALSE(VirtualStore::Exists(dir));
}

TEST_F(VirtualStoreTest, TestRead) {
    using namespace infinity;
    String path = String(GetFullTmpDir()) + "/test_file_read.abc";

    auto [file_handle, open_write_status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    if (!open_write_status.ok()) {
        UnrecoverableError(open_write_status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> write_data = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        write_data[i] = i + 1;
    }
    file_handle->Append(write_data.get(), len);
    file_handle->Sync();

    auto [read_handle, open_read_status] = VirtualStore::Open(path, FileAccessMode::kRead);
    if (!open_read_status.ok()) {
        UnrecoverableError(open_read_status.message());
    }

    UniquePtr<char[]> read_data = MakeUnique<char[]>(len);
    auto [read_len, read_status] = read_handle->Read(read_data.get(), len);
    EXPECT_TRUE(read_status.ok());

    EXPECT_EQ(read_len, len);
    for (SizeT i = 0; i < len; ++i) {
        EXPECT_EQ(read_data[i], i + 1);
    }
    VirtualStore::DeleteFile(path);
    EXPECT_FALSE(VirtualStore::Exists(path));
}

TEST_F(VirtualStoreTest, TestRename) {
    using namespace infinity;
    String old_path = String(GetFullTmpDir()) + "/test_file_old.abc";
    String new_path = String(GetFullTmpDir()) + "/test_file_new.abc";

    auto [file_handle, status] = VirtualStore::Open(old_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }
    file_handle->Append(data_array.get(), len);
    file_handle->Sync();

    VirtualStore::Rename(old_path, new_path);

    EXPECT_FALSE(VirtualStore::Exists(old_path));
    EXPECT_TRUE(VirtualStore::Exists(new_path));

    VirtualStore::DeleteFile(new_path);
    EXPECT_FALSE(VirtualStore::Exists(new_path));
}

TEST_F(VirtualStoreTest, TestTruncate) {
    using namespace infinity;
    String path = String(GetFullTmpDir()) + "/test_file_truncate.abc";

    auto [file_handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    SizeT initial_len = 20;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(initial_len);
    for (SizeT i = 0; i < initial_len; ++i) {
        data_array[i] = i + 1;
    }
    file_handle->Append(data_array.get(), initial_len);
    file_handle->Sync();

    VirtualStore::Truncate(path, 10);

    auto [truncated_handle, truncate_status] = VirtualStore::Open(path, FileAccessMode::kRead);
    if (!truncate_status.ok()) {
        UnrecoverableError(truncate_status.message());
    }

    UniquePtr<char[]> truncated_data = MakeUnique<char[]>(10);
    auto [read_len, read_status] = truncated_handle->Read(truncated_data.get(), 10);
    EXPECT_TRUE(read_status.ok());

    EXPECT_EQ(read_len, 10);
    for (SizeT i = 0; i < 10; ++i) {
        EXPECT_EQ(truncated_data[i], i + 1);
    }

    VirtualStore::DeleteFile(path);
    EXPECT_FALSE(VirtualStore::Exists(path));
}

TEST_F(VirtualStoreTest, TestAppend) {
    using namespace infinity;
    String dst_path = String(GetFullTmpDir()) + "/test_file_append_dst.abc";
    String src_path = String(GetFullTmpDir()) + "/test_file_append_src.abc";

    auto [src_handler, src_status] = VirtualStore::Open(src_path, FileAccessMode::kWrite);
    if (!src_status.ok()) {
        UnrecoverableError(src_status.message());
    }

    SizeT src_len = 10;
    UniquePtr<char[]> src_data = MakeUnique<char[]>(src_len);
    for (SizeT i = 0; i < src_len; ++i) {
        src_data[i] = i + 1;
    }
    src_handler->Append(src_data.get(), src_len);
    src_handler->Sync();

    auto [dst_handler, dst_status] = VirtualStore::Open(dst_path, FileAccessMode::kWrite);
    if (!dst_status.ok()) {
        UnrecoverableError(dst_status.message());
    }

    SizeT dst_len = 5;
    UniquePtr<char[]> dst_data = MakeUnique<char[]>(dst_len);
    for (SizeT i = 0; i < dst_len; ++i) {
        dst_data[i] = i + 10;
    }
    dst_handler->Append(dst_data.get(), dst_len);
    dst_handler->Sync();

    VirtualStore::Merge(dst_path, src_path);

    auto [appended_handle, append_status] = VirtualStore::Open(dst_path, FileAccessMode::kRead);
    if (!append_status.ok()) {
        UnrecoverableError(append_status.message());
    }

    UniquePtr<char[]> combined_data = MakeUnique<char[]>(src_len + dst_len);
    auto [read_len, read_status] = appended_handle->Read(combined_data.get(), src_len + dst_len);
    EXPECT_TRUE(read_status.ok());

    EXPECT_EQ(read_len, (i64)(src_len + dst_len));
    for (SizeT i = 0; i < dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i + 10);
    }
    for (SizeT i = dst_len; i < src_len + dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i - dst_len + 1);
    }

    VirtualStore::DeleteFile(src_path);
    VirtualStore::DeleteFile(dst_path);
    EXPECT_FALSE(VirtualStore::Exists(src_path));
    EXPECT_FALSE(VirtualStore::Exists(dst_path));
}

TEST_F(VirtualStoreTest, TestCleanDir) {
    using namespace infinity;
    String dir = String(GetFullTmpDir()) + "/cleanup_test_dir";
    String file_path1 = dir + "/file1.txt";
    String file_path2 = dir + "/file2.txt";

    VirtualStore::MakeDirectory(dir);

    auto [file_handler1, status1] = VirtualStore::Open(file_path1, FileAccessMode::kWrite);
    if (!status1.ok()) {
        UnrecoverableError(status1.message());
    }
    SizeT len1 = 10;
    UniquePtr<char[]> data_array1 = MakeUnique<char[]>(len1);
    for (SizeT i = 0; i < len1; ++i) {
        data_array1[i] = i + 1;
    }
    file_handler1->Append(data_array1.get(), len1);
    file_handler1->Sync();

    auto [file_handler2, status2] = VirtualStore::Open(file_path2, FileAccessMode::kWrite);
    if (!status2.ok()) {
        UnrecoverableError(status2.message());
    }
    SizeT len2 = 20;
    UniquePtr<char[]> data_array2 = MakeUnique<char[]>(len2);
    for (SizeT i = 0; i < len2; ++i) {
        data_array2[i] = i + 11;
    }
    file_handler2->Append(data_array2.get(), len2);
    file_handler2->Sync();

    VirtualStore::CleanupDirectory(dir);

    EXPECT_FALSE(VirtualStore::Exists(file_path1));
    EXPECT_FALSE(VirtualStore::Exists(file_path2));
    EXPECT_TRUE(VirtualStore::Exists(dir));
}

TEST_F(VirtualStoreTest, minio_upload) {
    using namespace infinity;

    auto test = [&]() {
        if (VirtualStore::BucketExists()) {
            String path = String(GetFullTmpDir()) + "/test_minio_upload.abc";
            String object_name = "test_minio_upload.abc";
            auto [file_handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            SizeT len = 10;
            UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
            for (SizeT i = 0; i < len; ++i) {
                data_array[i] = i + 1;
            }
            file_handle->Append(data_array.get(), len);
            file_handle->Sync();

            auto status1 = VirtualStore::UploadObject(path, object_name);
            EXPECT_TRUE(status1.ok());

            status1 = VirtualStore::RemoveObject(path);
            EXPECT_TRUE(status1.ok());

            VirtualStore::DeleteFile(path);
            EXPECT_FALSE(VirtualStore::Exists(path));
        } else {
            LOG_INFO("bucket existence check failed, skip the test");
        }
    };

    VirtualStore::InitRemoteStore();
    test();
    VirtualStore::UnInitRemoteStore();

    VirtualStore::InitRemoteStore(StorageType::kMinio, "192.168.200.165:9000", false, "minioadmin", "minioadmin", "infinity");
    test();
    VirtualStore::UnInitRemoteStore();
};
