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

module infinity_core:ut.replay_compact;

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
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
import :default_values;
import :index_secondary;
import :index_full_text;
import :mem_index;
import :index_base;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import internal_types;
import statement_common;

using namespace infinity;

class TestTxnReplayCompact : public NewReplayTest {
protected:
    void SetUp() override {
        NewReplayTest::SetUp();
        db_name = std::make_shared<std::string>("default_db");
        column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
        column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
        table_name = std::make_shared<std::string>("tb1");
        table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        index_name1 = std::make_shared<std::string>("index1");
        index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        index_name2 = std::make_shared<std::string>("index2");
        index_def2 = IndexFullText::Make(index_name2, std::make_shared<std::string>(), "file_name", {column_def2->name()}, {});
        block_row_cnt = 8192;
    }

    void CheckDataAfterSuccesfulCompact() {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kNormal);

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        size_t table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, 8192 * 4);

        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({2}));
        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, *table_meta);

            size_t segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, 8192 * 4);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0, 1, 2, 3}));

            size_t block_row_cnt = 0;
            for (const auto &block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
                EXPECT_EQ(block_row_cnt, 8192);

                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, txn->BeginTS(), txn->CommitTS(), state);
                EXPECT_TRUE(status.ok());
                {
                    std::pair<BlockOffset, BlockOffset> range;
                    BlockOffset offset = 0;
                    bool has_next = state.Next(offset, range);
                    EXPECT_TRUE(has_next);
                    EXPECT_EQ(range.first, 0);
                    EXPECT_EQ(range.second, 8192);
                    offset = range.second;
                    has_next = state.Next(offset, range);
                    EXPECT_FALSE(has_next);
                }
            }
        }
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    void CheckDataAfterFailedCompact() {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kNormal);

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        size_t table_row_cnt;
        std::tie(table_row_cnt, status) = table_meta->GetTableRowCount();
        EXPECT_EQ(table_row_cnt, 8192 * 4);

        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0, 1}));
        for (auto segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, *table_meta);

            size_t segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, 8192 * 2);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0, 1}));

            size_t block_row_cnt = 0;
            for (const auto &block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);
                std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
                EXPECT_EQ(block_row_cnt, 8192);

                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, txn->BeginTS(), txn->CommitTS(), state);
                EXPECT_TRUE(status.ok());
                {
                    std::pair<BlockOffset, BlockOffset> range;
                    BlockOffset offset = 0;
                    bool has_next = state.Next(offset, range);
                    EXPECT_TRUE(has_next);
                    EXPECT_EQ(range.first, 0);
                    EXPECT_EQ(range.second, 8192);
                    offset = range.second;
                    has_next = state.Next(offset, range);
                    EXPECT_FALSE(has_next);
                }
            }
        }
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    void CheckIndexAfterSuccessfulCompact(const std::string &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kNormal);

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        std::shared_ptr<TableIndexMeeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({2}));
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
            Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 0));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, block_row_cnt * 4);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    void CheckIndexAfterFailedCompact(const std::string &index_name) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check index"), TransactionType::kNormal);

        std::shared_ptr<DBMeeta> db_meta;
        std::shared_ptr<TableMeeta> table_meta;
        std::shared_ptr<TableIndexMeeta> table_index_meta;
        std::string table_key;
        std::string index_key;
        Status status = txn->GetTableIndexMeta(*db_name, *table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        {
            auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0, 1}));
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
            Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(chunk_info_ptr->base_row_id_, RowID(segment_id, 0));
            EXPECT_EQ(chunk_info_ptr->row_cnt_, block_row_cnt * 2);
        }

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

protected:
    std::shared_ptr<std::string> db_name{};
    std::shared_ptr<ColumnDef> column_def1{};
    std::shared_ptr<ColumnDef> column_def2{};
    std::shared_ptr<std::string> table_name{};
    std::shared_ptr<TableDef> table_def{};
    std::shared_ptr<std::string> index_name1{};
    std::shared_ptr<IndexBase> index_def1{};
    std::shared_ptr<std::string> index_name2{};
    std::shared_ptr<IndexBase> index_def2{};
    u32 block_row_cnt{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnReplayCompact,
                         ::testing::Values(TestTxnReplayCompact::NEW_CONFIG_PATH, TestTxnReplayCompact::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnReplayCompact, test_compact_commit) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};

        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    CheckDataAfterSuccesfulCompact();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayCompact, test_compact_rollback) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kNormal);
        Status status = txn2->DropColumns(*db_name, *table_name, std::vector<std::string>({column_def1->name()}));
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }

    RestartTxnMgr();

    CheckDataAfterFailedCompact();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayCompact, test_replay_compact_flush_gap) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, std::vector<SegmentID>({0, 1}));
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    Status status = txn->Checkpoint(wal_manager_->LastCheckpointTS());
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    RestartTxnMgr();

    CheckDataAfterSuccesfulCompact();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayCompact, test_compact_interrupt) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    CheckDataAfterFailedCompact();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayCompact, DISABLED_test_compact_with_index) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);
    create_index(index_def2);

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        Status status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    RestartTxnMgr();

    CheckDataAfterSuccesfulCompact();

    CheckIndexAfterSuccessfulCompact(*index_name1);
    CheckIndexAfterSuccessfulCompact(*index_name2);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnReplayCompact, test_compact1_rollback) {
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kNormal);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    auto create_index = [&](const std::shared_ptr<IndexBase> &index_base) {
        auto *txn =
            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("create index {}", *index_base->index_name_)), TransactionType::kNormal);
        Status status = txn->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    create_index(index_def1);

    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kNormal);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {
            MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abc"), block_row_cnt),
            MakeInputBlock(Value::MakeInt(2), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), block_row_cnt)};
        Status status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kNormal);
        Status status = txn2->DropColumns(*db_name, *table_name, std::vector<std::string>({column_def2->name()}));
        EXPECT_TRUE(status.ok());

        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("compact"), TransactionType::kNormal);
        status = txn->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_FALSE(status.ok());
    }

    RestartTxnMgr();
    CheckDataAfterFailedCompact();
    CheckIndexAfterFailedCompact(*index_name1);

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*db_name, *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
