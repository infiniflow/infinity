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

module infinity_core:ut.checkpoint_internal;

import :ut.replay_test;
import :ut.base_test;

import third_party;
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
import :infinity;

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

class TestTxnCheckpointInternalTest : public NewReplayTest {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCheckpointInternalTest,
                         ::testing::Values(TestTxnCheckpointInternalTest::NEW_CONFIG_PATH, TestTxnCheckpointInternalTest::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint0) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
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
    auto append = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();

    append();

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    checkpoint();

    checkpoint();

    RestartTxnMgr();

    auto check_table = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        Status status;

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto check_block = [&](BlockMeta &block_meta) {
            Value v1 = Value::MakeInt(1);
            Value v2 = Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz");

            size_t row_count = 0;
            std::tie(row_count, status) = block_meta.GetRowCnt1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(row_count, block_row_cnt);

            auto check_column = [&](ColumnID column_id, const Value &v) {
                ColumnMeta column_meta(column_id, block_meta);
                ColumnVector col1;
                status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col1);
                EXPECT_TRUE(status.ok());

                for (u32 i = 0; i < row_count; ++i) {
                    EXPECT_EQ(col1.GetValueByIndex(i), v);
                }
            };

            check_column(0, v1);
            check_column(1, v2);
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0, 1}));

            for (BlockID block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        {
            std::vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
            SegmentID segment_id = (*segment_ids_ptr)[0];
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
    };

    check_table();
}

TEST_P(TestTxnCheckpointInternalTest, SLOW_test_checkpoint1) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (auto i = 0; i < 1; i++)
        append();

    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        Status status = txn->Import(
            *db_name,
            *table_name,
            std::vector<std::shared_ptr<DataBlock>>{make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    auto check_db = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        Status status;

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        EXPECT_TRUE(status.ok());
    };

    check_db();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
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
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def11);
        columns.emplace_back(column_def22);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    auto renametable = [&](const std::string &new_table_name) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("renametable"), TransactionType::kRenameTable);
        Status status = txn->RenameTable(*db_name, *table_name, new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    checkpoint();
    RestartTxnMgr();
    renametable("renametable");

    {
        Status status;
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        TxnTimeStamp create_timestamp;
        status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(!status.ok());
        status = txn->GetTableMeta(*db_name, "renametable", db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());
        checkpoint();

        RestartTxnMgr();
        auto infinity = Infinity::RemoteConnect();

        checkpoint();
        RestartTxnMgr();
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col1", "col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        checkpoint();
        checkpoint();
        Status status = txn->CreateIndex(*db_name, "renametable", index_base, conflict_type_);
        EXPECT_TRUE(status.ok());

        txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp db_create_ts;
        status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
        TxnTimeStamp create_timestamp;
        status = txn->GetTableMeta(*db_name, "renametable", db_meta, table_meta, create_timestamp);
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        txn->GetTableIndexMeta(*db_name, "renametable", "index_name", db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        checkpoint();
        RestartTxnMgr();

        txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta1;
        std::shared_ptr<TableMeta> table_meta1;
        TxnTimeStamp db_create_ts1;
        status = txn->GetDBMeta(*db_name, db_meta1, db_create_ts1);
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<TableIndexMeta> table_index_meta1;
        std::string table_key1;
        std::string index_key1;
        txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        txn->GetTableIndexMeta(*db_name, "renametable", "index_name", db_meta1, table_meta1, table_index_meta1, &table_key1, &index_key1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint2) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

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
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (auto i = 0; i < 1; i++) {
        append();
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
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
        std::vector columns{column_def11, column_def22};
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    [[maybe_unused]] auto *fileworker_mgr = infinity::InfinityContext::instance().storage()->fileworker_manager();

    checkpoint();
    RestartTxnMgr();
}

TEST_P(TestTxnCheckpointInternalTest, SLOW_test_checkpoint3) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(999, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(9999, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto column_def3 = std::make_shared<ColumnDef>(99999, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2, column_def3});

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        std::shared_ptr<std::string> index_name_ptr = std::make_shared<std::string>("index_name");
        std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("dump index"), TransactionType::kDumpMemIndex);
        Status status = txn->DumpMemIndex(*db_name, *table_name, "index_name");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col2"});
        EXPECT_TRUE(!status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col1"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("optimize indexes"), TransactionType::kOptimizeIndex);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn->DropIndexByName(*db_name, *table_name, "index_name", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>{"col2"});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();
    checkpoint();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("optimize indexes"), TransactionType::kOptimizeIndex);
        Status status = txn->OptimizeAllIndexes();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
}

TEST_P(TestTxnCheckpointInternalTest, SLOW_test_checkpoint4) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kError);
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
    auto append = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto checkpoint = [&] {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
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
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        {
            Status status;
            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp db_create_ts;
            status = txn->GetDBMeta(*db_name, db_meta, db_create_ts);
            TxnTimeStamp create_timestamp;
            status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            auto [segment_ids, status1] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status1.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }
        Status status = txn->Compact(*db_name, *table_name, std::vector<SegmentID>({0}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    checkpoint();
    RestartTxnMgr();
    checkpoint();
}

TEST_P(TestTxnCheckpointInternalTest, test_checkpoint5) {
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

        Status status = txn1->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn->Checkpoint(wal_manager_->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("clean"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        TxnTimeStamp db_creat_ts;
        Status status = txn->GetDBMeta(*db_name, db_meta, db_creat_ts);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
