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

import stl;
import logger;
import infinity_context;
import storage;
import column_def;
import logical_type;
import data_type;
import table_def;
import extra_ddl_info;
import column_vector;
import value;
import buffer_manager;
import physical_import;
import status;
import compilation_config;
import bg_task;
import index_base;
import third_party;
import base_table_ref;
import index_secondary;
import infinity_exception;
import wal_manager;
import compaction_process;
import txn_state;
import new_txn_manager;
import new_txn;
import data_block;

using namespace infinity;

class CleanupTaskTest : public BaseTestParamStr {
protected:
    void WaitCleanup(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        Status status = new_txn_mgr->Cleanup();
        EXPECT_TRUE(status.ok());
    }

    void WaitCheckpoint(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CleanupTaskTest,
                         ::testing::Values((std::string(test_data_path()) + "/config/test_close_bgtask.toml").c_str(),
                                           (std::string(test_data_path()) + "/config/test_close_bgtask_vfs_off.toml").c_str()));

TEST_P(CleanupTaskTest, test_delete_db_simple) {
    // close auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = MakeShared<String>("db1");
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);
        txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        txn_mgr->CommitTxn(txn);
    }
    WaitCheckpoint(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    WaitCleanup(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kRead);
        auto [db_info, status] = txn->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
}

TEST_P(CleanupTaskTest, test_delete_db_complex) {
    // close auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = MakeShared<String>("db1");
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);
        txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);
        txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        txn_mgr->RollBackTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"), TransactionType::kNormal);
        txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop db1"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        WaitCleanup(storage);
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get db1"), TransactionType::kRead);
        auto [db_info, status] = txn->GetDatabaseInfo(*db_name);
        EXPECT_FALSE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
}

TEST_P(CleanupTaskTest, test_delete_table_simple) {
    // close auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints));
    }
    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    {

        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    WaitCheckpoint(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table1"), TransactionType::kNormal);

        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }

    WaitCleanup(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get table1"), TransactionType::kRead);
        auto [table_info, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
}

TEST_P(CleanupTaskTest, test_delete_table_complex) {
    // close auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints));
    }
    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    {

        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table1"), TransactionType::kNormal);

        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->RollBackTxn(txn);
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table1"), TransactionType::kNormal);

        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        WaitCleanup(storage);
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("get table1"), TransactionType::kRead);
        auto [table_info, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
}

TEST_P(CleanupTaskTest, test_compact_and_cleanup) {
    constexpr int kImportN = 5;

    // disable auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints));
    }
    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    {

        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        u32 block_row_cnt = 8192;
        auto make_input_block = [&] {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, Value v1) {
                for (u32 i = 0; i < block_row_cnt; ++i) {
                    col.AppendValue(v1);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kInteger)));
                col1->Initialize();
                append_to_col(*col1, Value::MakeInt(2));
                input_block->InsertVector(col1, 0);
            }
            input_block->Finalize();
            return input_block;
        };

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < kImportN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1, 2, 3, 4});
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    WaitCleanup(storage);
}

TEST_P(CleanupTaskTest, test_with_index_compact_and_cleanup) {
    constexpr int kImportN = 5;

    // close auto cleanup task
    Storage *storage = InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = MakeShared<String>("default_db");
    auto table_name = MakeShared<String>("table1");
    auto index_name = MakeShared<String>("idx1");
    auto column_name = MakeShared<String>("col1");

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        std::set<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), *column_name, constraints));
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table1"), TransactionType::kNormal);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        u32 block_row_cnt = 8192;
        auto make_input_block = [&] {
            auto input_block = MakeShared<DataBlock>();
            auto append_to_col = [&](ColumnVector &col, Value v1) {
                for (u32 i = 0; i < block_row_cnt; ++i) {
                    col.AppendValue(v1);
                }
            };
            // Initialize input block
            {
                auto col1 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kInteger)));
                col1->Initialize();
                append_to_col(*col1, Value::MakeInt(2));
                input_block->InsertVector(col1, 0);
            }
            input_block->Finalize();
            return input_block;
        };

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < kImportN; ++i) {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }
    {
        SharedPtr<IndexBase> index_base =
            IndexSecondary::Make(index_name, MakeShared<String>("test comment"), fmt::format("{}_{}", *table_name, *index_name), {*column_name});

        auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1, 2, 3, 4});
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    WaitCleanup(storage);
}
