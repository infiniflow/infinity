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
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meta;
import :default_values;
import :index_secondary;
import :index_full_text;
import :mem_index;
import :index_base;
import :secondary_index_in_mem;
import :memory_indexer;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import internal_types;
import statement_common;

class TestTxnReplayAppend : public NewReplayTest {
public:
    std::shared_ptr<DataBlock> MakeInputBlock(const Value &v1, size_t row_cnt) {
        auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 2);
        auto column_def1 = std::make_shared<ColumnDef>(0,
                                                       std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info),
                                                       "col1",
                                                       std::set<ConstraintType>());

        auto input_block = std::make_shared<DataBlock>();
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (u32 i = 0; i < row_cnt; ++i) {
                col1->AppendValue(v1);
            }
            input_block->InsertVector(col1, 0);
        }
        input_block->Finalize();
        return input_block;
    };
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayAppend,
                         ::testing::Values(TestTxnReplayAppend::NEW_CONFIG_PATH, TestTxnReplayAppend::NEW_VFS_OFF_CONFIG_PATH));

// TEST_P(TestTxnReplayAppend, test_append0) {
//     using namespace infinity;
//
//     std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
//     auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
//     auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
//     auto table_name = std::make_shared<std::string>("tb1");
//     auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
//         Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
//         Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//
//     auto make_input_block = [&](const Value &v1, const Value &v2, size_t row_cnt) {
//         auto make_column = [&](std::shared_ptr<ColumnDef> &column_def, const Value &v) {
//             auto col = ColumnVector::Make(column_def->type());
//             col->Initialize();
//             for (size_t i = 0; i < row_cnt; ++i) {
//                 col->AppendValue(v);
//             }
//             return col;
//         };
//         auto input_block = std::make_shared<DataBlock>();
//         {
//             auto col1 = make_column(column_def1, v1);
//             input_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = make_column(column_def2, v2);
//             input_block->InsertVector(col2, 1);
//         }
//         input_block->Finalize();
//         return input_block;
//     };
//
//     auto append = [&] {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
//         auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     append();
//     append();
//
//     RestartTxnMgr();
//
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();
//
//         std::shared_ptr<DBMeta> db_meta;
//         std::shared_ptr<TableMeta> table_meta;
//         TxnTimeStamp create_timestamp;
//         Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
//         EXPECT_TRUE(status.ok());
//
//         std::vector<SegmentID> *segment_ids_ptr = nullptr;
//         std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
//         SegmentID segment_id = (*segment_ids_ptr)[0];
//
//         SegmentMeta segment_meta(segment_id, *table_meta);
//
//         size_t segment_row_cnt = 0;
//         std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
//         EXPECT_EQ(segment_row_cnt, 8);
//
//         std::vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
//
//         BlockID block_id = (*block_ids_ptr)[0];
//         BlockMeta block_meta(block_id, segment_meta);
//
//         NewTxnGetVisibleRangeState state;
//         status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//         EXPECT_TRUE(status.ok());
//         {
//             std::pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
//             bool has_next = state.Next(offset, range);
//             EXPECT_TRUE(has_next);
//             EXPECT_EQ(range.first, 0);
//             EXPECT_EQ(range.second, 8);
//             offset = range.second;
//             has_next = state.Next(offset, range);
//             EXPECT_FALSE(has_next);
//         }
//     }
// }
//
// TEST_P(TestTxnReplayAppend, test_replay_append_delete) {
//     using namespace infinity;
//
//     std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
//     auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
//     auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
//     auto table_name = std::make_shared<std::string>("tb1");
//     auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
//         Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//
//     auto input_block = std::make_shared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(column_def1->type());
//             col1->Initialize();
//             col1->AppendValue(Value::MakeInt(1));
//             col1->AppendValue(Value::MakeInt(2));
//             input_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(column_def2->type());
//             col2->Initialize();
//             col2->AppendValue(Value::MakeVarchar("abc"));
//             col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
//             input_block->InsertVector(col2, 1);
//         }
//         input_block->Finalize();
//     }
//     for (int i = 0; i < 2; ++i) {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
//
//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//
//     RestartTxnMgr();
//
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
//         std::vector<RowID> row_ids;
//         row_ids.push_back(RowID(0, 1));
//         row_ids.push_back(RowID(0, 3));
//         Status status = txn->Delete(*db_name, *table_name, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//
//     RestartTxnMgr();
//
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
//         auto begin_ts = txn->BeginTS();
//         auto commit_ts = txn->CommitTS();
//
//         std::shared_ptr<DBMeta> db_meta;
//         std::shared_ptr<TableMeta> table_meta;
//         TxnTimeStamp create_timestamp;
//         Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
//         EXPECT_TRUE(status.ok());
//
//         std::vector<SegmentID> *segment_ids_ptr{};
//         std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
//         SegmentID segment_id = (*segment_ids_ptr)[0];
//
//         SegmentMeta segment_meta(segment_id, *table_meta);
//
//         size_t segment_row_cnt = 0;
//         std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
//         EXPECT_EQ(segment_row_cnt, 4);
//
//         std::vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
//         BlockID block_id = (*block_ids_ptr)[0];
//
//         BlockMeta block_meta(block_id, segment_meta);
//
//         NewTxnGetVisibleRangeState state;
//         status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//         EXPECT_TRUE(status.ok());
//         {
//             std::pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
//             bool has_next = state.Next(offset, range);
//             EXPECT_TRUE(has_next);
//             EXPECT_EQ(range.first, 0);
//             EXPECT_EQ(range.second, 1);
//             offset = range.second;
//             has_next = state.Next(offset, range);
//             EXPECT_TRUE(has_next);
//             EXPECT_EQ(range.first, 2);
//             EXPECT_EQ(range.second, 3);
//             offset = range.second;
//             has_next = state.Next(offset, range);
//             EXPECT_FALSE(has_next);
//         }
//     }
// }
//
// TEST_P(TestTxnReplayAppend, test_replay_append_with_index0) {
//     using namespace infinity;
//
//     auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
//
//     // auto db_name = std::make_shared<std::string>("db1");
//     auto db_name = std::make_shared<std::string>("default_db");
//     auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 2);
//     auto column_def1 =
//         std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col1", std::set<ConstraintType>());
//     // auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
//     auto table_name = std::make_shared<std::string>("tb1");
//     // auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
//     auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});
//     auto index_name1 = std::make_shared<std::string>("index1");
//     //
//     std::vector<InitParameter *> parameters;
//     parameters.emplace_back(new InitParameter("metric", "l2"));
//     parameters.emplace_back(new InitParameter("m", "16"));
//     parameters.emplace_back(new InitParameter("ef_construction", "200"));
//     // parameters.emplace_back(new InitParameter("encode", "plain"));
//     parameters.emplace_back(new InitParameter("encode", "plain"));
//     //
//     auto index_def1 = IndexHnsw::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()}, parameters);
//     {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
//         Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
//                                            TransactionType::kCreateIndex);
//         Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     // create_index(index_def1);
//     create_index(index_def1);
//
//     auto append = [&](int i) {
//         auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
//         std::vector<float> v;
//         for (size_t i = 0; i < 2; ++i) {
//             v.push_back(static_cast<float>(i));
//         }
//         auto input_block = MakeInputBlock(Value::MakeEmbedding(v), 8192);
//         // auto input_block = MakeInputBlock(Value::MakeInt(i), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     for (int i = 0; i < 5; ++i) {
//         append(i);
//     }
//
//     new_txn_mgr_->PrintAllKeyValue();
//
//     RestartTxnMgr();
//
//     new_txn_mgr_->PrintAllKeyValue();
//
//     auto check_segment_index = [&](SegmentIndexMeta &segment_index_meta) {
//         auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0}));
//
//         ChunkIndexMeta chunk_index_meta{0, segment_index_meta};
//         IndexFileWorker *index_file_worker{};
//         chunk_index_meta.GetFileWorker(index_file_worker);
//         HnswHandler *hnsw_index{};
//         index_file_worker->Read(hnsw_index);
//         size_t row_cnt = hnsw_index->GetRowCount();
//         EXPECT_EQ(row_cnt, 8192 * 4);
//         // std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
//         // ASSERT_NE(mem_index, nullptr);
//         //
//         // // auto [row_id, row_cnt] = check_mem_index(mem_index);
//         // EXPECT_EQ(row_id, RowID(0, 0));
//         // EXPECT_EQ(row_cnt, 8192 * 2);
//     };
//
//     auto check_index = [&](const std::string &index_name) {
//         auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);
//
//         std::shared_ptr<DBMeta> db_meta;
//         std::shared_ptr<TableMeta> table_meta;
//         std::shared_ptr<TableIndexMeta> table_index_meta;
//         std::string table_key;
//         std::string index_key;
//         Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
//         EXPECT_TRUE(status.ok());
//
//         std::vector<SegmentID> *segment_ids_ptr = nullptr;
//         std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
//
//         for (SegmentID segment_id : *segment_ids_ptr) {
//             SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
//             check_segment_index(segment_index_meta);
//         }
//
//         status = new_txn_mgr_->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     check_index(*index_name1);
// }

