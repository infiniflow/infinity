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
// import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
// import index_ivf;
import index_full_text;
// import index_hnsw;
// import embedding_info;
// import sparse_info;
// import index_bmp;
import internal_types;
// import defer_op;
// import statement_common;
// import meta_info;
import data_block;
import column_vector;
import value;
// import data_access_state;
// import kv_code;
// import kv_store;
import new_txn;
// import new_txn_store;
import buffer_obj;
// import buffer_handle;
// import secondary_index_in_mem;
// import secondary_index_data;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
// import catalog_meta;
// import mem_index;
// import roaring_bitmap;
// import index_filter_evaluators;
// import index_emvb;
import constant_expr;
// import config;
// import virtual_store;
// import default_values;
// import parsed_expr;
import logger;

using namespace infinity;

class TestTxnAlter : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnAlter,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnAlter, add_column0) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 =
        std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>(), default_varchar);
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
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
        input_block->Finalize();
        return input_block;
    };

    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    sleep(1);
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_defs = Vector<SharedPtr<ColumnDef>>({column_def2});
        Status status = txn->AddColumns(*db_name, *table_name, column_defs);
        if (!status.ok()) {
            LOG_INFO(status.message());
        }
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        new_txn_mgr->PrintAllKeyValue();

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto check_column = [&](ColumnMeta &column_meta, const Value &v, SizeT row_count) {
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            for (SizeT i = 0; i < row_count; ++i) {
                EXPECT_EQ(col.GetValue(i), v);
            }
        };

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            Pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);
            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            SizeT row_count = state.block_offset_end();
            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                check_column(column_meta, Value::MakeInt(1), row_count);
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                check_column(column_meta, Value::MakeVarchar(""), row_count);
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
    }
}

TEST_P(TestTxnAlter, drop_column0) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names = {column_def2->name_};
        Status status = txn->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto check_column = [&](ColumnMeta &column_meta, const Value &v, SizeT row_count) {
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            for (SizeT i = 0; i < row_count; ++i) {
                EXPECT_EQ(col.GetValue(i), v);
            }
        };

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            Pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);
            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            SizeT row_count = state.block_offset_end();
            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                check_column(column_meta, Value::MakeInt(1), row_count);
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
    }
}
