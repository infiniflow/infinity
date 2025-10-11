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

module infinity_core:ut.new_txn_replay;

import :ut.base_test;
import :ut.replay_test;

import third_party;
import :status;
import :new_txn_manager;
import :new_txn;
import :table_def;
import :index_base;
import :infinity_context;
import :txn_state;
import :data_block;
import :column_vector;
import :value;
import :new_catalog;
import :db_meta;
import :table_meta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :catalog_meta;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_emvb;
import :index_bmp;
import :defer_op;
import :mem_index;
import :buffer_obj;
import :buffer_handle;
import :logger;

import column_def;
import data_type;
import logical_type;
import extra_ddl_info;
import internal_types;
import embedding_info;
import sparse_info;
import statement_common;
import constant_expr;

using namespace infinity;

class TestTxnReplayTest : public NewReplayTest {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayTest,
                         ::testing::Values(TestTxnReplayTest::NEW_CONFIG_PATH, TestTxnReplayTest::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayTest, test_replay_create_db) {
    using namespace infinity;

    {
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"db1", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayTest, test_replay_create_db2) {
    using namespace infinity;

    {
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"db1", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase("db2", ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"db1", "db2", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayTest, test_replay_create_table) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        // check table
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);

        std::vector<std::string> table_names;
        Status status = txn->ListTable(*db_name, table_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(table_names, std::vector<std::string>{*table_name});

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayTest, test_replay_drop_table) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        // check table
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);
        auto [table_info, status] = txn->GetTableInfo(*db_name, *table_name);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_create) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto default_value1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        {
            int64_t num = 1;
            default_value1->integer_value_ = num;
        }

        auto column_def22 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kInteger), "col3", std::set<ConstraintType>(), "", default_value1);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def22);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_drop) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();

    {
        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop columns"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn->DropIndexByName(*db_name, *table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_drop_column_add_column) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col1"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        auto default_value1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        {
            int64_t num = 1;
            default_value1->integer_value_ = num;
        }
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>(), "", default_value1);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_compact) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, std::vector<SegmentID>({0}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_delete_compact) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, std::vector<SegmentID>({0}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_rename) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_dump_optimize_index) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize indexes"), TransactionType::kOptimizeIndex);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_append_append) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    append();
    append();

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    LOG_INFO("Checking KVs before restart.");
    new_txn_mgr->PrintAllKeyValue();

    RestartTxnMgr();

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];

        SegmentMeta segment_meta(segment_id, *table_meta);

        size_t segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 8192 * 2);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0, 1}));

        size_t block_row_cnt = 0;
        for (const auto &block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            EXPECT_EQ(block_row_cnt, 8192);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_dump_index) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    append();

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, "index_name", db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }

        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({0, 1}));

            for (const auto &chunk_id : *chunk_ids) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                ChunkIndexMetaInfo *chunk_info = nullptr;
                status = chunk_index_meta.GetChunkInfo(chunk_info);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt);
                EXPECT_EQ(chunk_info->base_row_id_, RowID(0, chunk_id * block_row_cnt));

                BufferObj *buffer_obj = nullptr;
                status = chunk_index_meta.GetIndexBuffer(buffer_obj);
                EXPECT_TRUE(status.ok());
            }
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after restart.");
    new_txn_mgr->PrintAllKeyValue();
}

TEST_P(TestTxnReplayTest, test_replay_flush_gap_optimize_index) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(std::make_shared<DataType>(v.type()));
            col->Initialize();
            for (size_t i = 0; i < block_row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        {
            auto col1 = make_column(v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    append();
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize indexes"), TransactionType::kOptimizeIndex);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, "index_name", db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }

        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({2}));

            ChunkID chunk_id = 2;
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            ChunkIndexMetaInfo *chunk_info = nullptr;
            status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt * 2);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));

            BufferObj *buffer_obj = nullptr;
            status = chunk_index_meta.GetIndexBuffer(buffer_obj);
            EXPECT_TRUE(status.ok());
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    LOG_INFO("Checking KVs after cleanup.");
    new_txn_mgr->PrintAllKeyValue();
}
