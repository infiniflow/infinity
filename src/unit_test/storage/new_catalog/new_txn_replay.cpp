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
import new_txn_manager;
import new_txn;
import column_def;
import data_type;
import table_def;
import index_base;
import infinity_context;
import logical_type;
import txn_state;
import extra_ddl_info;
import data_block;
import column_vector;
import value;
import new_catalog;
import internal_types;

import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import catalog_meta;

import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import index_emvb;
import embedding_info;
import sparse_info;
import index_bmp;
import defer_op;
import statement_common;
import mem_index;

import buffer_obj;
import buffer_handle;

// import infinity_exception;
// import meta_info;
// import value;
// import data_access_state;
// import kv_code;
// import kv_store;
// import new_txn;
// import new_txn_store;

// import secondary_index_in_mem;
// import secondary_index_data;

// import roaring_bitmap;
// import index_filter_evaluators;

// import constant_expr;

using namespace infinity;

class NewTxnReplayTest : public BaseTestParamStr {
protected:
    void RestartTxnMgr() {
        new_txn_mgr->PrintAllKeyValue();
        infinity::InfinityContext::instance().UnInit();

        InfinityContext::instance().InitPhase1(this->config_path);
        InfinityContext::instance().InitPhase2();
        new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

        new_txn_mgr->PrintAllKeyValue();
    }

    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    }

    NewTxnManager *new_txn_mgr = nullptr;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewTxnReplayTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(NewTxnReplayTest, test_replay_create_db) {
    using namespace infinity;

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
        Vector<String> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(Set<String>(db_names.begin(), db_names.end()), Set<String>({"db1", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewTxnReplayTest, test_replay_create_db2) {
    using namespace infinity;

    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db1", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
        Vector<String> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(Set<String>(db_names.begin(), db_names.end()), Set<String>({"db1", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase("db2", ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
        Vector<String> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(Set<String>(db_names.begin(), db_names.end()), Set<String>({"db1", "db2", "default_db"}));
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewTxnReplayTest, test_replay_create_table) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        // check table
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Vector<String> table_names;
        Status status = txn->ListTable(*db_name, table_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(table_names, Vector<String>{*table_name});

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(NewTxnReplayTest, test_replay_append) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
        BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());

        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, 4);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }
}

TEST_P(NewTxnReplayTest, test_replay_append_delete) {
    using namespace infinity;

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta, *txn->kv_instance());
        BlockMeta block_meta(block_id, segment_meta, *txn->kv_instance());

        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, 1);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 2);
            EXPECT_EQ(range.second, 3);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }
}

TEST_P(NewTxnReplayTest, test_replay_import) {
    using namespace infinity;

    new_txn_mgr->PrintAllKeyValue();

    SharedPtr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
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

    RestartTxnMgr();

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
