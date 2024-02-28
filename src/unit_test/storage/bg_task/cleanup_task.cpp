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
import compact_segments_task;

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
        auto *txn = txn_mgr->CreateTxn();
        TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, txn);
        cleanup_task->BeginTxn();
        cleanup_task->Execute();
        cleanup_task->CommitTxn();
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
        txn_mgr->RollBackTxn(txn);
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
            auto *txn = txn_mgr->CreateTxn();
            TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
            auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, txn);
            cleanup_task->BeginTxn();
            cleanup_task->Execute();
            cleanup_task->CommitTxn();
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
        auto *txn = txn_mgr->CreateTxn();
        TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, txn);
        cleanup_task->BeginTxn();
        cleanup_task->Execute();
        cleanup_task->CommitTxn();
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
        txn_mgr->RollBackTxn(txn);
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
            auto *txn = txn_mgr->CreateTxn();
            TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
            auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, txn);
            cleanup_task->BeginTxn();
            cleanup_task->Execute();
            cleanup_task->CommitTxn();
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

TEST_F(CleanupTaskTest, TestCompactAndCleanup) {
    constexpr int kImportN = 5;
    constexpr int kImportSize = 100;

    // close auto cleanup task
    auto config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_cleanup_task.toml");

    infinity::InfinityContext::instance().Init(config_path);
    Storage *storage = infinity::InfinityContext::instance().storage();
    EXPECT_NE(storage, nullptr);

    TxnManager *txn_mgr = storage->txn_manager();
    BufferManager *buffer_mgr = storage->buffer_manager();
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
        auto [table_entry, status] = txn->GetTableEntry(*db_name, *table_name);
        EXPECT_TRUE(table_entry != nullptr);
        table_entry->SetCompactionAlg(nullptr);

        for (int i = 0; i < kImportN; ++i) {
            Vector<SharedPtr<ColumnVector>> column_vectors;
            {
                SharedPtr<ColumnVector> column_vector = ColumnVector::Make(MakeShared<DataType>(column_defs[0]->type()->type()));
                column_vector->Initialize();
                Value v = Value::MakeInt(i);
                for (int i = 0; i < kImportSize; ++i) {
                    column_vector->AppendValue(v);
                }
                column_vectors.push_back(column_vector);
            }
            SizeT column_count = column_vectors.size();

            SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
            auto segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn, false);
            auto block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, column_count, txn);
            int row_count = -1;
            for (SizeT i = 0; i < column_count; ++i) {
                auto *column_vector = column_vectors[i].get();
                auto column_block_entry = block_entry->GetColumnBlockEntry(i);
                if (row_count == -1) {
                    row_count = column_vector->Size();
                } else {
                    EXPECT_EQ(row_count, column_vector->Size());
                }
                column_block_entry->Append(column_vector, 0, row_count, buffer_mgr);
            }
            block_entry->IncreaseRowCount(row_count);
            segment_entry->AppendBlockEntry(std::move(block_entry));

            auto *txn_store = txn->GetTxnTableStore(table_entry);
            PhysicalImport::SaveSegmentData(txn_store, segment_entry);
            segment_entry->SetSealed();
        }
        txn_mgr->CommitTxn(txn);
    }
    {
        auto txn = txn_mgr->CreateTxn();
        txn->Begin();

        auto [table_entry, status] = txn->GetTableEntry(*db_name, *table_name);
        EXPECT_TRUE(table_entry != nullptr);

        {
            auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry, txn);
            compact_task->Execute();
        }
        txn_mgr->CommitTxn(txn);
    }

    {
        auto *txn = txn_mgr->CreateTxn();
        TxnTimeStamp visible_ts = txn_mgr->GetMinUncommitTs();
        auto cleanup_task = MakeShared<CleanupTask>(catalog, visible_ts, txn);
        cleanup_task->BeginTxn();
        cleanup_task->Execute();
        cleanup_task->CommitTxn();
    }

    infinity::InfinityContext::instance().UnInit();
}