TEST_P(TestTxnReplayAppend, some_test_index_already_exist) {
    using namespace infinity;

    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    // auto db_name = std::make_shared<std::string>("db1");
    auto db_name = std::make_shared<std::string>("default_db");
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 2);
    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col1", std::set<ConstraintType>());
    // auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    // auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});
    auto index_name1 = std::make_shared<std::string>("index1");
    //
    std::vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("m", "16"));
    parameters.emplace_back(new InitParameter("ef_construction", "200"));
    // parameters.emplace_back(new InitParameter("encode", "plain"));
    parameters.emplace_back(new InitParameter("encode", "plain"));
    //
    auto index_def1 = IndexHnsw::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()}, parameters);
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                           TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);

    auto check_segment_index = [&](SegmentIndexMeta &segment_index_meta, int num) {
        auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0}));

        ChunkIndexMeta chunk_index_meta{0, segment_index_meta};
        IndexFileWorker *index_file_worker{};
        chunk_index_meta.GetFileWorker(index_file_worker);
        HnswHandler *hnsw_index{};
        index_file_worker->Read(hnsw_index);
        size_t row_cnt = hnsw_index->GetRowCount();
        EXPECT_EQ(row_cnt, 8192 * num);
        // std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        // ASSERT_NE(mem_index, nullptr);
        //
        // // auto [row_id, row_cnt] = check_mem_index(mem_index);
        // EXPECT_EQ(row_id, RowID(0, 0));
        // EXPECT_EQ(row_cnt, 8192 * 2);
    };

    auto check_index = [&](const std::string &index_name, int num) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        std::vector<SegmentID> *segment_ids_ptr{};
        std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));

        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            check_segment_index(segment_index_meta, num);
        }

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto append = [&](int i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        std::vector<float> v;
        for (size_t i = 0; i < 2; ++i) {
            v.push_back(static_cast<float>(i));
        }
        auto input_block = MakeInputBlock(Value::MakeEmbedding(v), 8192);
        // auto input_block = MakeInputBlock(Value::MakeInt(i), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    for (int i = 0; i < 2; ++i) {
        append(i);
    }

    // new_txn_mgr_->PrintAllKeyValue();

    RestartTxnMgr(); // maybe there checkpoint
    // new_txn_mgr_->PrintAllKeyValue();

    check_index(*index_name1, 2);

    RestartTxnMgr();

    new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    for (int i = 0; i < 2; ++i) {
        append(i); // Becauce Restart occurs checkpoint, so now the chunk_is in rocksdb is 1 rather then 0.
        // If it is 1, you will not get the original information
    }

    check_index(*index_name1, 4);
}

