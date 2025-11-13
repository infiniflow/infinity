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

module infinity_core:ut.cleanup_internal;

import :ut.base_test;
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
import :new_txn_store;
import :secondary_index_in_mem;
import :secondary_index_data;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meta;
import :catalog_meta;
import :mem_index;
import :roaring_bitmap;
import :index_filter_evaluators;
import :index_emvb;
import :wal_manager;

import third_party;

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

class TestTxnCleanupInternal : public BaseTestParamStr {
public:
    void SetUp() override {
        BaseTestParamStr::SetUp();

        new_txn_mgr_ = InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = InfinityContext::instance().storage()->wal_manager();
    }

    void TearDown() override {
        new_txn_mgr_->PrintAllKeyValue();

        size_t kv_num = new_txn_mgr_->KeyValueNum();
        EXPECT_EQ(kv_num, 3);

        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    void Init() {
        auto config_path = std::make_shared<std::string>(std::filesystem::absolute(GetParam()));
        InfinityContext::instance().InitPhase1(config_path);
        InfinityContext::instance().InitPhase2();

        new_txn_mgr_ = InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = InfinityContext::instance().storage()->wal_manager();
    }

    void UnInit() {
        new_txn_mgr_->PrintAllKeyValue();
        new_txn_mgr_ = nullptr;

        InfinityContext::instance().UnInit();
    }

    void Cleanup() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void Checkpoint() {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void create_index(const std::string &db_name, const std::string &table_name, const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                           TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(db_name, table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

protected:
    NewTxnManager *new_txn_mgr_{};
    WalManager *wal_manager_{};
    std::vector<std::string> delete_file_paths_;
    std::vector<std::string> exist_file_paths_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCleanupInternal,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCleanupInternal, test_cleanup_db) {
    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
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
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            auto status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
            auto status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

            auto status = txn->GetDBFilePaths(*db_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        Checkpoint();
        Cleanup();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);
    }
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            auto status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
            auto status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

            auto status = txn->GetDBFilePaths(*db_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
            auto status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
            EXPECT_TRUE(status.ok());

            status = txn->GetTableFilePaths(*db_name, *table_name, exist_file_paths_);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        std::println("1");
        new_txn_mgr_->PrintAllKeyValue();
        Cleanup();

        fmt::print("------\n");
        new_txn_mgr_->PrintAllKeyValue();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

            auto status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();
    }
}

TEST_P(TestTxnCleanupInternal, test_cleanup_table) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

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
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        new_txn_mgr_->PrintAllKeyValue();
        Checkpoint();
        new_txn_mgr_->PrintAllKeyValue();
        // new_txn_mgr_->PrintAllKeyValue();
        Cleanup();
        // new_txn_mgr_->PrintAllKeyValue();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
            Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        new_txn_mgr_->PrintAllKeyValue();
        Cleanup();

        new_txn_mgr_->PrintAllKeyValue();
    }
}

TEST_P(TestTxnCleanupInternal, test_cleanup_index) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});

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
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        create_index(*db_name, *table_name, index_def1);
        create_index(*db_name, *table_name, index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
            auto status = txn->GetTableIndexFilePaths(*db_name, *table_name, *index_name1, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

            auto status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);
    }
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        create_index(*db_name, *table_name, index_def1);
        create_index(*db_name, *table_name, index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
            auto status = txn->GetTableIndexFilePaths(*db_name, *table_name, *index_name1, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // create_index(*db_name, *table_name, index_def1);
        create_index(*db_name, *table_name, index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
            auto status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
            EXPECT_TRUE(status.ok());

            status = txn->GetTableFilePaths(*db_name, *table_name, exist_file_paths_);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        CheckFilePaths(delete_file_paths_, exist_file_paths_);

        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

            auto status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();
    }
}

TEST_P(TestTxnCleanupInternal, test_cleanup_compact) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    // auto index_name2 = std::make_shared<std::string>("index2");
    // auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
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
    for (size_t i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        auto status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
        std::println("{}", ++cnt);
        new_txn_mgr_->PrintAllKeyValue();
    };
    // new_txn_mgr_->PrintAllKeyValue();
    // {
    //     auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
    //     Status status;
    //
    //     status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
    //     EXPECT_TRUE(status.ok());
    //
    //     status = txn->Compact(*db_name, *table_name, {0, 1});
    //     EXPECT_TRUE(status.ok());
    //     status = new_txn_mgr_->CommitTxn(txn);
    //     EXPECT_TRUE(status.ok());
    //     std::println("{}", ++cnt);
    //     new_txn_mgr_->PrintAllKeyValue();
    // }

    Checkpoint();
    std::println("{}", ++cnt);
    new_txn_mgr_->PrintAllKeyValue();
    Cleanup();

    std::println("{}", ++cnt);
    new_txn_mgr_->PrintAllKeyValue();

    CheckFilePaths(delete_file_paths_, exist_file_paths_);

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        auto status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
        std::println("{}", ++cnt);
        new_txn_mgr_->PrintAllKeyValue();
    }
    Checkpoint();
    Cleanup();
    std::println("{}", ++cnt);
    new_txn_mgr_->PrintAllKeyValue();
    CheckFilePaths(delete_file_paths_, exist_file_paths_);
}

TEST_P(TestTxnCleanupInternal, test_cleanup_optimize) {
    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    create_index(*db_name, *table_name, index_def1);
    create_index(*db_name, *table_name, index_def2);

    size_t block_row_cnt = 8192;
    SegmentID segment_id = 0;

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
    auto dump_index = [&](const std::string &index_name) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        auto status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    for (size_t i = 0; i < 2; ++i) {
        {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        dump_index(*index_name1);
        dump_index(*index_name2);
    };

    auto merge_index = [&](SegmentID segment_id, const std::string &index_name) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("merge index {}", index_name)), TransactionType::kOptimizeIndex);

        {
            auto status = txn->GetSegmentIndexFilepaths(*db_name, *table_name, index_name, segment_id, delete_file_paths_);
            EXPECT_TRUE(status.ok());
        }

        auto status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    merge_index(segment_id, *index_name1);
    merge_index(segment_id, *index_name2);
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    CheckFilePaths(delete_file_paths_, exist_file_paths_);

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    CheckFilePaths(delete_file_paths_, exist_file_paths_);
}

TEST_P(TestTxnCleanupInternal, test_cleanup_drop_column) {
    using namespace infinity;

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    for (size_t i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        Status status = txn->GetColumnFilePaths(*db_name, *table_name, column_def2->name_, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names = {column_def2->name_};
        status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    CheckFilePaths(delete_file_paths_, exist_file_paths_);

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr_->PrintAllKeyValue();
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    CheckFilePaths(delete_file_paths_, exist_file_paths_);
}

TEST_P(TestTxnCleanupInternal, test_cleanup_drop_index_and_checkpoint_and_restart) {
    // move it into restart test
    using namespace infinity;

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "my_file_name", {column_def1->name()});

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    for (size_t i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    create_index(*db_name, *table_name, index_def1);

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        Status status = txn->GetColumnFilePaths(*db_name, *table_name, column_def2->name_, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names = {column_def2->name_};
        status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    UnInit();

    Init();
    Checkpoint();
    Cleanup();

    CheckFilePaths(delete_file_paths_, exist_file_paths_);

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    UnInit();

    Init();

    new_txn_mgr_->PrintAllKeyValue();
    Checkpoint();
    Cleanup();

    CheckFilePaths(delete_file_paths_, exist_file_paths_);
}
