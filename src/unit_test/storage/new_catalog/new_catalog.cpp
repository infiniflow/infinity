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
import infinity_exception;
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_ivf;
import index_full_text;
import index_hnsw;
import embedding_info;
import sparse_info;
import index_bmp;
import internal_types;
import defer_op;
import statement_common;
import meta_info;
import data_block;
import column_vector;
import value;
import data_access_state;
import kv_code;
import kv_store;
import new_txn;
import new_txn_store;
import buffer_obj;
import buffer_handle;
import secondary_index_in_mem;
import secondary_index_data;
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
import roaring_bitmap;
import index_filter_evaluators;
import index_emvb;
import constant_expr;

using namespace infinity;

class NewCatalogTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         NewCatalogTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(NewCatalogTest, test_block_lock) {
    using namespace infinity;

    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    String db_id_str = "0";
    String table_id_str = "0";
    SegmentID segment_id = 0;
    BlockID block_id = 0;
    String block_key1 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
    String block_key2 = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id + 1, "lock");

    Status status = new_catalog->AddBlockLock(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->AddBlockLock(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
        EXPECT_TRUE(status.ok());
    }
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_TRUE(status.ok());
    status = new_catalog->DropBlockLockByBlockKey(block_key1);
    EXPECT_FALSE(status.ok());
    {
        std::shared_ptr<BlockLock> block_lock;
        status = new_catalog->GetBlockLock(block_key1, block_lock);
        EXPECT_FALSE(status.ok());
    }
}

TEST_P(NewCatalogTest, test_insert_and_import) {
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
    auto make_input_block = [&](Value v1, Value v2) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(std::move(v));
            }
        };
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(3), Value::MakeVarchar("xyz"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import2"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(4), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(5), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
        };
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append2"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(6), Value::MakeVarchar("xyz"));
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1, 2}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({3}));

        SegmentMeta segment_meta(3, *table_meta, table_meta->kv_instance());
        auto [block_ids, block_status] = segment_meta.GetBlockIDs();
        EXPECT_TRUE(block_status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1, 2, 3, 4, 5}));

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

// TEST_P(NewCatalogTest, test_cleanup) {
//     using namespace infinity;

//     NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

//     SharedPtr<String> db_name = std::make_shared<String>("db1");
//     auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
//     auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
//     auto table_name = std::make_shared<std::string>("tb1");
//     auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

//     auto index_name1 = std::make_shared<std::string>("index1");
//     auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
//     auto index_name2 = std::make_shared<String>("index2");
//     auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//         Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//         Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }

//     auto input_block = MakeShared<DataBlock>();
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
//     auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
//         Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     create_index(index_def1);
//     create_index(index_def2);
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     new_txn_mgr->PrintAllKeyValue();
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index1"), TransactionType::kNormal);
//         Status status = txn->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
//         EXPECT_TRUE(status.ok());
//     }
//     new_txn_mgr->PrintAllKeyValue();
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
//         Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
//         EXPECT_TRUE(status.ok());
//     }
//     new_txn_mgr->PrintAllKeyValue();
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//         Status status = txn->DropDatabase(*db_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         Status status = new_txn_mgr->Cleanup(new_txn_mgr->max_committed_ts() + 1);
//         EXPECT_TRUE(status.ok());
//     }
//     new_txn_mgr->PrintAllKeyValue();
// }

// TEST_P(NewCatalogTest, test_checkpoint) {
//     using namespace infinity;

//     SharedPtr<String> db_name = std::make_shared<String>("db1");
//     auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
//     auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
//     auto table_name = std::make_shared<std::string>("tb1");
//     auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

//     auto index_name1 = std::make_shared<std::string>("index1");
//     auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

//     NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//         Status status = txn->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//         Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }

//     u32 block_row_cnt = 8192;
//     auto make_input_block = [&](const Value &v1, const Value &v2) {
//         auto input_block = MakeShared<DataBlock>();
//         // Initialize input block
//         auto append_column = [&](const SharedPtr<ColumnDef> &column_def, const Value &v) {
//             auto col = ColumnVector::Make(column_def->type());
//             col->Initialize();
//             for (u32 i = 0; i < block_row_cnt; ++i) {
//                 col->AppendValue(v);
//             }
//             input_block->InsertVector(col, input_block->column_count());
//         };
//         append_column(column_def1, v1);
//         append_column(column_def2, v2);
//         input_block->Finalize();
//         return input_block;
//     };
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

