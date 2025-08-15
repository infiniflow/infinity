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
import embedding_info;


using namespace infinity;

class TestTxnReplayOptimize : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();
        db_name = std::make_shared<String>("default_db");
        
        // Create columns for different index types
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "id", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "text_col", std::set<ConstraintType>());
        
        // Create embedding column with proper type info (4-dimensional float vectors)
        auto embedding_type_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 4);
        column_def3 = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kEmbedding, embedding_type_info), "embedding_col", std::set<ConstraintType>());
        
        column_def4 = std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kFloat), "float_col", std::set<ConstraintType>());
        
        table_name = std::make_shared<std::string>("optimize_test_table");
        table_def = TableDef::Make(db_name, table_name, std::make_shared<String>(), {column_def1, column_def2, column_def3, column_def4});
        
        // Create different types of indexes
        index_name1 = std::make_shared<std::string>("idx_secondary");
        index_def1 = IndexSecondary::Make(index_name1, std::make_shared<String>(), "idx_file1.idx", {column_def1->name()});
        
        index_name2 = std::make_shared<String>("idx_fulltext");
        index_def2 = IndexFullText::Make(index_name2, std::make_shared<String>(), "idx_file2.idx", {column_def2->name()}, {});
        
        index_name3 = std::make_shared<String>("idx_hnsw");
        
        // Create HNSW index with proper parameters following the pattern from other tests
        Vector<std::unique_ptr<InitParameter>> index_param_list;
        Vector<InitParameter *> index_param_list_ptr;
        
        // Add required parameters for HNSW index
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"metric", "l2"}));
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"encode", "plain"}));
        
        // Convert to raw pointers for the API call
        for (auto &param : index_param_list) {
            index_param_list_ptr.push_back(param.get());
        }
        
        LOG_INFO("Creating HNSW index with parameters: metric=l2, encode=plain");
        LOG_INFO("HNSW params size: " + std::to_string(index_param_list_ptr.size()));
        
        try {
            LOG_INFO("Attempting to create HNSW index with column: " + column_def3->name());
            index_def3 = IndexHnsw::Make(index_name3, std::make_shared<String>(), "idx_file3.idx", {column_def3->name()}, index_param_list_ptr);
            EXPECT_TRUE(index_def3 != nullptr);
            LOG_INFO("Successfully created HNSW index: " + *index_name3);
        } catch (const std::exception& e) {
            FAIL() << "Failed to create HNSW index: " << e.what();
        }
        
        index_name4 = std::make_shared<String>("idx_secondary2");
        index_def4 = IndexSecondary::Make(index_name4, std::make_shared<String>(), "idx_file4.idx", {column_def4->name()});
        
        block_row_cnt = 8192;
    }
    
    ~TestTxnReplayOptimize() override {
        // No manual cleanup needed - using RAII with unique_ptr
    }

    std::shared_ptr<DataBlock> make_input_block() {
        auto input_block = std::make_shared<DataBlock>();
        
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


    void CheckIndexBeforeOptimize(const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("check index before"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // Before optimization, should have 1 segment (0) with 4 blocks
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0})); // 1 segment
        
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
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("check index"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // After successful optimization, should have 1 segment (0) with 4 blocks
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0})); // 1 segment
        
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
            EXPECT_EQ(chunk_info_ptr->row_cnt_, block_row_cnt*4); // Four blocks per segment
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CheckIndexAfterFailedOptimize(const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("check index"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        // After failed optimization, should still have 1 segment (0) with 4 blocks
        auto [segment_ids_ptr, status2] = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status2.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0})); // 1 segment
        
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
    
    // Helper function to prepare table with indexes, data, and dumped indexes
    void PrepareTableWithIndexesAndData() {
        // Create all indexes one by one
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create secondary index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create fulltext index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create HNSW index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def3, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create secondary index 2"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def4, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        
        // Insert data into 1 segment with 4 blocks, dump indexes after each append
        for (int block_id = 0; block_id < 4; ++block_id) {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
            std::shared_ptr<DataBlock> input_block = make_input_block();
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
            
            // Dump all indexes for segment 0 after each append (each block)
            auto *dump_txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem indexes block {}", block_id)), TransactionType::kNormal);
            
            Status dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem indexes block {}", block_id)), TransactionType::kNormal);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name2, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem indexes block {}", block_id)), TransactionType::kNormal);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name3, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);

            dump_txn = new_txn_mgr->BeginTxn(std::make_unique<String>(fmt::format("dump mem indexes block {}", block_id)), TransactionType::kNormal);
            dump_status = dump_txn->DumpMemIndex(*db_name, *table_name, *index_name4, 0);
            EXPECT_TRUE(dump_status.ok());
            new_txn_mgr->CommitTxn(dump_txn);
        }
    }

