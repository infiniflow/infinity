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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.replay_restore;

import :ut.base_test;
import :ut.replay_test;

import third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :table_def;
import :data_block;
import :column_vector;
import :value;
import :new_txn;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meta;
import :default_values;
import :index_secondary;
import :index_full_text;
import :index_hnsw;
import :mem_index;
import :index_base;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import internal_types;
import statement_common;
import embedding_info;

using namespace infinity;

class TestTxnReplayRestore : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();
        db_name = std::make_shared<std::string>("default_db");

        // Create columns for different index types
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "id", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "text_col", std::set<ConstraintType>());

        // Create embedding column with proper type info (4-dimensional float vectors)
        auto embedding_type_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 4);
        column_def3 = std::make_shared<ColumnDef>(2,
                                                  std::make_shared<DataType>(LogicalType::kEmbedding, embedding_type_info),
                                                  "embedding_col",
                                                  std::set<ConstraintType>());

        column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kFloat), "float_col", std::set<ConstraintType>());

        table_name = std::make_shared<std::string>("replay_test_table");
        table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3, column_def4});

        // Create different types of indexes
        index_name1 = std::make_shared<std::string>("idx_secondary");
        index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "idx_file1.idx", {column_def1->name()});

        index_name2 = std::make_shared<std::string>("idx_fulltext");
        index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "idx_file2.idx", {column_def2->name()}, {});

        index_name3 = std::make_shared<std::string>("idx_hnsw");

        // Create HNSW index with proper parameters following the pattern from other tests
        std::vector<std::unique_ptr<InitParameter>> index_param_list;
        std::vector<InitParameter *> index_param_list_ptr;

        // Add required parameters for HNSW index
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"metric", "l2"}));
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"encode", "plain"}));

        // Convert to raw pointers for the API call
        for (auto &param : index_param_list) {
            index_param_list_ptr.push_back(param.get());
        }

        LOG_INFO("Creating HNSW index with parameters: metric=l2, encode=plain");
        LOG_INFO("HNSW params size: " + std::to_string(index_param_list_ptr.size()));

        try {
            LOG_INFO("Attempting to create HNSW index with column: " + column_def3->name());
            index_def3 = IndexHnsw::Make(index_name3, std::make_shared<std::string>(), "idx_file3.idx", {column_def3->name()}, index_param_list_ptr);
            EXPECT_TRUE(index_def3 != nullptr);
            LOG_INFO("Successfully created HNSW index: " + *index_name3);
        } catch (const std::exception &e) {
            FAIL() << "Failed to create HNSW index: " << e.what();
        }

        index_name4 = std::make_shared<std::string>("idx_secondary2");
        index_def4 = IndexSecondary::Make(index_name4, std::make_shared<std::string>(), "idx_file4.idx", {column_def4->name()});

        block_row_cnt = 8192;

        snapshot_dir = std::make_shared<std::string>(InfinityContext::instance().config()->SnapshotDir());
    }

    std::shared_ptr<DataBlock> make_input_block() {
        auto input_block = std::make_shared<DataBlock>();

        // Column 1: Integer IDs
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block->InsertVector(col1, 0);
        }

        // Column 2: Text for fulltext index
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                std::string text_value = "text_" + std::to_string(i);
                col2->AppendValue(Value::MakeVarchar(text_value));
            }
            input_block->InsertVector(col2, 1);
        }

        // Column 3: Embeddings for HNSW index
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                std::vector<f32> embedding = {static_cast<f32>(i), static_cast<f32>(i + 1), static_cast<f32>(i + 2), static_cast<f32>(i + 3)};
                col3->AppendValue(Value::MakeEmbedding(embedding));
            }
            input_block->InsertVector(col3, 2);
        }

        // Column 4: Float values for secondary index
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col4->AppendValue(Value::MakeFloat(static_cast<f32>(i) * 1.5f));
            }
            input_block->InsertVector(col4, 3);
        }

        input_block->Finalize();
        return input_block;
    }

    void PrepareTableWithIndexesAndData() {
        // Create all indexes one by one
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create secondary index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create fulltext index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create HNSW index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create secondary index 2"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def4, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // Insert data into 1 segment with 4 blocks, dump indexes after each append
        for (int block_id = 0; block_id < 4; ++block_id) {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            std::shared_ptr<DataBlock> input_block = make_input_block();
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());

            // Dump all indexes for segment 0 after each append (each block)
            auto *dump_txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem indexes block {}", block_id)),
                                                   TransactionType::kDumpMemIndex);

            Status dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem indexes block {}", block_id)),
                                             TransactionType::kDumpMemIndex);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name2, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem indexes block {}", block_id)),
                                             TransactionType::kDumpMemIndex);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name3, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("dump mem indexes block {}", block_id)),
                                             TransactionType::kDumpMemIndex);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name4, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);
        }
    }

    ~TestTxnReplayRestore() override {
        std::string cmd = fmt::format("rm -rf {}", *snapshot_dir);
        LOG_INFO(fmt::format("Exec cmd: {}", cmd));
        system(cmd.c_str());
    }

