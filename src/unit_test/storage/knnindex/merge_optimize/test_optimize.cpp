// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

import compilation_config;
import stl;
import logger;
import third_party;
import global_resource_usage;
import storage;
import infinity_context;
import txn_manager;
import txn;
import extra_ddl_info;
import table_def;
import column_def;
import data_type;
import logical_type;
import column_vector;
import data_block;
import index_hnsw;
import index_secondary;
import statement_common;
import embedding_info;
import knn_expr;
import catalog;
import infinity_exception;
import bg_task;
import txn_store;
import wal_manager;
import buffer_manager;
import internal_types;
import background_process;
import txn_state;
import new_txn_manager;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import catalog_meta;
import mem_index;
import status;
import new_txn;
import abstract_hnsw;
import buffer_obj;

using namespace infinity;

class OptimizeKnnTest : public BaseTestParamStr {
protected:
    void WaitCleanup(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        Status status = new_txn_mgr->Cleanup();
        EXPECT_TRUE(status.ok());
    }

    void WaitCheckpoint(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         OptimizeKnnTest,
                         ::testing::Values((std::string(test_data_path()) + "/config/test_optimize.toml").c_str(),
                                           (std::string(test_data_path()) + "/config/test_optimize_vfs_off.toml").c_str()));

TEST_P(OptimizeKnnTest, test_hnsw_optimize) {
    Storage *storage = InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = std::make_shared<std::string>("default_db");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 =
        std::make_shared<ColumnDef>(1,
                                    std::make_shared<DataType>(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 4)),
                                    "col2",
                                    std::set<ConstraintType>());

    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name = std::make_shared<std::string>("idx1");

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        txn->CreateTable(*db_name, table_def, ConflictType::kError);

        txn_mgr->CommitTxn(txn);
    }

    {
        Vector<String> column_names{"col2"};
        const String &file_name = "idx_file.idx";
        Vector<UniquePtr<InitParameter>> index_param_list; //
        Vector<InitParameter *> index_param_list_ptr;
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"metric", "l2"}));
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"encode", "plain"}));
        for (auto &param : index_param_list) {
            index_param_list_ptr.push_back(param.get());
        }
        // index_param_list

        auto index_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), file_name, column_names, index_param_list_ptr);
        // create index idx1
        auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_hnsw, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    auto DoAppend = [&]() {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
        Vector<SharedPtr<ColumnVector>> column_vectors;
        for (SizeT i = 0; i < table_def->columns().size(); ++i) {
            SharedPtr<DataType> data_type = table_def->columns()[i]->type();
            column_vectors.push_back(MakeShared<ColumnVector>(data_type));
            column_vectors.back()->Initialize();
        }
        Vector<int> col1{2, 4, 6, 8};
        Vector<Vector<float>> col2{{0.1, 0.2, 0.3, -0.2}, {0.2, 0.1, 0.3, 0.4}, {0.3, 0.2, 0.1, 0.4}, {0.4, 0.3, 0.2, 0.1}};
        SizeT row_cnt = 4;
        for (SizeT i = 0; i < row_cnt; ++i) {
            column_vectors[0]->AppendByPtr(reinterpret_cast<const char *>(&col1[i]));
            column_vectors[1]->AppendByPtr(reinterpret_cast<const char *>(col2[i].data()));
        }
        auto data_block = DataBlock::Make();
        data_block->Init(column_vectors);

        Status status = txn->Append(*db_name, *table_name, data_block);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    };

    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 2; ++i) {
            DoAppend();
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("dump mem index"), TransactionType::kNormal);

            Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name, 0);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("merge index {}", *index_name)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, *index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    WaitCheckpoint(storage);
    WaitCleanup(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(mem_index->memory_hnsw_index_, nullptr);
        txn_mgr->PrintAllKeyValue();
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({3}));
        }
        ChunkID chunk_id = 3;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 24);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(OptimizeKnnTest, test_secondary_index_optimize) {
    Storage *storage = InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1});
    auto index_name = std::make_shared<std::string>("idx1");

    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        txn->CreateTable(*db_name, table_def, ConflictType::kError);
        txn_mgr->CommitTxn(txn);
    }

    {
        Vector<String> column_names{"col1"};
        const String &file_name = "idx_file.idx";
        auto index_secondary = IndexSecondary::Make(index_name, MakeShared<String>("test comment"), file_name, column_names);
        // create index idx1
        auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_secondary, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    auto DoAppend = [&]() {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("insert table"), TransactionType::kNormal);
        Vector<SharedPtr<ColumnVector>> column_vectors;
        for (SizeT i = 0; i < table_def->columns().size(); ++i) {
            SharedPtr<DataType> data_type = table_def->columns()[i]->type();
            column_vectors.push_back(MakeShared<ColumnVector>(data_type));
            column_vectors.back()->Initialize();
        }
        Vector<int> col1{2, 4, 6, 8};
        SizeT row_cnt = 4;
        for (SizeT i = 0; i < row_cnt; ++i) {
            column_vectors[0]->AppendByPtr(reinterpret_cast<const char *>(&col1[i]));
        }
        auto data_block = DataBlock::Make();
        data_block->Init(column_vectors);

        Status status = txn->Append(*db_name, *table_name, data_block);
        EXPECT_TRUE(status.ok());
        txn_mgr->CommitTxn(txn);
    };

    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 2; ++i) {
            DoAppend();
        }
        {
            auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("dump mem index"), TransactionType::kNormal);

            Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name, 0);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("merge index {}", *index_name)), TransactionType::kNormal);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, *index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    WaitCheckpoint(storage);
    WaitCleanup(storage);
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index;
        status = segment_index_meta.GetMemIndex(mem_index);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(mem_index->memory_secondary_index_, nullptr);
        txn_mgr->PrintAllKeyValue();
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, Vector<ChunkID>({3}));
        }
        ChunkID chunk_id = 3;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info->row_cnt_, 24);
            EXPECT_EQ(chunk_info->base_row_id_, RowID(0, 0));
        }

        BufferObj *buffer_obj = nullptr;
        status = chunk_index_meta.GetIndexBuffer(buffer_obj);
        EXPECT_TRUE(status.ok());

        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