protected:
    std::shared_ptr<String> db_name{};
    std::shared_ptr<ColumnDef> column_def1{};
    std::shared_ptr<ColumnDef> column_def2{};
    std::shared_ptr<ColumnDef> column_def3{};
    std::shared_ptr<ColumnDef> column_def4{};
    std::shared_ptr<String> table_name{};
    std::shared_ptr<TableDef> table_def{};
    std::shared_ptr<String> index_name1{};
    std::shared_ptr<IndexBase> index_def1{};
    std::shared_ptr<String> index_name2{};
    std::shared_ptr<IndexBase> index_def2{};
    std::shared_ptr<String> index_name3{};
    std::shared_ptr<IndexBase> index_def3{};
    std::shared_ptr<String> index_name4{};
    std::shared_ptr<IndexBase> index_def4{};
    u32 block_row_cnt{};
    
    // Store original chunk IDs for verification
    Map<String, Map<SegmentID, Vector<ChunkID>>> original_chunk_ids_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayOptimize,
                         ::testing::Values(TestTxnReplayOptimize::NEW_CONFIG_PATH, TestTxnReplayOptimize::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayOptimize, test_optimize_commit) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    CheckIndexBeforeOptimize(*index_name1);
    CheckIndexBeforeOptimize(*index_name2);
    CheckIndexBeforeOptimize(*index_name3);
    CheckIndexBeforeOptimize(*index_name4);

    // Optimize table
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("optimize"), TransactionType::kNormal);
        Status status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    CheckIndexAfterSuccessfulOptimize(*index_name1);
    CheckIndexAfterSuccessfulOptimize(*index_name2);
    CheckIndexAfterSuccessfulOptimize(*index_name3);
    CheckIndexAfterSuccessfulOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayOptimize, test_optimize_rollback) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    CheckIndexBeforeOptimize(*index_name1);
    CheckIndexBeforeOptimize(*index_name2);
    CheckIndexBeforeOptimize(*index_name3);
    CheckIndexBeforeOptimize(*index_name4);


    // Try to optimize but it will be rolled back due to conflict
    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        std::shared_ptr<DataBlock> input_block = make_input_block();
        Status status = txn2->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("optimize"), TransactionType::kNormal);
        status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }

    CheckIndexAfterFailedOptimize(*index_name1);
    CheckIndexAfterFailedOptimize(*index_name2);
    CheckIndexAfterFailedOptimize(*index_name3);
    CheckIndexAfterFailedOptimize(*index_name4);

    RestartTxnMgr();

    CheckIndexAfterFailedOptimize(*index_name1);
    CheckIndexAfterFailedOptimize(*index_name2);
    CheckIndexAfterFailedOptimize(*index_name3);
    CheckIndexAfterFailedOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayOptimize, test_optimize_interrupt) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Prepare table with indexes, data, and dumped indexes
    PrepareTableWithIndexesAndData();

    // Start optimize but interrupt before commit
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("optimize"), TransactionType::kNormal);
        Status status = txn->OptimizeTableIndexes(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        // Don't commit - simulate interruption
    }

    RestartTxnMgr();

    CheckIndexAfterFailedOptimize(*index_name1);
    CheckIndexAfterFailedOptimize(*index_name2);
    CheckIndexAfterFailedOptimize(*index_name3);
    CheckIndexAfterFailedOptimize(*index_name4);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

