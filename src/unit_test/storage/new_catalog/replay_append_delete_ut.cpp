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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.replay_append_delete;

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
import :index_secondary;
import :index_full_text;
import :mem_index;
import :index_base;
import :buffer_obj;
import :secondary_index_in_mem;
import :memory_indexer;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
import replay_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import internal_types;
import statement_common;

class TestTxnReplayAppend : public NewReplayTest {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayAppend,
                         ::testing::Values(TestTxnReplayAppend::NEW_CONFIG_PATH, TestTxnReplayAppend::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayAppend, test_append0) {
    using namespace infinity;

    std::shared_ptr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
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

    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();
    append();

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];

        SegmentMeta segment_meta(segment_id, *table_meta);

        size_t segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 8);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

        BlockID block_id = (*block_ids_ptr)[0];
        BlockMeta block_meta(block_id, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, 8);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }
}

TEST_P(TestTxnReplayAppend, test_replay_append_delete) {
    using namespace infinity;

    std::shared_ptr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<String>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto input_block = std::make_shared<DataBlock>();
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal);
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];

        SegmentMeta segment_meta(segment_id, *table_meta);

        size_t segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 4);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        BlockID block_id = (*block_ids_ptr)[0];

        BlockMeta block_meta(block_id, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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

TEST_P(TestTxnReplayAppend, test_replay_append_with_index0) {
    using namespace infinity;

    std::shared_ptr<String> db_name = std::make_shared<String>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "file_name", {column_def2->name()}, {});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    u32 block_row_cnt = 8192;
    auto make_input_block = [&] {
        auto input_block = std::make_shared<DataBlock>();
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*db_name, *table_name, make_input_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();

    RestartTxnMgr();

    new_txn_mgr->PrintAllKeyValue();

    auto check_segment_index = [&](SegmentIndexMeta &segment_index_meta,
                                   const std::function<Pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> &check_mem_index) {
        auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({}));

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);

        auto [row_id, row_cnt] = check_mem_index(mem_index);
        EXPECT_EQ(row_id, RowID(0, 0));
        EXPECT_EQ(row_cnt, block_row_cnt * 2);
    };

    auto check_index = [&](const String &index_name, const std::function<Pair<RowID, u32>(const std::shared_ptr<MemIndex> &)> &check_mem_index) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("check index"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));

        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            check_segment_index(segment_index_meta, check_mem_index);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1, [&](const std::shared_ptr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->GetSecondaryIndex()->GetBeginRowID();
        u32 row_cnt = mem_index->GetSecondaryIndex()->GetRowCount();
        return std::make_pair(begin_id, row_cnt);
    });
    check_index(*index_name2, [&](const std::shared_ptr<MemIndex> &mem_index) {
        RowID begin_id = mem_index->GetFulltextIndex()->GetBeginRowID();
        u32 row_cnt = mem_index->GetFulltextIndex()->GetDocCount();
        return std::make_pair(begin_id, row_cnt);
    });
}
