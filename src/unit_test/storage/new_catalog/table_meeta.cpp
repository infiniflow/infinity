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
// import segment_meta;
// import block_meta;
// import column_meta;
import table_meeta;

using namespace infinity;

class TableMeetaTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TableMeetaTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TableMeetaTest, table_meeta) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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
    // ---------------------------

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta("0", *kv_instance);
    table_meta.Init();

    auto [segment_id, segment_status] = table_meta.GetLatestSegmentID();
    EXPECT_TRUE(segment_status.ok());
    EXPECT_EQ(segment_id, 0);

    auto [dir_ptr, dir_status] = table_meta.GetTableDir();
    EXPECT_TRUE(dir_status.ok());
    EXPECT_STREQ(dir_ptr->c_str(), "0");

    auto [segments_ptr, segments_status] = table_meta.GetSegmentIDs();
    EXPECT_TRUE(segments_status.ok());
    EXPECT_EQ(segments_ptr->size(), 0);

    auto [def_vectors, def_status] = table_meta.GetColumnDefs();
    EXPECT_TRUE(def_status.ok());
    EXPECT_EQ(def_vectors->size(), 0);

    // ---------------------------
    auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
    status = txn3->DropTable(*db_name, *table_name, ConflictType::kError);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn3);
    EXPECT_TRUE(status.ok());

    // drop database
    auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    status = txn4->DropDatabase("db1", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn4);
    EXPECT_TRUE(status.ok());
}