protected:
    std::shared_ptr<std::string> db_name{};
    std::shared_ptr<ColumnDef> column_def1{};
    std::shared_ptr<ColumnDef> column_def2{};
    std::shared_ptr<ColumnDef> column_def3{};
    std::shared_ptr<ColumnDef> column_def4{};
    std::shared_ptr<std::string> table_name{};
    std::shared_ptr<TableDef> table_def{};
    std::shared_ptr<std::string> index_name1{};
    std::shared_ptr<IndexBase> index_def1{};
    std::shared_ptr<std::string> index_name2{};
    std::shared_ptr<IndexBase> index_def2{};
    std::shared_ptr<std::string> index_name3{};
    std::shared_ptr<IndexBase> index_def3{};
    std::shared_ptr<std::string> index_name4{};
    std::shared_ptr<IndexBase> index_def4{};
    u32 block_row_cnt{};
    std::shared_ptr<std::string> snapshot_dir;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayRestore,
                         ::testing::Values(TestTxnReplayRestore::NEW_CONFIG_PATH, TestTxnReplayRestore::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayRestore, test_repaly_restore_table_snapshot) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    // Create table snapshot
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateTableSnapshot);
        auto status = txn->CreateTableSnapshot(*db_name, *table_name, "test001");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        auto status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("restore table"), TransactionType::kRestoreTable);

        // Deserialize the snapshot info
        std::shared_ptr<TableSnapshotInfo> table_snapshot;
        Status status;
        std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(*snapshot_dir, "test001");
        EXPECT_TRUE(status.ok());

        // Attempt to restore table
        status = txn->RestoreTableSnapshot(*db_name, table_snapshot);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        auto status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        size_t table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, 8192 * 4);
    }
}

TEST_P(TestTxnReplayRestore, test_repaly_restore_database_snapshot) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    // Create database snapshot
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateDBSnapshot);
        auto status = txn->CreateDBSnapshot(*db_name, "test002");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create database
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase("tmp_db", ConflictType::kError, std::make_shared<std::string>());
        ASSERT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        auto status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("restore database"), TransactionType::kRestoreDatabase);

        std::shared_ptr<DatabaseSnapshotInfo> database_snapshot;
        Status status;
        std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(*snapshot_dir, "test002");
        EXPECT_TRUE(status.ok());

        status = txn->RestoreDatabaseSnapshot(database_snapshot);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        auto status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        size_t table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, 8192 * 4);
    }
}

TEST_P(TestTxnReplayRestore, test_repaly_restore_system_snapshot) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    // Create database
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase("second_db", ConflictType::kError, std::make_shared<std::string>());
        ASSERT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    // Create system snapshot
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create snapshot"), TransactionType::kCreateSystemSnapshot);
        auto status = txn->CreateSystemSnapshot("test003");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create database
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase("third_db", ConflictType::kError, std::make_shared<std::string>());
        ASSERT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        ASSERT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        auto status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        auto status = txn->DropDatabase("second_db", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("restore system"), TransactionType::kRestoreSystem);

        std::shared_ptr<SystemSnapshotInfo> system_snapshot;
        Status status;
        std::tie(system_snapshot, status) = SystemSnapshotInfo::Deserialize(*snapshot_dir, "test003");
        EXPECT_TRUE(status.ok());

        status = txn->RestoreSystemSnapshot(system_snapshot);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        auto status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        size_t table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, 8192 * 4);
    }
}