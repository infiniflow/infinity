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

import infinity;
import infinity_exception;
import status;
import stl;
import global_resource_usage;
import third_party;
import infinity_context;
import block_version;
import virtual_store;
import buffer_manager;
import version_file_worker;
import column_vector;
import data_type;
import logical_type;
import persistence_manager;
import default_values;
import local_file_handle;

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
//     String version_path = String(GetFullDataDir()) + "/block_version_test";

//     {
//         auto [local_file_handle, status] = VirtualStore::Open(version_path, FileAccessMode::kWrite);
//         EXPECT_TRUE(status.ok());
//         block_version.SpillToFile(local_file_handle.get());
//     }

//     {
//         auto [local_file_handle, status] = VirtualStore::Open(version_path, FileAccessMode::kRead);
//         EXPECT_TRUE(status.ok());
//         auto block_version2 = BlockVersion::LoadFromFile(local_file_handle.get());
//         ASSERT_EQ(block_version, *block_version2);
//     }
// }

// TEST_P(BlockVersionTest, SaveAndLoad2) {
//     auto data_dir = MakeShared<String>(String(GetFullDataDir()) + "/block_version_test");
//     auto temp_dir = MakeShared<String>(String(GetFullTmpDir()) + "/temp/block_version_test");
//     auto persistence_dir = MakeShared<String>(String(GetFullTmpDir()) + "/persistence/block_version_test");
//     auto block_dir = MakeShared<String>("block_version_test/block");
//     auto version_file_name = MakeShared<String>("block_version_test");

//     {
//         BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

//         auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
//                                                          MakeShared<String>(String(GetFullTmpDir())),
//                                                          block_dir,
//                                                          version_file_name,
//                                                          8192,
//                                                          nullptr);
//         auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));

//         {
//             auto block_version_handle = buffer_obj->Load();
//             auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());

//             block_version->Append(10, 3);
//             block_version->Append(20, 6);
//             block_version->Delete(2, 30);
//             block_version->Delete(5, 40);
//         }
//         {
//             buffer_obj->Save(VersionFileWorkerSaveCtx(15));
//         }
//     }
//     {
//         BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

//         auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
//                                                          MakeShared<String>(String(GetFullTmpDir())),
//                                                          block_dir,
//                                                          version_file_name,
//                                                          8192,
//                                                          nullptr);
//         auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

//         {
//             BlockVersion block_version1(8192);
//             block_version1.Append(10, 3);

//             auto block_version_handle = buffer_obj->Load();
//             {
//                 const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
//                 ASSERT_EQ(block_version1, *block_version);
//             }
//             auto *block_version = static_cast<BlockVersion *>(block_version_handle.GetDataMut());
//             block_version->Append(20, 6);
//             block_version->Delete(2, 30);
//             block_version->Delete(5, 40);
//         }
//         {
//             buffer_obj->Save(VersionFileWorkerSaveCtx(35));
//         }
//     }
//     {
//         BufferManager buffer_mgr(1 << 20 /*memory limit*/, data_dir, temp_dir, nullptr);

//         auto file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(String(GetFullDataDir())),
//                                                          MakeShared<String>(String(GetFullTmpDir())),
//                                                          block_dir,
//                                                          version_file_name,
//                                                          8192,
//                                                          nullptr);
//         auto *buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));

//         {
//             BlockVersion block_version1(8192);
//             block_version1.Append(10, 3);
//             block_version1.Append(20, 6);
//             block_version1.Delete(2, 30);

//             auto block_version_handle = buffer_obj->Load();
//             const auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
//             ASSERT_EQ(block_version1, *block_version);
//         }
//     }
// }

// TEST_P(BlockVersionTest, delete_test) {
//     BlockVersion block_version(8192);
//     block_version.Delete(2, 30);
//     Status status = block_version.Delete(2, 30);
//     EXPECT_FALSE(status.ok());
// }

