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

module infinity_core:ut.new_txn_replay_with_ckp;

import :ut.base_test;
import :ut.replay_test;

import third_party;
import :status;
import :new_txn_manager;
import :new_txn;
import :table_def;
import :index_base;
import :infinity_context;
import :txn_state;
import :data_block;
import :column_vector;
import :value;
import :new_catalog;
import :db_meta;
import :table_meta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_index_meta;
import :segment_index_meta;
import :chunk_index_meta;
import :catalog_meta;
import :index_secondary;
import :index_ivf;
import :index_full_text;
import :index_hnsw;
import :index_emvb;
import :index_bmp;
import :defer_op;
import :mem_index;
import :logger;
import :kv_store;

import column_def;
import data_type;
import logical_type;
import extra_ddl_info;
import internal_types;
import embedding_info;
import sparse_info;
import statement_common;
import constant_expr;

using namespace infinity;

class TxnReplayExceptionTest : public NewReplayTest {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TxnReplayExceptionTest,
                         ::testing::Values(TxnReplayExceptionTest::NEW_CONFIG_PATH, TxnReplayExceptionTest::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TxnReplayExceptionTest, test_replay_create_db) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto new_table_name = std::make_shared<std::string>("tb2");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        auto default_value1 = std::make_shared<ConstantExpr>(LiteralType::kInteger);
        {
            int64_t num = 1;
            default_value1->integer_value_ = num;
        }
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kInteger), "col3", std::set<ConstraintType>(), "", default_value1);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        Status status = txn->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        ConflictType conflict_type_{ConflictType::kError};
        const std::string file_name = "";
        std::vector<std::string> column_names{"col2"};
        std::shared_ptr<IndexBase> index_base = IndexSecondary::Make(index_name, index_comment_ptr, file_name, column_names);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, conflict_type_);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn4 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        Status status = txn4->RenameTable(*db_name, *table_name, *new_table_name);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
    }

    {
        std::vector<std::string> column_names;
        column_names.push_back(column_def1->name());
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop columns"), TransactionType::kDropColumn);
        Status status = txn->DropColumns(*db_name, *new_table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        Status status = txn->DropIndexByName(*db_name, *new_table_name, *index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn->DropTable(*db_name, *new_table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn1->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr_->kv_store()->Flush();
    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"default_db"}));
        status = new_txn_mgr_->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TxnReplayExceptionTest, test_replay_import) {
    using namespace infinity;

    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto new_table_name = std::make_shared<std::string>("tb2");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto index_name = std::make_shared<std::string>("index_name");
    auto index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        auto status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto index_name1 = std::make_shared<std::string>("index1");
    auto index_name2 = std::make_shared<std::string>("index2");
    {
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_def1->index_name_)),
                                           TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_def2->index_name_)),
                                           TransactionType::kCreateIndex);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def2, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    u32 block_row_cnt = 8192;
    {
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
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
            std::vector input_blocks = {make_input_block(), make_input_block()};
            auto status = txn->Import(*db_name, *table_name, input_blocks);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    }

    new_txn_mgr_->kv_store()->Flush();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        auto begin_ts = txn->BeginTS();
        auto commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0, 1}));

        auto check_block = [&](BlockMeta &block_meta) {
            NewTxnGetVisibleRangeState state;
            auto status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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

            size_t row_count = state.block_offset_end();
            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                EXPECT_EQ(col.GetValueByIndex(0), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(1), Value::MakeInt(2));
                EXPECT_EQ(col.GetValueByIndex(8190), Value::MakeInt(1));
                EXPECT_EQ(col.GetValueByIndex(8191), Value::MakeInt(2));
            }
            {
                size_t column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
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
            EXPECT_EQ(*block_ids, std::vector<BlockID>({0, 1}));

            for (auto block_id : *block_ids) {
                BlockMeta block_meta(block_id, segment_meta);
                check_block(block_meta);
            }
        };

        for (auto segment_id : *segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            check_segment(segment_meta);
        }
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());

        auto check_chunk_index = [&](ChunkIndexMeta &chunk_index_meta) {
            SegmentID segment_id = chunk_index_meta.segment_index_meta().segment_id();

            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 0));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, block_row_cnt * 2);
        };

        auto check_segment_index = [&](SegmentIndexMeta &segment_index_meta) {
            auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0}));

            for (ChunkID chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                check_chunk_index(chunk_index_meta);
            }
        };

        auto check_index = [&](const std::string &index_name) {
            auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kRead);

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            std::shared_ptr<TableIndexMeta> table_index_meta;
            std::string table_key;
            std::string index_key;
            Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
            EXPECT_TRUE(status.ok());

            std::vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0, 1}));

            for (SegmentID segment_id : *segment_ids_ptr) {
                SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);
                check_segment_index(segment_index_meta);
            }

            status = new_txn_mgr_->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        };
        check_index(*index_name1);
        check_index(*index_name2);
    }

    {
        auto *txn2 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"default_db", "db1"}));
        status = new_txn_mgr_->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TxnReplayExceptionTest, test_replay_compact) {
    using namespace infinity;

    new_txn_mgr_->PrintAllKeyValue();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("default_db");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    u32 block_row_cnt = 8192;
    auto make_input_block = [&](const Value &v1, const Value &v2) {
        auto input_block = std::make_shared<DataBlock>();
        auto append_to_col = [&](ColumnVector &col, Value v) {
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

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            make_input_block(Value::MakeInt(1), Value::MakeVarchar("abc")),
            make_input_block(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"))};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kCompact);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr_->kv_store()->Flush();
    RestartTxnMgr();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
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

        auto check_block = [&](BlockID block_id, const Value &v1, const Value &v2) {
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            Status status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, block_row_cnt);
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }
        };

        for (size_t i = 0; i < block_ids->size(); ++i) {
            BlockID block_id = (*block_ids)[i];
            Value v1 = !(block_id & 1) ? Value::MakeInt(1) : Value::MakeInt(2);
            Value v2 = !(block_id & 1) ? Value::MakeVarchar("abc") : Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz");
            check_block(block_id, v1, v2);
        }

        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TxnReplayExceptionTest, test_replay_insert) {
    using namespace infinity;

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto new_table_name = std::make_shared<std::string>("tb2");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    std::shared_ptr<std::string> index_name = std::make_shared<std::string>("index_name");
    std::shared_ptr<std::string> index_comment_ptr = std::make_shared<std::string>("index_comment");

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
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
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();
    append();

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("delete"), TransactionType::kDelete);
        std::vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn1 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        Status status = txn1->DropDatabase(*db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr_->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr_->kv_store()->Flush();
    RestartTxnMgr();

    {
        auto *txn2 = new_txn_mgr_->BeginTxn(std::make_unique<std::string>("check db"), TransactionType::kRead);
        std::vector<std::string> db_names;
        Status status = txn2->ListDatabase(db_names);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(std::set<std::string>(db_names.begin(), db_names.end()), std::set<std::string>({"default_db"}));
        status = new_txn_mgr_->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }
}
