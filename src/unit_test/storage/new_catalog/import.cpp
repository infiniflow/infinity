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
import column_def;
import data_type;
import logical_type;
import table_def;
import index_base;
import index_secondary;
import index_full_text;
import internal_types;
import data_block;
import column_vector;
import value;
import new_txn;
import buffer_obj;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import constant_expr;
import create_index_info;

using namespace infinity;

class TestTxnImport : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnImport,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnImport, test_import1) {

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

    // Import two segments, each segments contains two blocks
    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
    }
}

TEST_P(TestTxnImport, test_import_with_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
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
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

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

    // Import two segments, each segments contains two blocks
    for (SizeT i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto check_index = [&](const String &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check index1"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Optional<TableIndexMeeta> table_index_meta;
        String table_key;
        String index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        auto check_segment = [&](SegmentID segment_id) {
            SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

            {
                Vector<ChunkID> *chunk_ids = nullptr;
                std::tie(chunk_ids, status) = segment_index_meta.GetChunkIDs1();
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(*chunk_ids, Vector<ChunkID>({0}));
            }
            ChunkID chunk_id = 0;
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            {
                ChunkIndexMetaInfo *chunk_info = nullptr;
                Status status = chunk_index_meta.GetChunkInfo(chunk_info);
                EXPECT_TRUE(status.ok());
                EXPECT_EQ(chunk_info->row_cnt_, block_row_cnt * 2);
                EXPECT_EQ(chunk_info->base_row_id_, RowID(segment_id, 0));
            }

            BufferObj *buffer_obj = nullptr;
            status = chunk_index_meta.GetIndexBuffer(buffer_obj);
            EXPECT_TRUE(status.ok());
        };
        {
            auto [segment_ids, status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

            for (auto segment_id : *segment_ids) {
                check_segment(segment_id);
            }
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    check_index(*index_name1);
    check_index(*index_name2);
}

TEST_P(TestTxnImport, test_import_with_index_rollback) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
    auto index_name2 = std::make_shared<String>("index2");
    auto index_def2 = IndexFullText::Make(index_name2, MakeShared<String>(), "file_name", {column_def2->name()}, {});
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
    auto create_index = [&](const SharedPtr<IndexBase> &index_base) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

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

    {
        auto *txn_import = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
        Status status = txn_import->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn_import);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn_import = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block()};
        Status status = txn_import->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn_drop_db = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn_drop_db->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn_drop_db);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn_import);
        EXPECT_FALSE(status.ok());
    }
}

TEST_P(TestTxnImport, test_insert_and_import) {
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

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
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

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1, 2}));

        {
            SegmentMeta segment_meta(0, *table_meta);
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
        }
        {
            SegmentMeta segment_meta(1, *table_meta);
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
        }
        {
            SegmentMeta segment_meta(2, *table_meta);
            auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_drop_db) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };
    auto check_table_2segments = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };
    auto check_table_1segment = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        SegmentID segment_id = 0;
        SegmentMeta segment_meta(segment_id, table_meta);
        check_segment(segment_meta, txn);
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop db   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_2segments(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_drop_table) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };
    auto check_table_2segments = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };
    auto check_table_1segment = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        SegmentID segment_id = 0;
        SegmentMeta segment_meta(segment_id, table_meta);
        check_segment(segment_meta, txn);
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_2segments(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the imported data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  drop table    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop table   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_add_columns) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };
    auto check_table_2segments = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };
    auto check_table_1segment = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        SegmentID segment_id = 0;
        SegmentMeta segment_meta(segment_id, table_meta);
        check_segment(segment_meta, txn);
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_2segments(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add columns    commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  add columns    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        // Add two columns
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import                                           commit (success)
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns     commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn6->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_drop_columns) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();
        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };
    auto check_table_2segments = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };
    auto check_table_1segment = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));

        SegmentID segment_id = 0;
        SegmentMeta segment_meta(segment_id, table_meta);
        check_segment(segment_meta, txn);
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        // drop column
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_2segments(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop columns    commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|--------------------|
    //                           t2                  drop column     commit (fail)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table_1segment(*table_meta, txn5);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                                     commit (fail)
    //    |----------|--------------------------------------------------------------|
    //                            |----------------------|-------------------|
    //                           t2                  drop column   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (fail)
    //    |----------|---------------------------------------------------|
    //          |----------------------|-----------------------|
    //         t2                  drop columns        commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (fail)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------------------|
    //         t2                  drop columns          commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (fail)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop columns   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // Drop columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (fail)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  drop columns  commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add columns"), TransactionType::kNormal);
        // Add two columns
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import                                           commit (success)
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  add columns     commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Add two columns
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop columns"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import) {
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
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
            }
            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
                EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            }
        };

        auto check_segment = [&](SegmentMeta &segment_meta) {
            auto [block_ids, status] = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        auto check_table = [&](TableMeeta &table_meta) {
            auto [segment_ids, seg_status] = table_meta.GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0, 1}));

            for (SegmentID segment_id : *segment_ids) {
                SegmentMeta segment_meta(segment_id, table_meta);
                check_segment(segment_meta);
            }
        };

        check_table(*table_meta);
    }
}