//         auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc"));
//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     [[maybe_unused]] auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
//         Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     create_index(index_def1);
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

//         auto input_block = make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
//         Status status = txn->Append(*db_name, *table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }

//     [[maybe_unused]] auto check_index = [&](const String &index_name, std::function<Pair<RowID, u32>(const SharedPtr<MemIndex> &)> check_mem_index) {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("check index {}", index_name)), TransactionType::kNormal);

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Optional<TableIndexMeeta> table_index_meta;
//         Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta, table_index_meta->kv_instance());

//         Vector<ChunkID> *chunk_ids_ptr = nullptr;
//         status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({0}));

//         ChunkID chunk_id = 0;
//         ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, *txn->kv_instance());

//         BufferObj *buffer_obj = nullptr;
//         status = chunk_index_meta.GetIndexBuffer(buffer_obj);
//         EXPECT_TRUE(status.ok());

//         {
//             BufferHandle buffer_handle = buffer_obj->Load();
//             [[maybe_unused]] const auto *index = reinterpret_cast<const SecondaryIndexData *>(buffer_handle.GetData());
//         }

//         SharedPtr<MemIndex> mem_index;
//         status = segment_index_meta.GetMemIndex(mem_index);
//         EXPECT_TRUE(status.ok());
//         {
//             auto [row_id, row_cnt] = check_mem_index(mem_index);
//             EXPECT_EQ(row_id, RowID(0, block_row_cnt));
//             EXPECT_EQ(row_cnt, block_row_cnt);
//         }

//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     auto check_table = [&] {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         Status status;

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         SegmentMeta segment_meta(segment_id, *table_meta, table_meta->kv_instance());

//         auto check_block = [&](BlockID block_id, const Value &v1, const Value &v2) {
//             BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());

//             SizeT row_count = 0;
//             // std::tie(row_count, status) = block_meta.GetRowCnt();
//             std::tie(row_count, status) = block_meta.GetRowCnt1(begin_ts);
//             EXPECT_TRUE(status.ok());
//             EXPECT_EQ(row_count, block_row_cnt);

//             auto check_column = [&](ColumnID column_id, const Value &v) {
//                 ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());
//                 ColumnVector col1;
//                 status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorTipe::kReadOnly, col1);
//                 EXPECT_TRUE(status.ok());

//                 for (u32 i = 0; i < row_count; ++i) {
//                     EXPECT_EQ(col1.GetValue(i), v);
//                 }
//             };

//             check_column(0, v1);
//             check_column(1, v2);
//         };
//         check_block(0, Value::MakeInt(1), Value::MakeVarchar("abc"));
//         check_block(1, Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

//         // check_index(*index_name1, [&](const SharedPtr<MemIndex> &mem_index) {
//         //     RowID begin_id = mem_index->memory_secondary_index_->GetBeginRowID();
//         //     u32 row_cnt = mem_index->memory_secondary_index_->GetRowCount();
//         //     return std::make_pair(begin_id, row_cnt);
//         // });

//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };
//     // check_table();

//     // new_txn_mgr->PrintAllKeyValue();
//     // infinity::InfinityContext::instance().UnInit();

//     // InfinityContext::instance().InitPhase1(this->config_path);
//     // InfinityContext::instance().InitPhase2();
//     // new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

//     // new_txn_mgr->PrintAllKeyValue();
//     check_table();
//     {
//         auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("checkpoint"), TransactionType::kNormal);
//         CheckpointOption option;
//         Status status = txn->Checkpoint(option);
//         EXPECT_TRUE(status.ok());
//         status = new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }
//     new_txn_mgr->PrintAllKeyValue();
//     infinity::InfinityContext::instance().UnInit();

//     InfinityContext::instance().InitPhase1(this->config_path);
//     InfinityContext::instance().InitPhase2();
//     new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

//     new_txn_mgr->PrintAllKeyValue();
//     check_table();
// }
