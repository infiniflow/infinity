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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_optimize;

import :ut.base_test;
import :logger;
import third_party;
import :infinity_context;
import :table_def;
import :column_vector;
import :data_block;
import :index_hnsw;
import :index_secondary;
import :infinity_exception;
import :bg_task;
import :wal_manager;
import :buffer_manager;
import :background_process;
import :txn_state;
import :new_txn_manager;
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
import :status;
import :new_txn;
import :hnsw_handler;
import :buffer_obj;
import :storage;

import compilation_config;
import global_resource_usage;
import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import statement_common;
import embedding_info;
import knn_expr;
import internal_types;

using namespace infinity;

class OptimizeKnnTest : public BaseTestParamStr {
protected:
    void WaitCleanup(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
        Status status = txn->Cleanup();
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void WaitCheckpoint(Storage *storage) {
        NewTxnManager *new_txn_mgr = storage->new_txn_manager();
        WalManager *wal_manager = storage->wal_manager();
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        Status status = txn2->Checkpoint(wal_manager->LastCheckpointTS(), false);
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
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    auto index_name = std::make_shared<std::string>("idx1");

    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        txn->CreateTable(*db_name, table_def, ConflictType::kError);

        txn_mgr->CommitTxn(txn);
    }

    {
        std::vector<std::string> column_names{"col2"};
        const std::string &file_name = "idx_file.idx";
        std::vector<std::unique_ptr<InitParameter>> index_param_list; //
        std::vector<InitParameter *> index_param_list_ptr;
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"metric", "l2"}));
        index_param_list.push_back(std::make_unique<InitParameter>(InitParameter{"encode", "plain"}));
        for (auto &param : index_param_list) {
            index_param_list_ptr.push_back(param.get());
        }
        // index_param_list

        auto index_hnsw = IndexHnsw::Make(index_name, std::make_shared<std::string>("test comment"), file_name, column_names, index_param_list_ptr);
        // create index idx1
        auto *txn3 = txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn3->CreateIndex(*db_name, *table_name, index_hnsw, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());
    }

    auto DoAppend = [&]() {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("insert table"), TransactionType::kAppend);
        std::vector<std::shared_ptr<ColumnVector>> column_vectors;
        for (size_t i = 0; i < table_def->columns().size(); ++i) {
            std::shared_ptr<DataType> data_type = table_def->columns()[i]->type();
            column_vectors.push_back(std::make_shared<ColumnVector>(data_type));
            column_vectors.back()->Initialize();
        }
        std::vector<int> col1{2, 4, 6, 8};
        std::vector<std::vector<float>> col2{{0.1, 0.2, 0.3, -0.2}, {0.2, 0.1, 0.3, 0.4}, {0.3, 0.2, 0.1, 0.4}, {0.4, 0.3, 0.2, 0.1}};
        size_t row_cnt = 4;
        for (size_t i = 0; i < row_cnt; ++i) {
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
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("dump mem index"), TransactionType::kDumpMemIndex);

            Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name, 0);
            EXPECT_TRUE(status.ok());

            txn_mgr->CommitTxn(txn);
        }
    }
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("merge index {}", *index_name)), TransactionType::kOptimizeIndex);
        SegmentID segment_id = 0;
        Status status = txn->OptimizeIndex(*db_name, *table_name, *index_name, segment_id);
        EXPECT_TRUE(status.ok());
        status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    WaitCheckpoint(storage);
    WaitCleanup(storage);
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("check index1"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, *index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        }
        SegmentID segment_id = 0;
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        ASSERT_NE(mem_index, nullptr);
        EXPECT_EQ(mem_index->GetSecondaryIndex(), nullptr);
        txn_mgr->PrintAllKeyValue();
        {
            auto [chunk_ids, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids, std::vector<ChunkID>({3}));
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
