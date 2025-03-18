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
// import index_base;
// import index_secondary;
// import index_ivf;
// import index_full_text;
// import index_hnsw;
// import embedding_info;
// import sparse_info;
// import index_bmp;
// import internal_types;
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
// import buffer_obj;
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

using namespace infinity;

class TestImport : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestImport,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TestImport, test_import1) {

    using namespace infinity;

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

    // Import two segments, each segments contains two blocks
    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
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

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

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
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
                check_block(block_meta);
            }
        };

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
            check_segment(segment_meta);
        }
    }
}

TEST_P(TestImport, test_import_drop_db) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts) {
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
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        auto [block_ids, status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            check_block(block_meta, begin_ts);
        }
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop db   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestImport, test_import_drop_table) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts) {
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
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        auto [block_ids, status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            check_block(block_meta, begin_ts);
        }
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestImport, test_import_add_columns) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts) {
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
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        auto [block_ids, status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            check_block(block_meta, begin_ts);
        }
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add columns    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  add columns    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                           t1      import                                          rollback (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                     t1      import (fail)                                       rollback
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                               t1      import                                           commit (success)
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns     commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
}

TEST_P(TestImport, test_import_drop_columns) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts) {
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
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        auto [block_ids, status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            check_block(block_meta, begin_ts);
        }
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop columns    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|--------------------|
    //                           t2                  drop column (fail)    rollback (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                                     commit (success)
    //    |----------|--------------------------------------------------------------|
    //                            |----------------------|-------------------|
    //                           t2                  drop column (fail)  rollback (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|-----------------------|
    //         t2                  drop columns (fail)    rollback (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------------------|
    //         t2                  drop columns          rollback (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                           t1      import                                          rollback (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // Drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                     t1      import (fail)                                       rollback
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        // Add two columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                               t1      import                                           commit (success)
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns     commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
}

TEST_P(TestImport, test_import) {
    using namespace infinity;

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
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
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

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

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
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
                check_block(block_meta);
            }
        };

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
            check_segment(segment_meta);
        }
    }

    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
}

TEST_P(TestImport, test_import_append_table) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts) {
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
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValue(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValue(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValue(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValue(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValue(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        auto [block_ids, status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());
            check_block(block_meta, begin_ts);
        }
    };

    auto input_block1 = MakeShared<DataBlock>();
    {
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 1"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1, 2}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append   commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  append    commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append   commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append   commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta, *txn5->kv_instance());
            check_segment(segment_meta, txn5);
        }
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
}
