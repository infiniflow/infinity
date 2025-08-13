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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.checkpoint1;

import :ut.replay_test;
import :ut.base_test;
import :stl;
import :third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :infinity_exception;
import :table_def;
import :index_base;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_bmp;
import :defer_op;
import :meta_info;
import :data_block;
import :column_vector;
import :value;
import :kv_code;
import :kv_store;
import :new_txn;
import :buffer_obj;
import :buffer_handle;
import :secondary_index_in_mem;
import :secondary_index_data;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
import :catalog_meta;
import :mem_index;
import :roaring_bitmap;
import :index_filter_evaluators;
import :index_emvb;
#else
#include "gtest/gtest.h"
import infinity_core;
import replay_test;
import base_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import embedding_info;
import sparse_info;
import internal_types;
import statement_common;
import constant_expr;

using namespace infinity;

class TestTxnCheckpointTest : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();

        db_name = std::make_shared<String>("db1");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    }

protected:
    SharedPtr<String> db_name;
    SharedPtr<ColumnDef> column_def1;
    SharedPtr<ColumnDef> column_def2;
    SharedPtr<String> table_name;
    SharedPtr<TableDef> table_def;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCheckpointTest,
                         ::testing::Values(TestTxnCheckpointTest::NEW_CONFIG_NOWAL_PATH, TestTxnCheckpointTest::NEW_VFS_OFF_CONFIG_NOWAL_PATH));

TEST_P(TestTxnCheckpointTest, checkpoint_and_create_db) {
    Status status;
    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            std::sort(db_names.begin(), db_names.end());
            EXPECT_EQ(db_names, Vector<String>({*db_name, "default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());

            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(db_names, Vector<String>({"default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(db_names, Vector<String>({"default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> db_names;
            Status status = txn->ListDatabase(db_names);
            EXPECT_TRUE(status.ok());
            // Checkpoint occurs after create db is committed, kv of the db is flushed during checkpoint.
            EXPECT_EQ(db_names, Vector<String>({*db_name, "default_db"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }
}

TEST_P(TestTxnCheckpointTest, checkpoint_and_create_table) {
    SharedPtr<String> db_name = std::make_shared<String>("default_db");

    Status status;
    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> table_names;
            status = txn->ListTable(*db_name, table_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(table_names, Vector<String>({"tb1"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());

            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> table_names;
            status = txn->ListTable(*db_name, table_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(table_names, Vector<String>({}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> table_names;
            status = txn->ListTable(*db_name, table_names);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(table_names, Vector<String>({}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    UninitTxnMgr();
    CleanupDbDirs();
    InitTxnMgr();

    {
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());

            auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            new_txn_mgr->PrintAllKeyValue();

            Status status = txn2->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn2);
            EXPECT_TRUE(status.ok());
        }
        RestartTxnMgr();
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

            Vector<String> table_names;
            status = txn->ListTable(*db_name, table_names);
            EXPECT_TRUE(status.ok());
            // Checkpoint occurs after create table is committed, kv of the table is flushed during checkpoint.
            EXPECT_EQ(table_names, Vector<String>({"tb1"}));

            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }
}
