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
import wal_manager;
import constant_expr;

using namespace infinity;

class TestTxnCleanup : public BaseTestParamStr {
public:
    // void SetUp() override { BaseTestParamStr::SetUp(); }

    // void TearDown() override {}

    void Init() {
        auto config_path = std::make_shared<std::string>(std::filesystem::absolute(GetParam()));
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();

        new_txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void UnInit() {
        new_txn_mgr_->PrintAllKeyValue();

        // SizeT kv_num = new_txn_mgr_->KeyValueNum();
        // EXPECT_EQ(kv_num, 5);

        new_txn_mgr_ = nullptr;

        infinity::InfinityContext::instance().UnInit();
    }

    void CheckFilePaths() {
        for (auto &file_path : file_paths_) {
            file_path = String(this->GetFullDataDir()) + "/" + file_path;
        }

        for (const auto &file_path : file_paths_) {
            if (!std::filesystem::path(file_path).is_absolute()) {
                ADD_FAILURE() << "File path is not absolute: " << file_path;
            }
            EXPECT_FALSE(std::filesystem::exists(file_path));
        }

        file_paths_.clear();
    }

protected:
    NewTxnManager *new_txn_mgr_;
    WalManager *wal_manager_;
    Vector<String> file_paths_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, TestTxnCleanup, ::testing::Values(BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCleanup, test_cleanup_db) {
    Init();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Status status = txn->Import(*db_name, *table_name, {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Status status = txn->GetDBFilePaths(*db_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    sleep(1); // Fix can't clean up issue
    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }

    this->CheckFilePaths();
    UnInit();
}

TEST_P(TestTxnCleanup, test_cleanup_table) {
    Init();
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

        status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }

    this->CheckFilePaths();
    UnInit();
}

TEST_P(TestTxnCleanup, test_cleanup_index) {
    Init();
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
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnoprstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    create_index(index_def1);
    create_index(index_def2);

    new_txn_mgr_->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());

        status = txn->GetTableIndexFilePaths(*db_name, *table_name, *index_name1, file_paths_);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
    UnInit();
}

TEST_P(TestTxnCleanup, test_cleanup_compact) {
    Init();
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

        status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr_->PrintAllKeyValue();
    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    new_txn_mgr_->PrintAllKeyValue();
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
    UnInit();
}

TEST_P(TestTxnCleanup, test_cleanup_optimize) {
    Init();
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
            Status status = txn->GetSegmentIndexFilepaths(*db_name, *table_name, index_name, segment_id, file_paths_);
            EXPECT_TRUE(status.ok());
        }

        Status status = txn->OptimizeIndex(*db_name, *table_name, index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    merge_index(segment_id, *index_name1);
    merge_index(segment_id, *index_name2);

    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
    UnInit();
}

TEST_P(TestTxnCleanup, test_cleanup_drop_column) {
    Init();
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
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        Status status = txn->GetColumnFilePaths(*db_name, *table_name, column_def2->name_, file_paths_);
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

    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();

    {
        auto *txn = new_txn_mgr_->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        Status status = txn->GetTableFilePaths(*db_name, *table_name, file_paths_);
        EXPECT_TRUE(status.ok());

        status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    UnInit();

    Init();

    new_txn_mgr_->PrintAllKeyValue();

    {
        Status status = new_txn_mgr_->Cleanup();
        EXPECT_TRUE(status.ok());
    }
    this->CheckFilePaths();
    UnInit();
}

// BlockMeta for Version
// SharedPtr<String> block_dir_ptr = this->GetBlockDir();
// BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
// {
//     auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
//                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
//                                                              block_dir_ptr,
//                                                              BlockVersion::FileName(),
//                                                              this->block_capacity(),
//                                                              buffer_mgr->persistence_manager());
//     version_buffer_ = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
//     if (!version_buffer_) {
//         return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_file_worker->GetFilePath()));
//     }
//     version_buffer_->AddObjRc();
// }

// ChunkIndexMeta for Index

// ColumnMeta for Data and Var(outline)
// SharedPtr<ColumnDef> col_def;
//     {
//         SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
//         std::tie(column_defs_ptr, status) = block_meta_.segment_meta().table_meta().GetColumnDefs();
//         if (!status.ok()) {
//             return status;
//         }
//         col_def = (*column_defs_ptr)[column_idx_];
//     }
//     ColumnID column_id = col_def->id();
//
//     SharedPtr<String> block_dir_ptr = block_meta_.GetBlockDir();
//     BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
//     {
//         auto filename = MakeShared<String>(fmt::format("{}.col", column_id));
//         SizeT total_data_size = 0;
//         if (col_def->type()->type() == LogicalType::kBoolean) {
//             total_data_size = (block_meta_.block_capacity() + 7) / 8;
//         } else {
//             total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
//         }
//         auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
//                                                       MakeShared<String>(InfinityContext::instance().config()->TempDir()),
//                                                       block_dir_ptr,
//                                                       filename,
//                                                       total_data_size,
//                                                       buffer_mgr->persistence_manager());
//         column_buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
//         if (!column_buffer_) {
//             return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
//         }
//         column_buffer_->AddObjRc();
//     }
//     VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
//     if (buffer_type == VectorBufferType::kVarBuffer) {
//         auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_id));
//         auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
//                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
//                                                              block_dir_ptr,
//                                                              filename,
//                                                              0, /*buffer_size*/
//                                                              buffer_mgr->persistence_manager());
//         outline_buffer_ = buffer_mgr->AllocateBufferObject(std::move(outline_file_worker));
//         if (!outline_buffer_) {
//             return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
//         }
//         outline_buffer_->AddObjRc();
//     }

// TEST_P(CleanupTaskTest, test_with_index_compact_and_restart_and_cleanup) {
//     constexpr int kImportN = 5;
//
//     Init();
//     // close auto cleanup task
//     Storage *storage = InfinityContext::instance().storage();
//     EXPECT_NE(storage, nullptr);
//
//     NewTxnManager *txn_mgr = storage->new_txn_manager();
//
//     auto db_name = MakeShared<String>("default_db");
//     auto table_name = MakeShared<String>("tbl1");
//     auto index_name = MakeShared<String>("idx1");
//     auto column_name = MakeShared<String>("col1");
//
//     Vector<SharedPtr<ColumnDef>> column_defs;
//     {
//         std::set<ConstraintType> constraints;
//         ColumnID column_id = 0;
//         column_defs.push_back(MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kInteger)), *column_name,
//         constraints));
//     }
//     {
//         auto table_def = MakeUnique<TableDef>(db_name, table_name, MakeShared<String>(), column_defs);
//         auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create tbl1"), TransactionType::kNormal);
//         auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         txn_mgr->CommitTxn(txn);
//     }
//     {
//         u32 block_row_cnt = 8192;
//         auto make_input_block = [&](SizeT val) {
//             auto input_block = MakeShared<DataBlock>();
//             auto append_to_col = [&](ColumnVector &col, Value v1) {
//                 for (u32 i = 0; i < block_row_cnt; ++i) {
//                     col.AppendValue(v1);
//                 }
//             };
//             // Initialize input block
//             {
//                 auto col1 = ColumnVector::Make(MakeShared<DataType>(DataType(LogicalType::kInteger)));
//                 col1->Initialize();
//                 append_to_col(*col1, Value::MakeInt(val));
//                 input_block->InsertVector(col1, 0);
//             }
//             input_block->Finalize();
//             return input_block;
//         };
//
//         // Import two segments, each segments contains two blocks
//         for (SizeT i = 0; i < kImportN; ++i) {
//             auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
//             Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(1), make_input_block(5)};
//             Status status = txn->Import(*db_name, *table_name, input_blocks);
//             EXPECT_TRUE(status.ok());
//             status = txn_mgr->CommitTxn(txn);
//             EXPECT_TRUE(status.ok());
//         }
//     }
//     {
//         SharedPtr<IndexBase> index_base =
//             IndexSecondary::Make(index_name, MakeShared<String>("test comment"), fmt::format("{}_{}", *table_name, *index_name), {*column_name});
//
//         auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
//         Status status = txn3->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = txn_mgr->CommitTxn(txn3);
//         EXPECT_TRUE(status.ok());
//     }
//
//     {
//         auto *txn4 = txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
//         //     Status NewTxn::DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names) {
//         //
//         //     if (column_names.empty()) {
//         //         return Status::NotSupport("DropColumns: column_names is empty");
//         //     }
//         //
//         //     {
//         //         Set<String> name_set;
//         //         for (const auto &name : column_names) {
//         //             if (!name_set.insert(name).second) {
//         //                 return Status::DuplicateColumnName(name);
//         //             }
//         //         }
//         //     }
//         //
//         //     Optional<DBMeeta> db_meta;
//         //     Optional<TableMeeta> table_meta;
//         //     String table_key;
//         //     Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
//         //     if (!status.ok()) {
//         //         return status;
//         //     }
//         //
//         //     SharedPtr<Vector<SharedPtr<ColumnDef>>> old_column_defs;
//         //     std::tie(old_column_defs, status) = table_meta->GetColumnDefs();
//         //     if (!status.ok()) {
//         //         return status;
//         //     }
//         //     Map<String, ColumnID> column_name_set;
//         //     for (const auto &column_def : *old_column_defs) {
//         //         column_name_set.emplace(column_def->name(), column_def->id());
//         //     }
//         //     Vector<ColumnID> column_ids;
//         //     for (const auto &column_name : column_names) {
//         //         if (auto iter = column_name_set.find(column_name); iter != column_name_set.end()) {
//         //             column_ids.push_back(iter->second);
//         //         } else {
//         //             return Status::ColumnNotExist(column_name);
//         //         }
//         //     }
//         //
//         //     if (column_names.size() == old_column_defs->size()) {
//         //         return Status::NotSupport("Cannot delete all the columns of a table");
//         //     }
//         //
//         //     Vector<String> *index_id_strs_ptr = nullptr;
//         //     status = table_meta->GetIndexIDs(index_id_strs_ptr);
//         //     if (!status.ok()) {
//         //         return status;
//         //     }
//         //
//         //     for (const String &index_id : *index_id_strs_ptr) {
//         //         TableIndexMeeta table_index_meta(index_id, *table_meta);
//         //         auto [index_base, index_status] = table_index_meta.GetIndexBase();
//         //         if (!index_status.ok()) {
//         //             return index_status;
//         //         }
//         //         for (const String &column_name : column_names) {
//         //             if (IsEqual(index_base->column_name(), column_name)) {
//         //                 return Status::IndexOnColumn(column_name);
//         //             }
//         //         }
//         //     }
//         //
//         //     // Put the data into local txn store
//         //     if (base_txn_store_ != nullptr) {
//         //         return Status::UnexpectedError("txn store is not null");
//         //     }
//         //
//         //     base_txn_store_ = MakeShared<DropColumnsTxnStore>();
//         //     DropColumnsTxnStore *txn_store = static_cast<DropColumnsTxnStore *>(base_txn_store_.get());
//         //     txn_store->db_name_ = db_name;
//         //     txn_store->db_id_str_ = db_meta->db_id_str();
//         //     txn_store->db_id_ = std::stoull(db_meta->db_id_str());
//         //     txn_store->table_name_ = table_name;
//         //     txn_store->column_names_ = column_names;
//         //
//         //     auto wal_command =
//         //         MakeShared<WalCmdDropColumnsV2>(db_name, db_meta->db_id_str(), table_name, table_meta->table_id_str(), column_names,
//         column_ids);
//         //     wal_command->table_key_ = table_key;
//         //     wal_entry_->cmds_.push_back(wal_command);
//         //     txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));
//         //
//         //     return Status::OK();
//         // }
//         Status status = txn4->DropColumns();
//         EXPECT_TRUE(status.ok());
//         status = txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());
//     }
//
//     UnInit();
//
//     Init();
//
//     {
//         auto *txn5 = txn_mgr->BeginTxn(MakeUnique<String>("get column"), TransactionType::kNormal);
//     }
// }