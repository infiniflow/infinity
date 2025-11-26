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

module infinity_core:ut.dump_index_task;

import :ut.base_test;
import :storage;
import :infinity_context;
import :status;
import :buffer_manager;
import :column_vector;
import :table_def;
import :value;
import :physical_import;
import :default_values;
import :infinity_exception;
import :base_table_ref;
import :compaction_process;
import :logger;
import :txn_state;
import :data_block;
import :new_txn_manager;
import :new_txn;
import :db_meta;
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :index_secondary;

import third_party;
import global_resource_usage;
import logical_type;
import internal_types;
import extra_ddl_info;
import column_def;
import data_type;

using namespace infinity;

class DumpMemIndexTaskTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         DumpMemIndexTaskTest,
                         ::testing::Values(BaseTestParamStr::NEW_BG_ON_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_BG_ON_CONFIG_PATH));

TEST_P(DumpMemIndexTaskTest, row_cnt_exceed_memindex_capacity) {
    auto *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_name1 = std::make_shared<std::string>("idx1");
    auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        auto status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
        auto status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    // Mem index capacity is set to 8192 rows in toml
    i64 mem_index_capacity = InfinityContext::instance().storage()->config()->MemIndexCapacity();
    LOG_INFO(fmt::format("mem_index_capacity: {}", mem_index_capacity));

    // Append 8192 rows to exceed the mem index capacity
    append();

    // Wait for the mem index dump in background to finish
    sleep(2);

    // Check chunk index after first mem index dump
    {
        auto *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        auto status = txn->GetTableIndexMeta(*db_name, *table_name, *index_name1, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
            segment_id = (*segment_ids_ptr)[0];
        }

        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
        ChunkID chunk_id = 0;
        {
            std::vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0}));
            chunk_id = (*chunk_ids_ptr)[0];
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        {
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 0));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, 8192);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Second time: append 8192 rows to exceed the mem index capacity
    append();

    // Wait for the mem index dump in background to finish
    sleep(2);

    // Check chunk index after second mem index dump
    {
        auto *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        std::shared_ptr<TableIndexMeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        auto status = txn->GetTableIndexMeta(*db_name, *table_name, *index_name1, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
            segment_id = (*segment_ids_ptr)[0];
        }

        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
        ChunkID chunk_id = 0;
        {
            std::vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0, 1}));
            chunk_id = (*chunk_ids_ptr)[1];
        }
        {
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 8192));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, 8192);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}
