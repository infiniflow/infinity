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

#include <filesystem>

import base_test;
import stl;
import third_party;
import status;
import new_catalog;
import new_txn_manager;
import infinity_context;
import txn_state;
import extra_ddl_info;
import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import wal_manager;
import constant_expr;

using namespace infinity;

class TestTxnCleanupInternal : public BaseTestParamStr {
public:
    void SetUp() override {
        BaseTestParamStr::SetUp();

        new_txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void TearDown() override {
        new_txn_mgr_->PrintAllKeyValue();

        SizeT kv_num = new_txn_mgr_->KeyValueNum();
        EXPECT_EQ(kv_num, 3);

        new_txn_mgr_ = nullptr;
        BaseTestParamStr::TearDown();
    }

    void Init() {
        auto config_path = std::make_shared<std::string>(std::filesystem::absolute(GetParam()));
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        new_txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void UnInit() {
        new_txn_mgr_->PrintAllKeyValue();
        new_txn_mgr_ = nullptr;

        infinity::InfinityContext::instance().UnInit();
    }

    void Cleanup() {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void Checkpoint() {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckFilePaths() {
        auto *pm = infinity::InfinityContext::instance().persistence_manager();
        if (pm == nullptr) {
            Path data_dir = this->GetFullDataDir();
            for (auto &file_path : delete_file_paths_) {
                file_path = data_dir / file_path;
            }
            for (auto &file_path : exist_file_paths_) {
                file_path = data_dir / file_path;
            }
            for (const auto &file_path : delete_file_paths_) {
                if (!std::filesystem::path(file_path).is_absolute()) {
                    ADD_FAILURE() << "File path is not absolute: " << file_path;
                }
                EXPECT_FALSE(std::filesystem::exists(file_path));

                auto path = static_cast<Path>(file_path).parent_path();
                EXPECT_TRUE(!std::filesystem::exists(path) || std::filesystem::is_directory(path) && !std::filesystem::is_empty(path) ||
                            std::filesystem::is_directory(path) && std::filesystem::is_empty(path) && path == data_dir);
            }
            for (const auto &file_path : exist_file_paths_) {
                if (!std::filesystem::path(file_path).is_absolute()) {
                    ADD_FAILURE() << "File path is not absolute: " << file_path;
                }
                EXPECT_TRUE(std::filesystem::exists(file_path));
            }
        } else {
            auto local_path_map = pm->GetAllFiles();
            for (const auto &file_path : delete_file_paths_) {
                auto persist_read_result = local_path_map.find(file_path);
                EXPECT_TRUE(persist_read_result == local_path_map.end());
            }
            for (const auto &file_path : exist_file_paths_) {
                auto persist_read_result = local_path_map.find(file_path);
                EXPECT_FALSE(persist_read_result == local_path_map.end());
            }
        }

        delete_file_paths_.clear();
        exist_file_paths_.clear();
    }

protected:
    NewTxnManager *new_txn_mgr_;
    WalManager *wal_manager_;
    Vector<String> delete_file_paths_;
    Vector<String> exist_file_paths_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCleanupInternal,
                         ::testing::Values(BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH, BaseTestParamStr::NEW_CONFIG_PATH));

TEST_P(TestTxnCleanupInternal, test_cleanup_db) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    SizeT block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
            col->Initialize();
            for (SizeT i = 0; i < block_row_cnt; ++i) {
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
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Status status =
                txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

            Status status = txn->GetDBFilePaths(*db_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        new_txn_mgr_->PrintAllKeyValue();
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();
    }
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Status status =
                txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

            Status status = txn->GetDBFilePaths(*db_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kIgnore, MakeShared<String>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Status status =
                txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());

            status = txn->GetTableFilePaths(*db_name, *table_name, exist_file_paths_);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

            Status status = txn->DropDatabase(*db_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();
    }
}

TEST_P(TestTxnCleanupInternal, test_cleanup_table) {
    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    SizeT block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
            col->Initialize();
            for (SizeT i = 0; i < block_row_cnt; ++i) {
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
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();
    }
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());

            status = txn->GetTableFilePaths(*db_name, *table_name, exist_file_paths_);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();
    }
}

TEST_P(TestTxnCleanupInternal, test_cleanup_index) {
    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    SizeT block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
            col->Initialize();
            for (SizeT i = 0; i < block_row_cnt; ++i) {
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
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        create_index(index_def1);
        create_index(index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
            Status status = txn->GetTableIndexFilePaths(*db_name, *table_name, *index_name1, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

            Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
            EXPECT_TRUE(status.ok());

            status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();
    }
    {
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        create_index(index_def1);
        create_index(index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
            Status status = txn->GetTableIndexFilePaths(*db_name, *table_name, *index_name1, delete_file_paths_);
            EXPECT_TRUE(status.ok());
            status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        create_index(index_def1);
        create_index(index_def2);

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
            Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
            EXPECT_TRUE(status.ok());

            status = txn->GetTableFilePaths(*db_name, *table_name, exist_file_paths_);
            EXPECT_TRUE(status.ok());

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        Checkpoint();
        Cleanup();

        this->CheckFilePaths();

        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

            Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
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
    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    [[maybe_unused]] auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    SizeT block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
            col->Initialize();
            for (SizeT i = 0; i < block_row_cnt; ++i) {
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
    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Status status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    new_txn_mgr_->PrintAllKeyValue();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status;

        status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr_->PrintAllKeyValue();
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr_->PrintAllKeyValue();
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
}

TEST_P(TestTxnCleanupInternal, test_cleanup_optimize) {
    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    [[maybe_unused]] auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    SizeT block_row_cnt = 8192;
    SegmentID segment_id = 0;

    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto make_column = [&](const Value &v) {
            auto col = ColumnVector::Make(MakeShared<DataType>(v.type()));
            col->Initialize();
            for (SizeT i = 0; i < block_row_cnt; ++i) {
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
    auto dump_index = [&](const String &index_name) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    for (SizeT i = 0; i < 2; ++i) {
        {
            auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        dump_index(*index_name1);
        dump_index(*index_name2);
    };

    auto merge_index = [&](SegmentID segment_id, const String &index_name) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("merge index {}", index_name)), TransactionType::kNormal);

        {
            Status status = txn->GetSegmentIndexFilepaths(*db_name, *table_name, index_name, segment_id, delete_file_paths_);
            EXPECT_TRUE(status.ok());
        }

        Status status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    merge_index(segment_id, *index_name1);
    merge_index(segment_id, *index_name2);
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
}

TEST_P(TestTxnCleanupInternal, test_cleanup_drop_column) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
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

    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Status status = txn->GetColumnFilePaths(*db_name, *table_name, column_def2->name_, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names = {column_def2->name_};
        status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    Checkpoint();
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

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
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
}

TEST_P(TestTxnCleanupInternal, test_cleanup_drop_index_and_checkpoint_and_restart) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "my_file_name", {column_def1->name()});

    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
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

    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    create_index(index_def1);

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Status status = txn->GetColumnFilePaths(*db_name, *table_name, column_def2->name_, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names = {column_def2->name_};
        status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    UnInit();

    Init();
    Checkpoint();
    Cleanup();

    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, delete_file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);

        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    UnInit();

    Init();

    new_txn_mgr_->PrintAllKeyValue();
    Checkpoint();
    Cleanup();

    this->CheckFilePaths();
}

TEST_P(TestTxnCleanupInternal, test_import_with_index_rollback_cleanup) {

    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
            for (u32 i = 0; i < block_row_cnt; i += 2) {
                col.AppendValue(v1);
                col.AppendValue(v2);
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    {
        auto *txn_import = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
        Status status = txn_import->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn_drop_db = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn_drop_db->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn_drop_db);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn_import);
        EXPECT_FALSE(status.ok());
    }
    Checkpoint();
    Cleanup();

    this->CheckFilePaths();
}