TEST_P(TestTxnImport, test_import_append_table) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };

    auto check_table = [&](TableMeeta &table_meta, NewTxn *txn, const Vector<SegmentID> &segment_ids) {
        auto [segment_ids_ptr, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };

    auto input_block1 = MakeShared<DataBlock>();
    {
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        for (SizeT i = 0; i < 2; ++i) {
            auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
            status = txn3->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn3);
            EXPECT_TRUE(status.ok());
        }

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 1"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1, 2});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  append   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  append    commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit (success)
    //                |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, input_block1);
        //        status = txn6->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                           t1      import                                          commit (success)
    //                           |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                     t1      import                                          commit (success)
    //                                     |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  append      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                                               t1      import (fail)                                         rollback
    //                                               |----------|---------------------------------------------------|
    //          |----------------------|----------|
    //         t2                  dropdb      commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_import_table) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };

    auto check_table = [&](TableMeeta &table_meta, NewTxn *txn, Vector<SegmentID> segment_ids) {
        new_txn_mgr->PrintAllKeyValue();

        auto [seg_ids, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*seg_ids, segment_ids);

        for (auto segment_id : segment_ids) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }
    };

    auto input_block1 = MakeShared<DataBlock>();
    {
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  import   commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                          commit (success)
    //    |----------|--------------------------------|
    //                            |----------------------|---------------|
    //                           t2                  import          commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                               commit (success)
    //    |----------|-----------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  import       commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                               commit (success)
    //    |----------|--------------------------------------------------------|
    //                            |----------------------|---------------|
    //                           t2                  import           commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                          commit (success)
    //    |----------|---------------------------------------------------|
    //          |----------------------|---------------|
    //         t2                  import           commit (success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }

    //                t1      import                                          commit(success)
    //                |----------|---------------------------------------------------|
    //          |----------------------|--------------|
    //         t2                  import         commit(success)
    {
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent import"), TransactionType::kNormal);

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks2 = {make_input_block(), make_input_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        check_table(*table_meta, txn5, {0, 1});

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn8->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn8);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_rename) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  rename    commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  rename   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import two segments, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  rename table         commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                   commit (success)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        rename table (success) commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      import                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                rename                 commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   import                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column  commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  import                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  rename   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  import(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|----------|
    //                    t2           drop column   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_and_create_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };

    auto check_table = [&](TableMeeta &table_meta, NewTxn *txn, const Vector<SegmentID> &segment_ids) {
        auto [segment_ids_ptr, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        Status status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->at(0), "idx1");

        TableIndexMeeta table_index_meta(index_id_strs_ptr->at(0), table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        EXPECT_TRUE(index_status.ok());
        EXPECT_EQ(*index_base->index_name_, String("idx1"));
        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_segment_ids_ptr->size(), 1);
    };
    auto check_no_index = [&](TableMeeta &table_meta, NewTxn *txn, const Vector<SegmentID> &segment_ids) {
        auto [segment_ids_ptr, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        Status status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_id_strs_ptr, Vector<String>({}));
    };
    auto check_no_data = [&](TableMeeta &table_meta, NewTxn *txn) {
        auto [segment_ids_ptr, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({}));

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        Status status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_names_ptr, Vector<String>({"idx1"}));
    };

    auto input_block1 = MakeShared<DataBlock>();
    {
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  create index  commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             create index     commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  create index         commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_no_index(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        create index (success)    commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_no_data(*table_meta, txn5);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      import                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                create index                 commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_no_data(*table_meta, txn5);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   import                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  create index  commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_no_data(*table_meta, txn5);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  import                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  create index   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // Import 1 segment1, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_no_data(*table_meta, txn5);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  import                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           create index   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_and_drop_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    u32 block_row_cnt = 8192;
    auto append_to_col = [&](ColumnVector &col, Value v1, Value v2) {
        for (u32 i = 0; i < block_row_cnt; i += 2) {
            col.AppendValue(v1);
            col.AppendValue(v2);
        }
    };

    auto make_input_block = [&] {
        auto input_block = MakeShared<DataBlock>();

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

    auto check_block = [&](BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
        NewTxnGetVisibleRangeState state;
        Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
        }
        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            EXPECT_EQ(col.GetValueByIndex(0), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeVarchar("abc"));
            EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    };

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(*block_ids, Vector<BlockID>({0, 1}));

        for (auto block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            check_block(block_meta, begin_ts, commit_ts);
        }
    };

    auto check_table = [&](TableMeeta &table_meta, NewTxn *txn, const Vector<SegmentID> &segment_ids) {
        auto [segment_ids_ptr, seg_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);

        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            check_segment(segment_meta, txn);
        }

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        Status status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 0);
        EXPECT_EQ(index_names_ptr->size(), 0);
        //        EXPECT_EQ(index_names_ptr->at(0), "idx1");
        //
        //        TableIndexMeeta table_index_meta(index_id_strs_ptr->at(0), table_meta);
        //        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        //        EXPECT_TRUE(index_status.ok());
        //        EXPECT_EQ(*index_base->index_name_, String("idx1"));
        //        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);
        //
        //        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        //        status = table_index_meta.GetSegmentIndexIDs1(index_segment_ids_ptr);
        //        EXPECT_TRUE(status.ok());
        //        EXPECT_EQ(index_segment_ids_ptr->size(), 1);
    };

    auto input_block1 = MakeShared<DataBlock>();
    {
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            append_to_col(*col1, Value::MakeInt(1), Value::MakeInt(2));
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            append_to_col(*col2, Value::MakeVarchar("abc"), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop index  commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             drop index     commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  drop index         commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      import                                   commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        drop index (success)    commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      import                                    commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                drop index                 commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   import                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  drop index  commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  import                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  drop index   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  import                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           drop index   commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn4->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn7->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(), make_input_block()};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Scan and check
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        check_table(*table_meta, txn5, {0});
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnImport, test_import_and_compact) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, const Value &v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
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

    auto PrepareForCompact = [&] {
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
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For compact
        for (int i = 0; i < 2; ++i) {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import {}", i)), TransactionType::kNormal);
            Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
            Status status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const Vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, segment_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  compact  commit (success)
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1, 2});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //    t1      import      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             compact (fail)    rollback
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        //        new_txn_mgr->PrintAllKeyValue();

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1      import                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  compact                  commit(success)
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1      import                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        compact              commit (success)
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1                                      import                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                compact                     commit (success)
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1                                                   import                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  compact commit (success)
    {
        PrepareForCompact();

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //                                                  t1                  import                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  drop index   commit (success)
    {
        PrepareForCompact();

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //                                                           t1                  import                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           drop index   commit (success)
    {
        PrepareForCompact();

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment, each segments contains two blocks
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks1 = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz")),
                                                      make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn3->Import(*db_name, *table_name, input_blocks1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }
}

TEST_P(TestTxnImport, test_import_and_optimize_index) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = MakeShared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, const Value &v) {
            for (u32 i = 0; i < block_row_cnt; ++i) {
                col.AppendValue(v);
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

    auto PrepareForCompactAndOptimize = [&] {
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
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
            Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For optimize
        for (int i = 0; i < 2; ++i) {
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("append {}", i)), TransactionType::kNormal);
                SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
                Status status = txn->Append(*db_name, *table_name, input_block);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }

            //            new_txn_mgr->PrintAllKeyValue();
            {
                auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("dump index {}", i)), TransactionType::kNormal);
                Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
            SharedPtr<DataBlock> input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](Vector<ColumnID> column_idxes) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> expected_segments{0, 1};
        EXPECT_EQ(*segment_ids_ptr, expected_segments);

        Optional<TableIndexMeeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0, 1}));

        {
            SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
            Vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({2}));
        }

        {
            SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[1], *table_index_meta);
            Vector<ChunkID> *chunk_ids_ptr = nullptr;
            std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({0}));
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      import      commit (success)
    //    |----------|---------|
    //                            |----------------------|---------------|
    //                           t2                  optimize index  commit (success)
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    /* FIXME: PostRollback() for dump index is not implemented.
    //    t1      import      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2            optimize index     commit
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //    t1      import                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  optimize index          commit (success)
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //    t1      import                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize index         commit (success)
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //    t1                                      import                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                optimize index                  commit (success)
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //    t1                                                   import                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |--------------------|------------|
    //                    t2              optimize index   commit (success)
    {
        PrepareForCompactAndOptimize();

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }

    //                                                  t1                  import                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  optimize index   commit (success)
    {
        PrepareForCompactAndOptimize();

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }
    */

    //                                                           t1                  import                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           optimize index   commit (success)
    {
        PrepareForCompactAndOptimize();

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // Import 1 segment with 1 block
        auto *txn1 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("import")), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        status = txn1->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        CheckTable({3});

        DropDB();
    }
}
