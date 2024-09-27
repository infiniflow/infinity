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
import virtual_storage;
import virtual_storage_type;
import local_file;
import abstract_file_handle;

using namespace infinity;

class LocalFileTest : public BaseTest {};

TEST_F(LocalFileTest, TestAppend) {
    using namespace infinity;
    String path = String(GetFullTmpDir()) + "/test_file2.abc";

    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = local_file_handle->Open(path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    EXPECT_TRUE(virtual_storage.Exists(path));
    virtual_storage.DeleteFile(path);
    EXPECT_FALSE(virtual_storage.Exists(path));

    virtual_storage.UnInit();
}

TEST_F(LocalFileTest, TestDir) {
    using namespace infinity;
    String dir = String(GetFullTmpDir()) + "/unit_test";
    String path = dir + "/test_file.test";

    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    virtual_storage.MakeLocalDirectory(dir);

    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = local_file_handle->Open(path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    EXPECT_TRUE(virtual_storage.Exists(path));
    EXPECT_TRUE(virtual_storage.LocalExists(dir));

    virtual_storage.RemoveLocalDirectory(dir);
    EXPECT_FALSE(virtual_storage.Exists(path));
    EXPECT_FALSE(virtual_storage.Exists(dir));
}

TEST_F(LocalFileTest, TestRead) {
    using namespace infinity;

    String path = String(GetFullTmpDir()) + "/test_file_read.abc";

    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = local_file_handle->Open(path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for (SizeT i = 0; i < len; ++i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    status = local_file_handle->Open(path, FileAccessMode::kRead);
    EXPECT_TRUE(status.ok());

    UniquePtr<char[]> read_data = MakeUnique<char[]>(len);
    auto [read_len, read_status] = local_file_handle->Read(read_data.get(), len);
    EXPECT_TRUE(read_status.ok());
    local_file_handle->Close();

    EXPECT_EQ(read_len, len);
    for(SizeT i = 0; i < len; ++i) {
        EXPECT_EQ(read_data[i], i + 1);
    }

    virtual_storage.DeleteFile(path);
    EXPECT_FALSE(virtual_storage.Exists(path));
}

TEST_F(LocalFileTest, TestRename) {
    using namespace infinity;
    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    String old_path = String(GetFullTmpDir()) + "/test_file_old.abc";
    String new_path = String(GetFullTmpDir()) + "/test_file_new.abc";

    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = local_file_handle->Open(old_path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT len = 10;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    status = virtual_storage.RenameLocal(old_path, new_path);
    EXPECT_TRUE(status.ok());

    EXPECT_FALSE(virtual_storage.Exists(old_path));
    EXPECT_TRUE(virtual_storage.Exists(new_path));

    virtual_storage.DeleteFile(new_path);
    EXPECT_FALSE(virtual_storage.Exists(new_path));
}

TEST_F(LocalFileTest, TestTruncate) {
    using namespace infinity;
    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    String path = String(GetFullTmpDir()) + "/test_file_truncate.abc";

    auto [local_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = local_file_handle->Open(path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT len = 20;
    UniquePtr<char[]> data_array = MakeUnique<char[]>(len);
    for(SizeT i = 0; i < len; ++ i) {
        data_array[i] = i + 1;
    }

    local_file_handle->Append(data_array.get(), len);
    local_file_handle->Sync();
    local_file_handle->Close();

    virtual_storage.TruncateLocal(path, 10);

    status = local_file_handle->Open(path, FileAccessMode::kRead);
    EXPECT_TRUE(status.ok());

    UniquePtr<char[]> truncated_data = MakeUnique<char[]>(10);
    auto [read_len, read_status] = local_file_handle->Read(truncated_data.get(), 10);
    EXPECT_TRUE(read_status.ok());
    EXPECT_EQ(read_len, 10);

    status = local_file_handle->Close();
    EXPECT_TRUE(status.ok());

    for(SizeT i = 0; i < 10; ++i) {
        EXPECT_EQ(truncated_data[i], i + 1);
    }

    virtual_storage.DeleteFile(path);
    EXPECT_FALSE(virtual_storage.Exists(path));
}


TEST_F(LocalFileTest, TestMerge) {
    using namespace infinity;
    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    String dst_path = String(GetFullTmpDir()) + "/test_file_append_dst.abc";
    String src_path = String(GetFullTmpDir()) + "/test_file_append_src.abc";

    // Write source file
    auto [src_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = src_file_handle->Open(src_path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT src_len = 10;
    UniquePtr<char[]> data_array1 = MakeUnique<char[]>(src_len);
    for(SizeT i = 0; i < src_len; ++ i) {
        data_array1[i] = i + 1;
    }

    src_file_handle->Append(data_array1.get(), src_len);
    src_file_handle->Sync();
    src_file_handle->Close();

    auto [dst_file_handle, dst_status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(dst_status.ok());

    status = dst_file_handle->Open(dst_path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());

    SizeT dst_len = 10;
    UniquePtr<char[]> data_array2 = MakeUnique<char[]>(dst_len);
    for(SizeT i = 0; i < dst_len; ++ i) {
        data_array2[i] = i + 1;
    }

    dst_file_handle->Append(data_array2.get(), dst_len);
    dst_file_handle->Sync();
    dst_file_handle->Close();

    virtual_storage.MergeLocal(dst_path, src_path);
    
    auto [merge_file_handle, merge_status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(merge_status.ok());

    status = merge_file_handle->Open(dst_path, FileAccessMode::kRead);
    EXPECT_TRUE(status.ok());

    UniquePtr<char[]> combined_data = MakeUnique<char[]>(src_len + dst_len);
    auto [read_len, merge_read_status] = merge_file_handle->Read(combined_data.get(), src_len + dst_len);
    EXPECT_TRUE(merge_read_status.ok());
    merge_file_handle->Close();

    EXPECT_EQ(read_len, (i64)(src_len + dst_len));
    for(SizeT i = 0; i < dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i + 1);
    }
    for(SizeT i = dst_len; i < src_len + dst_len; ++i) {
        EXPECT_EQ(combined_data[i], i - dst_len + 1);
    }

    virtual_storage.DeleteFile(src_path);
    virtual_storage.DeleteFile(dst_path);
    EXPECT_FALSE(virtual_storage.Exists(src_path));
    EXPECT_FALSE(virtual_storage.Exists(dst_path));
}

TEST_F(LocalFileTest, TestCleanDir) {
    using namespace infinity;
    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);

    String dir = String(GetFullTmpDir()) + "/cleanup_test_dir";
    String file_path1 = dir + "/file1.txt";
    String file_path2 = dir + "/file2.txt";

    virtual_storage.MakeLocalDirectory(dir);

    // Append file1.txt
    auto [src_file_handle, status1] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status1.ok());

    status1 = src_file_handle->Open(file_path1, FileAccessMode::kWrite);
    EXPECT_TRUE(status1.ok());

    SizeT src_len = 10;
    UniquePtr<char[]> data_array1 = MakeUnique<char[]>(src_len);
    for(SizeT i = 0; i < src_len; ++ i) {
        data_array1[i] = i + 1;
    }

    src_file_handle->Append(data_array1.get(), src_len);
    src_file_handle->Sync();
    src_file_handle->Close();

    // Append more to file1.txt
    auto [append_src_file_handle, status2] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status2.ok());

    status2 = append_src_file_handle->Open(file_path1, FileAccessMode::kWrite);
    EXPECT_TRUE(status2.ok());

    append_src_file_handle->Append(data_array1.get(), src_len);
    append_src_file_handle->Sync();
    append_src_file_handle->Close();


    // Append more to file2.txt
    auto [append_file2_handle, status3] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status3.ok());

    status3 = append_file2_handle->Open(file_path2, FileAccessMode::kWrite);
    EXPECT_TRUE(status3.ok());

    SizeT len2 = 20;
    UniquePtr<char[]> data_array2 = MakeUnique<char[]>(len2);
    for(SizeT i = 0; i < len2; ++i) {
        data_array2[i] = i + 11;
    }

    append_file2_handle->Append(data_array2.get(), src_len);
    append_file2_handle->Sync();
    append_file2_handle->Close();

    virtual_storage.CleanupLocalDirectory(dir);

    EXPECT_FALSE(virtual_storage.Exists(file_path1));
    EXPECT_FALSE(virtual_storage.Exists(file_path2));
    EXPECT_TRUE(virtual_storage.Exists(dir));
    virtual_storage.RemoveLocalDirectory(dir);
    EXPECT_FALSE(virtual_storage.Exists(dir));
}
