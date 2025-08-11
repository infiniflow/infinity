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

module infinity_core:ut.replay_optimize;

import :ut.base_test;
import :ut.replay_test;
import :stl;
import :third_party;
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
import :index_hnsw;
import :mem_index;
import :index_base;

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

using namespace infinity;

class TestTxnReplayOptimize : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();
        db_name = std::make_shared<String>("default_db");
        
        // Create columns for different index types
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "id", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "text_col", std::set<ConstraintType>());
        column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding), "embedding_col", std::set<ConstraintType>());
        column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kFloat), "float_col", std::set<ConstraintType>());
        
        table_name = std::make_shared<std::string>("optimize_test_table");
        table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2, column_def3, column_def4});
        
        // Create different types of indexes
        index_name1 = std::make_shared<std::string>("idx_secondary");
        index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        
        index_name2 = std::make_shared<String>("idx_fulltext");
        index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
        
        index_name3 = std::make_shared<String>("idx_hnsw");
        index_def3 = IndexHnsw::Make(index_name3, MakeShared<String>(), "file_name", {column_def3->name()}, {});
        
        index_name4 = std::make_shared<String>("idx_secondary2");
        index_def4 = IndexSecondary::Make(index_name4, MakeShared<String>(), "file_name", {column_def4->name()});
        
        block_row_cnt = 8192;
    }

    SharedPtr<DataBlock> make_input_block() {
        auto input_block = MakeShared<DataBlock>();
        
        // Column 1: Integer IDs
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block->InsertVector(col1, 0);
        }
        
        // Column 2: Text for fulltext index
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                String text_value = "text_" + std::to_string(i);
                col2->AppendValue(Value::MakeVarchar(text_value));
            }
            input_block->InsertVector(col2, 1);
        }
        
        // Column 3: Embeddings for HNSW index
        {
            auto col3 = ColumnVector::Make(column_def3->type());
            col3->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                Vector<f32> embedding = {static_cast<f32>(i), static_cast<f32>(i+1), static_cast<f32>(i+2), static_cast<f32>(i+3)};
                col3->AppendValue(Value::MakeEmbedding(embedding));
            }
            input_block->InsertVector(col3, 2);
        }
        
        // Column 4: Float values for secondary index
        {
            auto col4 = ColumnVector::Make(column_def4->type());
            col4->Initialize();
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col4->AppendValue(Value::MakeFloat(static_cast<f32>(i) * 1.5f));
            }
            input_block->InsertVector(col4, 3);
        }
        
        input_block->Finalize();
        return input_block;
    }

    void CheckDataAfterSuccessfulOptimize() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SizeT table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, block_row_cnt * 4); // 4 blocks inserted

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        // After optimization, should have 4 segments (0, 1, 2, 3)
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0, 1, 2, 3}));
        
        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, *table_meta);

            SizeT segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, block_row_cnt); // Each segment has one block

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0})); // Each segment has one block

            SizeT block_row_cnt = 0;
            for (const auto &block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
                EXPECT_EQ(block_row_cnt, block_row_cnt);

                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, txn->BeginTS(), txn->CommitTS(), state);
                EXPECT_TRUE(status.ok());
                {
                    Pair<BlockOffset, BlockOffset> range;
                    BlockOffset offset = 0;
                    bool has_next = state.Next(offset, range);
                    EXPECT_TRUE(has_next);
                    EXPECT_EQ(range.first, 0);
                    EXPECT_EQ(range.second, block_row_cnt);
                    offset = range.second;
                    has_next = state.Next(offset, range);
                    EXPECT_FALSE(has_next);
                }
            }
        }
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckDataAfterFailedOptimize() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SizeT table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, block_row_cnt * 4); // 4 blocks inserted

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        // After failed optimization, should still have 4 segments (0, 1, 2, 3)
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0, 1, 2, 3}));
        
        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, *table_meta);

            SizeT segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, block_row_cnt); // Each segment has one block

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0})); // Each segment has one block

            SizeT block_row_cnt = 0;
            for (const auto &block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
                EXPECT_EQ(block_row_cnt, block_row_cnt);

                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, txn->BeginTS(), txn->CommitTS(), state);
                EXPECT_TRUE(status.ok());
                {
                    Pair<BlockOffset, BlockOffset> range;
                    BlockOffset offset = 0;
                    bool has_next = state.Next(offset, range);
                    EXPECT_TRUE(has_next);
                    EXPECT_EQ(range.first, 0);
                    EXPECT_EQ(range.second, block_row_cnt);
                    offset = range.second;
                    has_next = state.Next(offset, range);
                    EXPECT_FALSE(has_next);
                }
            }
        }
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckIndexBeforeOptimize(const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index before"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // Before optimization, each segment should have multiple chunks
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0, 1, 2, 3})); // 4 segments
        
        for (auto segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            
            Vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            // Before optimization, each segment should have multiple chunks (due to Append)
            EXPECT_GT(chunk_ids_ptr->size(), 1); // More than one chunk per segment
            
            // Store original chunk IDs for verification
            original_chunk_ids_[index_name][segment_id] = *chunk_ids_ptr;
            
            // Verify each chunk has the expected row count
            for (auto chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                
                ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
                status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
                EXPECT_TRUE(status.ok());
                EXPECT_GT(chunk_info_ptr->row_cnt_, 0); // Each chunk should have some rows
            }
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckIndexAfterSuccessfulOptimize(const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // After successful optimization, each segment should have exactly ONE chunk
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0, 1, 2, 3})); // 4 segments
        
        for (auto segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            
            Vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            // After successful optimization, each segment should have exactly ONE chunk
            EXPECT_EQ(chunk_ids_ptr->size(), 1);
            
            ChunkID chunk_id = (*chunk_ids_ptr)[0];
            
            // Verify that the new chunk ID is the last one (highest ID)
            auto original_chunks_it = original_chunk_ids_.find(index_name);
            if (original_chunks_it != original_chunk_ids_.end()) {
                auto segment_chunks_it = original_chunks_it->second.find(segment_id);
                if (segment_chunks_it != original_chunks_it->second.end()) {
                    const Vector<ChunkID>& original_chunk_ids = segment_chunks_it->second;
                    // The new chunk ID should be greater than all original chunk IDs
                    ChunkID max_original_chunk_id = 0;
                    for (auto original_chunk_id : original_chunk_ids) {
                        EXPECT_NE(chunk_id, original_chunk_id); // Should be different
                        max_original_chunk_id = std::max(max_original_chunk_id, original_chunk_id);
                    }
                    // The new chunk ID should be the last one (highest)
                    EXPECT_EQ(chunk_id, max_original_chunk_id + 1);
                }
            }
            
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 0));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, block_row_cnt); // One block per segment
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckIndexAfterFailedOptimize(const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // After failed optimization, each segment should still have multiple chunks (no consolidation happened)
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0, 1, 2, 3})); // 4 segments
        
        for (auto segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            
            Vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            // After failed optimization, each segment should still have multiple chunks (no consolidation)
            EXPECT_GT(chunk_ids_ptr->size(), 1); // More than one chunk per segment
            
            // Verify each chunk has the expected row count
            for (auto chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                
                ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
                status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
                EXPECT_TRUE(status.ok());
                EXPECT_GT(chunk_info_ptr->row_cnt_, 0); // Each chunk should have some rows
            }
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

protected:
    SharedPtr<String> db_name{};
    SharedPtr<ColumnDef> column_def1{};
    SharedPtr<ColumnDef> column_def2{};
    SharedPtr<ColumnDef> column_def3{};
    SharedPtr<ColumnDef> column_def4{};
    SharedPtr<String> table_name{};
    SharedPtr<TableDef> table_def{};
    SharedPtr<String> index_name1{};
    SharedPtr<IndexBase> index_def1{};
    SharedPtr<String> index_name2{};
    SharedPtr<IndexBase> index_def2{};
    SharedPtr<String> index_name3{};
    SharedPtr<IndexBase> index_def3{};
    SharedPtr<String> index_name4{};
    SharedPtr<IndexBase> index_def4{};
    u32 block_row_cnt{};
    
    // Store original chunk IDs for verification
    Map<String, Map<SegmentID, Vector<ChunkID>>> original_chunk_ids_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayOptimize,
                         ::testing::Values(TestTxnReplayOptimize::NEW_CONFIG_PATH, TestTxnReplayOptimize::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayOptimize, test_optimize_commit) {
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create all indexes
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create indexes"), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def4, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Insert data into 4 segments with multiple chunks per segment using Append
    for (int i = 0; i < 4; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        SharedPtr<DataBlock> input_block = make_input_block();
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check index state before optimization (should have multiple chunks)
    CheckIndexBeforeOptimize(*index_name1);
    CheckIndexBeforeOptimize(*index_name2);
    CheckIndexBeforeOptimize(*index_name3);
    CheckIndexBeforeOptimize(*index_name4);

    // Optimize table
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize"), TransactionType::kNormal);
        Status status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    CheckDataAfterSuccessfulOptimize();
    CheckIndexAfterSuccessfulOptimize(*index_name1);
    CheckIndexAfterSuccessfulOptimize(*index_name2);
    CheckIndexAfterSuccessfulOptimize(*index_name3);
    CheckIndexAfterSuccessfulOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayOptimize, test_optimize_rollback) {
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create all indexes
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create indexes"), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def4, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Insert data into 4 segments with multiple chunks per segment using Append
    for (int i = 0; i < 4; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        SharedPtr<DataBlock> input_block = make_input_block();
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check index state before optimization (should have multiple chunks)
    CheckIndexBeforeOptimize(*index_name1);
    CheckIndexBeforeOptimize(*index_name2);
    CheckIndexBeforeOptimize(*index_name3);
    CheckIndexBeforeOptimize(*index_name4);

    // Try to optimize but it will be rolled back due to conflict
    {
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Status status = txn2->DropColumns(*db_name, *table_name, Vector<String>({column_def1->name()}));
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize"), TransactionType::kNormal);
        status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok()); // Should fail due to column drop conflict
    }

    RestartTxnMgr();

    CheckDataAfterFailedOptimize();
    CheckIndexAfterFailedOptimize(*index_name1);
    CheckIndexAfterFailedOptimize(*index_name2);
    CheckIndexAfterFailedOptimize(*index_name3);
    CheckIndexAfterFailedOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayOptimize, test_optimize_interrupt) {
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create all indexes
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create indexes"), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = txn->CreateIndex(*db_name, *table_name, index_def4, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Insert data into 4 segments with multiple chunks per segment using Append
    for (int i = 0; i < 4; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        SharedPtr<DataBlock> input_block = make_input_block();
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check index state before optimization (should have multiple chunks)
    CheckIndexBeforeOptimize(*index_name1);
    CheckIndexBeforeOptimize(*index_name2);
    CheckIndexBeforeOptimize(*index_name3);
    CheckIndexBeforeOptimize(*index_name4);

    // Start optimize but interrupt before commit
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize"), TransactionType::kNormal);
        Status status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        // Don't commit - simulate interruption
    }

    RestartTxnMgr();

    CheckDataAfterFailedOptimize();
    CheckIndexAfterFailedOptimize(*index_name1);
    CheckIndexAfterFailedOptimize(*index_name2);
    CheckIndexAfterFailedOptimize(*index_name3);
    CheckIndexAfterFailedOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

