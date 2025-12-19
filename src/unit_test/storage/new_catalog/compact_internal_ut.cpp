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

module infinity_core:ut.compact_internal;

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

class TestTxnCompactInternal : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnCompactInternal,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnCompactInternal, test_compact) {
    using namespace infinity;

    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (u32 i = 1; i < block_row_cnt; i += 2) {
            row_ids.push_back(RowID(0, i));
        }
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({2}));

        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(block_status.ok());
        EXPECT_EQ(*block_ids, std::vector<BlockID>({0, 1, 2, 3}));

        {
            BlockMeta block_meta(0, segment_meta);
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            std::pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta,
                                                            column_meta.get_column_def(),
                                                            state.block_offset_end(),
                                                            ColumnVectorMode::kReadOnly,
                                                            col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 + 1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 1), Value::MakeInt(2));
            }
        }
        for (BlockID block_id = 1; block_id < 4; ++block_id) {
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            std::pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            if (block_id == 3) {
                EXPECT_EQ(range.second, block_row_cnt / 2);
            } else {
                EXPECT_EQ(range.second, block_row_cnt);
            }

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta,
                                                            column_meta.get_column_def(),
                                                            state.block_offset_end(),
                                                            ColumnVectorMode::kReadOnly,
                                                            col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
                if (block_id == 3) {
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 1), Value::MakeInt(2));
                } else {
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 1), Value::MakeInt(2));
                }
            }
        }
    }
}

TEST_P(TestTxnCompactInternal, test_compact_with_index) {
    using namespace infinity;

    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<std::string>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                          TransactionType::kCreateIndex);
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        for (u32 i = 1; i < block_row_cnt; i += 2) {
            row_ids.push_back(RowID(0, i));
        }
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({2}));

        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(block_status.ok());
        EXPECT_EQ(*block_ids, std::vector<BlockID>({0, 1, 2, 3}));

        {
            BlockMeta block_meta(0, segment_meta);
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            std::pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, block_row_cnt);

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta,
                                                            column_meta.get_column_def(),
                                                            state.block_offset_end(),
                                                            ColumnVectorMode::kReadOnly,
                                                            col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 1), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 + 1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 2), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 1), Value::MakeInt(2));
            }
        }
        for (BlockID block_id = 1; block_id < 4; ++block_id) {
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            BlockOffset offset = 0;
            std::pair<BlockOffset, BlockOffset> range;
            bool next = state.Next(offset, range);
            EXPECT_TRUE(next);
            EXPECT_EQ(range.first, 0);
            if (block_id == 3) {
                EXPECT_EQ(range.second, block_row_cnt / 2);
            } else {
                EXPECT_EQ(range.second, block_row_cnt);
            }

            offset = range.second;
            next = state.Next(offset, range);
            EXPECT_FALSE(next);

            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta,
                                                            column_meta.get_column_def(),
                                                            state.block_offset_end(),
                                                            ColumnVectorMode::kReadOnly,
                                                            col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
                if (block_id == 3) {
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt / 2 - 1), Value::MakeInt(2));
                } else {
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 2), Value::MakeInt(1));
                    EXPECT_EQ(col.GetValueByIndex(block_row_cnt - 1), Value::MakeInt(2));
                }
            }
        }
    }
    auto check_index = [&](const std::string &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index1"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({2}));
            segment_id = (*segment_ids)[0];
        }

        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({0}));
        }
        ChunkID chunk_id = 0;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt * 4 - block_row_cnt / 2);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(segment_id, 0));
        }

        IndexFileWorker *file_worker{};
        status = chunk_index_meta.GetFileWorker(file_worker);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1);
    check_index(*index_name2);
}
