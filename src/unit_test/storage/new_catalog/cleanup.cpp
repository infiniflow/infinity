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
import data_access_state;
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
import constant_expr;

using namespace infinity;

class TestCleanup : public BaseTestParamStr {
protected:
    Status GetTableFilePaths(NewTxn *txn, const String &db_name, const String &table_name, Vector<String> &file_paths) {
        Status status = NewCatalog::GetTableFilePaths(txn, db_name, table_name, file_paths);
        if (!status.ok()) {
            return status;
        }
        for (auto &file_path : file_paths) {
            file_path = String(this->GetFullDataDir()) + "/" + file_path;
        }
        return Status::OK();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TestCleanup, ::testing::Values(BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TestCleanup, test_cleanup_table) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Vector<String> file_paths;
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
        CheckpointOption option;
        Status status = txn->Checkpoint(option);
        EXPECT_TRUE(status.ok());

        status = this->GetTableFilePaths(txn, *db_name, *table_name, file_paths);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
        EXPECT_TRUE(status.ok());
    }

    for (const auto &file_path : file_paths) {
        EXPECT_FALSE(std::filesystem::exists(file_path));
    }
}
