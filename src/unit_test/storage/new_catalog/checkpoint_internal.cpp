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

import replay_test;
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
import infinity;

using namespace infinity;

class TestTxnCheckpointInternalTest : public NewReplayTest {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCheckpointInternalTest,
                         ::testing::Values(TestTxnCheckpointInternalTest::NEW_CONFIG_PATH, TestTxnCheckpointInternalTest::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint0) {
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
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
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
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();

    append();

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    checkpoint();

    checkpoint();

    RestartTxnMgr();

    auto check_table = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Status status;

        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto check_block = [&](BlockMeta &block_meta) {
            Value v1 = Value::MakeInt(1);
            Value v2 = Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz");

            SizeT row_count = 0;
            // std::tie(row_count, status) = block_meta.GetRowCnt();
            std::tie(row_count, status) = block_meta.GetRowCnt1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_count, block_row_cnt);

            auto check_column = [&](ColumnID column_id, const Value &v) {
                ColumnMeta column_meta(column_id, block_meta);
                ColumnVector col1;
                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col1);
                EXPECT_TRUE(status.ok());

                for (u32 i = 0; i < row_count; ++i) {
                    EXPECT_EQ(col1.GetValueByIndex(i), v);
                }
            };

            check_column(0, v1);
            check_column(1, v2);
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(txn->kv_instance(), txn->BeginTS(), txn->CommitTS());
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));

            for (BlockID block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        {
            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
            SegmentID segment_id = (*segment_ids_ptr)[0];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
    };

    check_table();
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint1) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
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

    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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

    for (auto i = 0; i < 1; i++)
        append();

    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Status status =
            txn->Import(*db_name,
                        *table_name,
                        Vector<SharedPtr<DataBlock>>{make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    auto check_db = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Status status;

        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        EXPECT_TRUE(status.ok());
    };

    check_db();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn->DropColumns(*db_name, *table_name, Vector<String>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto default_value1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        {
            int64_t num = 1;
            default_value1->integer_value_ = num;
        }

        auto default_value2 = std::make_shared<ConstantExpr>(LiteralType::kString);
        {
            const char *str = "abcdefghijklmnopqrstuvwxyz";
            size_t str_len = strlen(str);
            default_value2->str_value_ = (char *)malloc(str_len + 1);
            strncpy(default_value2->str_value_, str, str_len + 1);
        }
        auto column_def11 =
            std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>(), "", default_value2);
        auto column_def22 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kInteger), "col3", std::set<ConstraintType>(), "", default_value1);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def11);
        columns.emplace_back(column_def22);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    auto renametable = [&](const String &new_table_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("renametable"), TransactionType::kNormal);
        Status status = txn->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    checkpoint();
    RestartTxnMgr();
    renametable("renametable");

    {
        Status status;
        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(!status.ok());
        status = txn->GetTableMeta(*db_name, "renametable", db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        checkpoint();

        RestartTxnMgr();
        auto infinity = Infinity::RemoteConnect();

        checkpoint();
        RestartTxnMgr();
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        ConflictType conflict_type_{ConflictType::kError};
        SharedPtr<String> index_name_ptr = MakeShared<String>("index_name");
        SharedPtr<String> index_comment_ptr = MakeShared<String>("index_comment");
        const String file_name = "";
        Vector<String> column_names{"col1", "col2"};
        SharedPtr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        checkpoint();
        checkpoint();
        Status status = txn->CreateIndex(*db_name, "renametable", index_base, conflict_type_);
        EXPECT_TRUE(status.ok());

        txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        SharedPtr<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        status = txn->GetTableMeta(*db_name, "renametable", db_meta, table_meta);
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        txn->GetTableIndexMeta(*db_name, "renametable", "index_name", db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        checkpoint();
        RestartTxnMgr();

        txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        SharedPtr<DBMeeta> db_meta1;
        Optional<TableMeeta> table_meta1;
        TxnTimeStamp db_create_ts1;
        status = txn->GetDBMeta(*db_name, db_meta1, db_create_ts1);
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta1;
        String table_key1;
        String index_key1;
        txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        txn->GetTableIndexMeta(*db_name, "renametable", "index_name", db_meta1, table_meta1, table_index_meta1, &table_key1, &index_key1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint2) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
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

    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (auto i = 0; i < 1; i++)
        append();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn->DropColumns(*db_name, *table_name, Vector<String>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto default_value1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        {
            int64_t num = 1;
            default_value1->integer_value_ = num;
        }

        auto default_value2 = std::make_shared<ConstantExpr>(LiteralType::kString);
        {
            const char *str = "abcdefghijklmnopqrstuvwxyz";
            size_t str_len = strlen(str);
            default_value2->str_value_ = (char *)malloc(str_len + 1);
            strncpy(default_value2->str_value_, str, str_len + 1);
        }
        auto column_def11 = std::make_shared<ColumnDef>(100000,
                                                        std::make_shared<DataType>(LogicalType::kVarchar),
                                                        "col2",
                                                        std::set<ConstraintType>(),
                                                        "",
                                                        default_value2);
        auto column_def22 = std::make_shared<ColumnDef>(999999,
                                                        std::make_shared<DataType>(LogicalType::kInteger),
                                                        "col3",
                                                        std::set<ConstraintType>(),
                                                        "",
                                                        default_value1);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def11);
        columns.emplace_back(column_def22);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint3) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(999, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(9999, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(99999, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        ConflictType conflict_type_{ConflictType::kError};
        SharedPtr<String> index_name_ptr = MakeShared<String>("index_name");
        SharedPtr<String> index_comment_ptr = MakeShared<String>("index_comment");
        const String file_name = "";
        Vector<String> column_names{"col2"};
        SharedPtr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn->DropColumns(*db_name, *table_name, Vector<String>{"col2"});
        EXPECT_TRUE(!status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn->DropColumns(*db_name, *table_name, Vector<String>{"col1"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize indexes"), TransactionType::kNormal);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*db_name, *table_name, "index_name", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn->DropColumns(*db_name, *table_name, Vector<String>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize indexes"), TransactionType::kNormal);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint4) {
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
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
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
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    append();

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        {
            Status status;
            SharedPtr<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            TxnTimeStamp db_create_ts;
            status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
            status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            auto [segment_ids, status1] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status1.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        Status status = txn->Compact(*db_name, *table_name, Vector<SegmentID>({0}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint5) {
    SharedPtr<String> db_name = std::make_shared<String>("db1");

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);

        Status status = txn1->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("clean"), TransactionType::kNormal);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
        SharedPtr<DBMeeta> db_meta;
        TxnTimeStamp db_creat_ts;
        Status status = txn->GetDBMeta(*db_name, db_meta, db_creat_ts);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
