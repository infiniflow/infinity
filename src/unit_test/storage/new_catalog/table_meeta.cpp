// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
import stl;
import third_party;
import kv_store;
import status;
import new_txn_manager;
import new_txn;
import infinity_context;
import column_def;
import data_type;
import logical_type;
import table_def;
import txn_state;
import extra_ddl_info;
import segment_meta;
// import block_meta;
// import column_meta;
import table_meeta;

using namespace infinity;

class TestTxnTableMeeta : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnTableMeeta,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnTableMeeta, table_meeta) {
    using namespace infinity;

    Status status;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        // create db1
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // create table
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
    }
    // ---------------------------

    // get table
    auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
    auto [table_info, get_status] = txn2->GetTableInfo(*db_name, *table_name);
    EXPECT_TRUE(get_status.ok());

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta(table_info->db_id_, table_info->table_id_, *kv_instance, txn2->BeginTS());

    {
        // SegmentID segment_id = 0;
        // auto segment_status = table_meta.GetNextSegmentID(segment_id);
        // EXPECT_TRUE(segment_status.ok());
        // EXPECT_EQ(segment_id, 0);
        {
            SegmentMeta segment_meta(0, table_meta);
            segment_meta.Init();
            // segment_meta.SetRowCnt(1048);
            {
                auto [blocks, block_status] = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(block_status.ok());
                EXPECT_EQ(blocks->size(), 0);
            }
            {
                TxnTimeStamp commit_ts = 1;

                BlockID block_id = 0;
                std::tie(block_id, status) = segment_meta.AddBlockID1(commit_ts);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_id, 0);

                std::tie(block_id, status) = segment_meta.AddBlockID1(commit_ts);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_id, 1);
            }

            {
                auto [blocks, block_status] = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(block_status.ok());
                EXPECT_EQ(*blocks, std::vector<BlockID>({0, 1}));
            }

            // {
            //     auto [row_count, block_status] = segment_meta.GetRowCnt();
            //     EXPECT_EQ(row_count, 1048);
            // }
        }
    }

    // {
    //     Status status = table_meta.SetNextSegmentID(1);
    //     EXPECT_TRUE(status.ok());

    //     status = table_meta.AddSegmentID(1);
    //     EXPECT_TRUE(status.ok());

    //     status = table_meta.SetNextSegmentID(2);
    //     EXPECT_TRUE(status.ok());

    //     status = table_meta.AddSegmentID(2);
    //     EXPECT_TRUE(status.ok());

    //     SegmentID segment_id = 0;
    //     auto segment_status = table_meta.GetNextSegmentID(segment_id);
    //     EXPECT_TRUE(segment_status.ok());
    //     EXPECT_EQ(segment_id, 2);
    // }

    // {
    //     auto dir_ptr = table_meta.GetTableDir();
    //     EXPECT_STREQ(dir_ptr->c_str(), "1");
    // }

    // {
    //     auto [segments_ptr, segments_status] = table_meta.GetSegmentIDs1();
    //     EXPECT_TRUE(segments_status.ok());
    //     EXPECT_EQ(segments_ptr->size(), 3);
    //     EXPECT_EQ(segments_ptr->at(0), 0);
    //     EXPECT_EQ(segments_ptr->at(1), 1);
    //     EXPECT_EQ(segments_ptr->at(2), 2);
    // }

    {
        auto [def_vectors, def_status] = table_meta.GetColumnDefs();
        EXPECT_TRUE(def_status.ok());
        EXPECT_EQ(def_vectors->size(), 2);
    }

    {
        auto [column_id1, column_status1] = table_meta.GetColumnIDByColumnName("col1");
        EXPECT_TRUE(column_status1.ok());
        EXPECT_EQ(column_id1, 0);

        auto [column_id2, column_status2] = table_meta.GetColumnIDByColumnName("col2");
        EXPECT_TRUE(column_status2.ok());
        EXPECT_EQ(column_id2, 1);
    }

    kv_instance->Commit();
    kv_instance.reset();

    get_status = new_txn_mgr->CommitTxn(txn2);
    EXPECT_TRUE(get_status.ok());

    {
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop database
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }
}