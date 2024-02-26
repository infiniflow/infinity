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

import stl;
import infinity_context;
import storage;
import column_def;
import logical_type;
import data_type;
import txn_manager;
import table_def;
import extra_ddl_info;
import column_vector;
import value;
import catalog;
import segment_entry;
import block_entry;
import buffer_manager;
import physical_import;
import status;
import compilation_config;
import cleanup_task;

using namespace infinity;

class CleanupTaskTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override { system("tree /tmp/infinity"); }
};

TEST_F(CleanupTaskTest, TestDeleteDB_Simple) {
    // close auto cleanup task
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

    infinity::InfinityContext::instance().Init(config_path);
    Storage *storage = infinity::InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    TxnManager *txn_mgr = storage->txn_manager();
    Catalog *catalog = storage->catalog();

    auto db_name = MakeShared<String>("db1");
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        txn->CreateDatabase(*db_name, ConflictType::kError);
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts);
        cleanup_task->Execute();
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [db_entry, status] = txn->GetDatabase(*db_name);
        EXPECT_EQ(db_entry, nullptr);
        txn_mgr->CommitTxn(txn);
    }
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(CleanupTaskTest, TestDeleteDB_Complex) {
    // close auto cleanup task
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

    infinity::InfinityContext::instance().Init(config_path);
    Storage *storage = infinity::InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    TxnManager *txn_mgr = storage->txn_manager();
    Catalog *catalog = storage->catalog();

    auto db_name = MakeShared<String>("db1");
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        txn->CreateDatabase(*db_name, ConflictType::kError);
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        txn->CreateDatabase(*db_name, ConflictType::kError);
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        {
            TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
            auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts);
            cleanup_task->Execute();
        }
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [db_entry, status] = txn->GetDatabase(*db_name);
        EXPECT_EQ(db_entry, nullptr);
        txn_mgr->CommitTxn(txn);
    }
    infinity::InfinityContext::instance().UnInit();
}

TEST_F(CleanupTaskTest, TestDeleteTable_Simple) {
    // close auto cleanup task
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

    infinity::InfinityContext::instance().Init(config_path);
    Storage *storage = infinity::InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    TxnManager *txn_mgr = storage->txn_manager();
    Catalog *catalog = storage->catalog();

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        HashSet<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints));
    }
    auto db_name = MakeShared<String>("default");
    auto table_name = MakeShared<String>("table1");
    {

        auto table_def = MakeUnique<TableDef>(db_name, table_name, column_defs);
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->DropTableCollectionByName(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }

    {
        TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts);
        cleanup_task->Execute();
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [table_entry, status] = txn->GetTableEntry(*db_name, *table_name);
        EXPECT_EQ(table_entry, nullptr);

        txn_mgr->CommitTxn(txn);
    }

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(CleanupTaskTest, TestDeleteTable_Complex) {
    // close auto cleanup task
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

    infinity::InfinityContext::instance().Init(config_path);
    Storage *storage = infinity::InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    TxnManager *txn_mgr = storage->txn_manager();
    Catalog *catalog = storage->catalog();

    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        HashSet<ConstraintType> constraints;
        ColumnID column_id = 0;
        column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), "col1", constraints));
    }
    auto db_name = MakeShared<String>("default");
    auto table_name = MakeShared<String>("table1");
    {

        auto table_def = MakeUnique<TableDef>(db_name, table_name, column_defs);
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->DropTableCollectionByName(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        txn_mgr->CommitTxn(txn);
    }
    {
        auto table_def = MakeUnique<TableDef>(db_name, table_name, column_defs);
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    }

    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();

        Status status = txn->DropTableCollectionByName(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        {
            TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
            auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts);
            cleanup_task->Execute();
        }
        txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->CreateTxn();
        txn->Begin();
        auto [table_entry, status] = txn->GetTableEntry(*db_name, *table_name);
        EXPECT_EQ(table_entry, nullptr);

        txn_mgr->CommitTxn(txn);
    }

    infinity::InfinityContext::instance().UnInit();
}