// append    restart                      another_append_opperation
// |         |                            |
// chunk_0   chumk_0(but not happed?)     chunk_0(be removed)

TEST_P(TestTxnReplayAppend, some_test_index_dont_exist) {
    using namespace infinity;

    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    // auto db_name = std::make_shared<std::string>("db1");
    auto db_name = std::make_shared<std::string>("default_db");
    auto column3_type_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 2);
    auto column_def1 =
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kEmbedding, column3_type_info), "col1", std::set<ConstraintType>());
    // auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    // auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});
    auto index_name1 = std::make_shared<std::string>("index1");
    //
    std::vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("m", "16"));
    parameters.emplace_back(new InitParameter("ef_construction", "200"));
    // parameters.emplace_back(new InitParameter("encode", "plain"));
    parameters.emplace_back(new InitParameter("encode", "plain"));
    //
    auto index_def1 = IndexHnsw::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()}, parameters);
    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)),
                                           TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    // create_index(index_def1);

    auto check_segment_index = [&](SegmentIndexMeta &segment_index_meta, int num) {
        auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0}));

        ChunkIndexMeta chunk_index_meta{0, segment_index_meta};
        IndexFileWorker *index_file_worker{};
        chunk_index_meta.GetFileWorker(index_file_worker);
        HnswHandler *hnsw_index{};
        index_file_worker->Read(hnsw_index);
        size_t row_cnt = hnsw_index->GetRowCount();
        EXPECT_EQ(row_cnt, 8192 * num);
        // std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        // ASSERT_NE(mem_index, nullptr);
        //
        // // auto [row_id, row_cnt] = check_mem_index(mem_index);
        // EXPECT_EQ(row_id, RowID(0, 0));
        // EXPECT_EQ(row_cnt, 8192 * 2);
    };

    auto check_index = [&](const std::string &index_name, int num) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        std::vector<SegmentID> *segment_ids_ptr{};
        std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));

        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
            check_segment_index(segment_index_meta, num);
        }

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto append = [&](int i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        std::vector<float> v;
        for (size_t i = 0; i < 2; ++i) {
            v.push_back(static_cast<float>(i));
        }
        auto input_block = MakeInputBlock(Value::MakeEmbedding(v), 8192);
        // auto input_block = MakeInputBlock(Value::MakeInt(i), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    for (int i = 0; i < 2; ++i) {
        append(i);
    }
    create_index(index_def1);

    new_txn_mgr_->PrintAllKeyValue();

    RestartTxnMgr();

    new_txn_mgr_->PrintAllKeyValue();

    check_index(*index_name1, 2);

    RestartTxnMgr();

    new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    for (int i = 0; i < 2; ++i) {
        append(i);
    }
}