// TEST_P(BlockVersionTest, check_delete_test) {
//     BlockVersion block_version(8192);
//     block_version.Delete(2, 30);
//     EXPECT_TRUE(block_version.CheckDelete(2, 30));
//     EXPECT_TRUE(block_version.CheckDelete(2, 40));
//     EXPECT_FALSE(block_version.CheckDelete(2, 29));
//     EXPECT_FALSE(block_version.CheckDelete(3, 30));
//     EXPECT_FALSE(block_version.CheckDelete(8193, 30));
// }

// TEST_P(BlockVersionTest, get_delete_ts_test) {
//     BlockVersion block_version(8192);
//     block_version.Delete(2, 30);
//     block_version.Delete(5, 40);
//     auto res = MakeShared<ColumnVector>(MakeShared<DataType>(LogicalType::kTinyInt));
//     res->Initialize();
//     block_version.GetDeleteTS(2, 4, *res);
//     EXPECT_EQ(res->ToString(0), "30");
//     EXPECT_EQ(res->ToString(1), "0");
//     EXPECT_EQ(res->ToString(2), "0");
//     EXPECT_EQ(res->ToString(3), "40");
// }

// // restore snapshot test
// TEST_P(BlockVersionTest, test_restore_from_snapshot) {
//     // Create a block version with some data
//     BlockVersion block_version(8192);
    
//     // Add some creation records with different timestamps
//     block_version.Append(10, 5);   // Create 5 rows at timestamp 10
//     block_version.Append(20, 3);   // Create 3 more rows at timestamp 20
//     block_version.Append(30, 2);   // Create 2 more rows at timestamp 30
    
//     // Add some deletion records
//     block_version.Delete(2, 25);   // Delete row 2 at timestamp 25
//     block_version.Delete(7, 35);   // Delete row 7 at timestamp 35
    
//     // Verify initial state
//     EXPECT_EQ(block_version.created_.size(), 3);
//     EXPECT_EQ(block_version.created_[0].create_ts_, 10);
//     EXPECT_EQ(block_version.created_[0].row_count_, 5);
//     EXPECT_EQ(block_version.created_[1].create_ts_, 20);
//     EXPECT_EQ(block_version.created_[1].row_count_, 3);
//     EXPECT_EQ(block_version.created_[2].create_ts_, 30);
//     EXPECT_EQ(block_version.created_[2].row_count_, 2);
    
//     EXPECT_EQ(block_version.deleted_[2], 25);
//     EXPECT_EQ(block_version.deleted_[7], 35);
    
//     // Test RestoreFromSnapshot
//     TxnTimeStamp restore_ts = 100;
//     block_version.RestoreFromSnapshot(restore_ts);
    
//     // Verify the restoration worked correctly
//     // 1. All creation records should be consolidated into one with restore_ts
//     EXPECT_EQ(block_version.created_.size(), 1);
//     EXPECT_EQ(block_version.created_[0].create_ts_, restore_ts);
//     EXPECT_EQ(block_version.created_[0].row_count_, 10);  // Total rows: 5+3+2=10
    
//     // 2. All deletion timestamps should be updated to restore_ts
//     EXPECT_EQ(block_version.deleted_[2], restore_ts);
//     EXPECT_EQ(block_version.deleted_[7], restore_ts);
    
//     // 3. Non-deleted rows should remain 0
//     EXPECT_EQ(block_version.deleted_[0], 0);
//     EXPECT_EQ(block_version.deleted_[1], 0);
//     EXPECT_EQ(block_version.deleted_[3], 0);
//     EXPECT_EQ(block_version.deleted_[4], 0);
//     EXPECT_EQ(block_version.deleted_[5], 0);
//     EXPECT_EQ(block_version.deleted_[6], 0);
    
//     // 4. latest_change_ts should be updated
//     EXPECT_EQ(block_version.latest_change_ts(), restore_ts);
// }

// TEST_P(BlockVersionTest, test_restore_from_snapshot_empty_version) {
//     // Test with empty block version
//     BlockVersion block_version(8192);
    
//     TxnTimeStamp restore_ts = 100;
//     block_version.RestoreFromSnapshot(restore_ts);
    
//     // Should handle empty version gracefully
//     EXPECT_EQ(block_version.created_.size(), 0);
//     EXPECT_EQ(block_version.latest_change_ts(), restore_ts);
// }
