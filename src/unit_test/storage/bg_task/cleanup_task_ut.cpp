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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.cleanup_task;

import :ut.base_test;
import :logger;
import :infinity_context;
import :storage;
import :table_def;
import :column_vector;
import :value;
import :fileworker_manager;
import :physical_import;
import :status;
import :bg_task;
import :index_base;
import third_party;
import :base_table_ref;
import :index_secondary;
import :infinity_exception;
import :wal_manager;
import :compaction_process;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :data_block;

import column_def;
import logical_type;
import data_type;
import extra_ddl_info;

using namespace infinity;

class CleanupTaskTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CleanupTaskTest,
                         ::testing::Values(BaseTestParamStr::NEW_BG_ON_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_BG_ON_CONFIG_PATH));

TEST_P(CleanupTaskTest, SLOW_test_delete_db_simple) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *wal_manager = infinity::InfinityContext::instance().storage()->wal_manager();
    i64 cleanup_interval = InfinityContext::instance().storage()->config()->CleanupInterval();

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name1 = std::make_shared<std::string>("tb1");
    auto table_name2 = std::make_shared<std::string>("tb2");
    auto table_def1 = TableDef::Make(db_name, table_name1, std::make_shared<std::string>(), {column_def1, column_def2});
    auto table_def2 = TableDef::Make(db_name, table_name2, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def1), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def2), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
            auto status = txn->Append(*db_name, *table_name1, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
            auto status = txn->Append(*db_name, *table_name2, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        std::vector<std::string> exist_file_paths;
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table 1"), TransactionType::kRead);
            auto status = txn->GetTableFilePaths(*db_name, *table_name1, exist_file_paths);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        std::vector<std::string> delete_file_paths;
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table 2"), TransactionType::kDropTable);
            auto status = txn->GetTableFilePaths(*db_name, *table_name2, delete_file_paths);
            EXPECT_TRUE(status.ok());
            status = txn->DropTable(*db_name, *table_name2, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager->LastCheckpointTS(), false);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // Wait for the cleanup task to run
        LOG_INFO(fmt::format("cleanup_interval: {} seconds, wait for cleanup task to run", cleanup_interval));
        sleep(cleanup_interval + 1);

        CheckFilePaths(delete_file_paths, exist_file_paths);
    }
}