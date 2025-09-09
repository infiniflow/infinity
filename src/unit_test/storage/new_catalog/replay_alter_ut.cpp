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

module infinity_core:ut.replay_alter;

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
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
import :default_values;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import internal_types;

class TestTxnReplayAlter : public NewReplayTest {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayAlter,
                         ::testing::Values(TestTxnReplayAlter::NEW_CONFIG_PATH, TestTxnReplayAlter::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayAlter, test_add_column) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    auto column_def2 =
        std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>(), default_varchar);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto make_input_block = [&](const Value &v1, size_t row_cnt) {
        auto make_column = [&](std::shared_ptr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (size_t i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = std::make_shared<DataBlock>();
        {
            auto col1 = make_column(column_def1, v1);
            input_block->InsertVector(col1, 0);
        }
        input_block->Finalize();
        return input_block;
    };

    size_t row_cnt = 4;
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(1), row_cnt);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kNormal);

        Status status = txn->AddColumns(*db_name, *table_name, std::vector<std::shared_ptr<ColumnDef>>({column_def2}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kNormal);

        // TxnTimeStamp begin_ts = txn->BeginTS();

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
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
        EXPECT_EQ(segment_row_cnt, row_cnt);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));

        auto [column_defs, status1] = table_meta->GetColumnDefs();
        EXPECT_TRUE(status1.ok());
        EXPECT_EQ(column_defs->size(), 2);

        BlockMeta block_meta(0, segment_meta);
        {
            ColumnMeta column_meta(1, block_meta);
            ColumnVector column_vector;

            Status status =
                NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_cnt, ColumnVectorMode::kReadOnly, column_vector);
            EXPECT_TRUE(status.ok());

            for (size_t i = 0; i < row_cnt; ++i) {
                auto value = column_vector.GetValueByIndex(i);
                EXPECT_EQ(value, Value::MakeVarchar(""));
            }
        }
    }
}

TEST_P(TestTxnReplayAlter, test_drop_column) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto make_input_block = [&](const Value &v1, const Value &v2, size_t row_cnt) {
        auto make_column = [&](std::shared_ptr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (size_t i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = std::make_shared<DataBlock>();
        {
            auto col1 = make_column(column_def1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(column_def2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    size_t row_cnt = 4;
    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), row_cnt);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kNormal);

        Status status = txn->DropColumns(*db_name, *table_name, std::vector<std::string>({column_def2->name_}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kNormal);

        // TxnTimeStamp begin_ts = txn->BeginTS();

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [column_defs, status1] = table_meta->GetColumnDefs();
        EXPECT_TRUE(status1.ok());
        EXPECT_EQ(column_defs->size(), 1);
    }
}
