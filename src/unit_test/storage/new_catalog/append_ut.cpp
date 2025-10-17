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

module infinity_core:ut.append;

import :ut.base_test;

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
import :index_base;

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import constant_expr;
import create_index_info;

using namespace infinity;

class TestTxnAppend : public BaseTestParamStr {
public:
    std::tuple<size_t, Status> GetTableRowCount(const std::string &db_name, const std::string &table_name);
};

std::tuple<size_t, Status> TestTxnAppend::GetTableRowCount(const std::string &db_name, const std::string &table_name) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("get row count"), TransactionType::kRead);
    TxnTimeStamp begin_ts = txn->BeginTS();
    TxnTimeStamp commit_ts = txn->CommitTS();

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta;
    TxnTimeStamp create_timestamp;
    Status status = txn->GetTableMeta(db_name, table_name, db_meta, table_meta, create_timestamp);
    if (!status.ok()) {
        return {0, status};
    }

    size_t row_count = 0;
    auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
    if (!status.ok()) {
        return {0, status};
    }
    for (auto &segment_id : *segment_ids) {
        SegmentMeta segment_meta(segment_id, *table_meta);
        auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return {0, status};
        }
        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return {0, status};
        }

        for (auto &block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            if (!status.ok()) {
                return {0, status};
            }
            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset block_offset_begin = 0;
            while (state.Next(block_offset_begin, range)) {
                block_offset_begin = range.second;
                row_count += range.second - range.first;
            }
        }
    }
    return {row_count, status};
}

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnAppend,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnAppend, test_append0) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto append = [&] {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();
    append();

    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];

        SegmentMeta segment_meta(segment_id, *table_meta);

        size_t segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 8);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));

        BlockID block_id = (*block_ids_ptr)[0];
        BlockMeta block_meta(block_id, segment_meta);
        size_t block_row_cnt = 0;
        std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
        EXPECT_EQ(block_row_cnt, 8);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append1) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto input_block = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }
    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append      commit (success)
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append again"), TransactionType::kAppend);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    new_txn_mgr->PrintAllKeyValue();

    //    t1      append                 commit (success)
    //    |----------|-------------------------|
    //            |----------------------|----------|
    //           t2                  append      commit (success)
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("concurrent append 1"), TransactionType::kAppend);
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("concurrent append 2"), TransactionType::kAppend);

        Status status1 = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status1.ok());
        Status status2 = txn2->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status2.ok());

        status1 = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status1.ok());

        new_txn_mgr->PrintAllKeyValue();

        status2 = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status2.ok());
    }
    size_t total_row_count = 8;

    // Check the appended data.
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>{0});
        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
        BlockMeta block_meta((*block_ids_ptr)[0], segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(total_row_count));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        size_t row_count = state.block_offset_end();

        {
            size_t column_idx = 0;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            Value v1 = col.GetValueByIndex(0);
            EXPECT_EQ(v1, Value::MakeInt(1));
            Value v2 = col.GetValueByIndex(1);
            EXPECT_EQ(v2, Value::MakeInt(2));
        }

        {
            size_t column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;
            status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);

            EXPECT_TRUE(status.ok());
            Value v1 = col.GetValueByIndex(0);
            EXPECT_EQ(v1, Value::MakeVarchar("abc"));
            Value v2 = col.GetValueByIndex(1);
            EXPECT_EQ(v2, Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        }
    }
}

TEST_P(TestTxnAppend, test_append2) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    size_t insert_row = 8192;

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append      commit (success)
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(insert_row);
        Status status = txn->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append again"), TransactionType::kAppend);
        auto input_block2 = MakeInputBlock2(insert_row);
        Status status = txn->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                 commit (success)
    //    |----------|-------------------------|
    //            |----------------------|----------|
    //           t2                  append      commit (success)
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("concurrent append 1"), TransactionType::kAppend);
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("concurrent append 2"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        auto input_block2 = MakeInputBlock2(8192);
        Status status1 = txn->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status1.ok());
        Status status2 = txn2->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status2.ok());

        status1 = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status1.ok());
        status2 = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status2.ok());
    }
    size_t total_row_count = insert_row * 4;

    // Check the appended data.
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(segment_ids->size(), 1);
        SegmentID segment_id = segment_ids->at(0);
        EXPECT_EQ(segment_id, 0);
        SegmentMeta segment_meta(segment_id, *table_meta);

        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());

        size_t block_count = (total_row_count / DEFAULT_BLOCK_CAPACITY) + (total_row_count % DEFAULT_BLOCK_CAPACITY == 0 ? 0 : 1);
        EXPECT_EQ(block_ids_ptr->size(), block_count);

        for (size_t idx = 0; idx < block_count; ++idx) {
            BlockID block_id = block_ids_ptr->at(idx);
            EXPECT_EQ(block_id, idx);
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);

            EXPECT_EQ(range.second, DEFAULT_BLOCK_CAPACITY);

            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, DEFAULT_BLOCK_CAPACITY);

            {
                size_t column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                if (idx % 2 == 0) {
                    for (size_t row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeInt(row_id));
                    }
                } else {
                    for (size_t row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeInt(2 * row_id));
                    }
                }
            }

            {
                size_t column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;
                status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                if (idx % 2 == 0) {
                    for (size_t row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeVarchar(fmt::format("abc_{}", row_id)));
                    }
                } else {
                    for (size_t row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeVarchar(fmt::format("abcdefghijklmnopqrstuvwxyz_{}", row_id)));
                    }
                }
            }
        }
    }
}

TEST_P(TestTxnAppend, test_append_mismatch) {
    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto input_block = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }

    auto input_block2 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block2->InsertVector(col1, 0);
        }
        input_block2->Finalize();
    }

    auto input_block3 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block3->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def1->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeInt(1));
            col2->AppendValue(Value::MakeInt(2));
            input_block3->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, input_block2);
        EXPECT_FALSE(status.ok()); // ColumnCountMismatch
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append again"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, input_block3);
        EXPECT_FALSE(status.ok()); // DataTypeMismatch
        status = new_txn_mgr->RollBackTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_drop_db) {
    using namespace infinity;

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
#if 0
    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block1 = MakeInputBlock1(8192);
            Status status = txn->Append(*db_name, *table_name, input_block1);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            // drop database
            auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
            Status status = txn6->DropDatabase("db1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn6);
            EXPECT_TRUE(status.ok());
        }

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        }
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
#endif
    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
#if 0
    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                                   commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_drop_table) {
    using namespace infinity;

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  droptable      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            auto input_block1 = MakeInputBlock1(8192);
            Status status = txn->Append(*db_name, *table_name, input_block1);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            // drop table
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
            Status status = txn->DropTable("db1", "tb1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            // drop database
            auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
            Status status = txn6->DropDatabase("db1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn6);
            EXPECT_TRUE(status.ok());
        }

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        }
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop table   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop table   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                                   commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_add_column) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (fail)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  add column              commit (fail)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        add column        commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                 commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column  commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_drop_column) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop column    commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (fail)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  drop column           commit (fail)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        drop column      commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        size_t row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 0);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                drop column                 commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column  commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        size_t row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 0);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("add column"), TransactionType::kAddColumn);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        std::vector<std::shared_ptr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn4->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop column"), TransactionType::kDropColumn);
        std::vector<std::string> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_rename) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  rename    commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  rename   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  rename table         commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (conflict)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        rename table (success) commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                    commit (conflict)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                rename                 commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (conflict)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  rename table  commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  rename   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|----------|
    //                    t2           rename table   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("rename table"), TransactionType::kRenameTable);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto input_block1 = MakeInputBlock1(8192);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_append) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
            SegmentMeta segment_meta(0, *table_meta);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  append    commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        size_t row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 8192);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  append             commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
            SegmentMeta segment_meta(0, *table_meta);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (success)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        append           commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        size_t row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 8192);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  append   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column  commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            std::vector<BlockID> *block_ids_ptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  append   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, std::vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            size_t row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_append_concurrent) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //               t1                  append    commit (success)
    //  thread1      |--------------------|----------|
    //  thread2      |------------------- |----------|
    //               t2                  append   commit (success)
    {
        std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        std::vector<std::thread> worker_threads;
        size_t thread_count = 4;
        size_t loop_count = 128;
        size_t block_num = thread_count * loop_count / 2;
        for (size_t i = 0; i < thread_count; ++i) {
            worker_threads.emplace_back([&, i] {
                for (size_t j = 0; j < loop_count; ++j) {
                    Status status;
                    int k = 0;
                    do {
                        auto *txn3 =
                            new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("append_{}_{}_{}", i, j, k)), TransactionType::kAppend);
                        status = txn3->Append(*db_name, *table_name, input_block1);
                        EXPECT_TRUE(status.ok());
                        status = new_txn_mgr->CommitTxn(txn3);
                        ++k;
                    } while (!status.ok());
                }
            });
        }
        for (size_t i = 0; i < thread_count; ++i) {
            worker_threads[i].join();
        }

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta;
            TxnTimeStamp create_timestamp;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(segment_ids->size(), 1);
            {
                SegmentID segment_id = segment_ids->at(0);
                EXPECT_EQ(segment_id, 0);
                SegmentMeta segment_meta(segment_id, *table_meta);

                std::vector<BlockID> *block_ids_ptr = nullptr;
                std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_ids_ptr->size(), block_num);
                for (size_t idx = 0; idx < block_ids_ptr->size(); ++idx) {
                    BlockID block_id = block_ids_ptr->at(idx);
                    EXPECT_EQ(block_id, idx);
                    BlockMeta block_meta(block_id, segment_meta);

                    NewTxnGetVisibleRangeState state;
                    status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
                    EXPECT_TRUE(status.ok());
                    size_t row_count = state.block_offset_end();
                    {
                        std::pair<BlockOffset, BlockOffset> range;
                        BlockOffset offset = 0;
                        bool has_next = state.Next(offset, range);
                        EXPECT_TRUE(has_next);
                        EXPECT_EQ(range.first, 0);
                        EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                        offset = range.second;
                        has_next = state.Next(offset, range);
                        EXPECT_FALSE(has_next);

                        {
                            size_t column_idx = 0;
                            ColumnMeta column_meta(column_idx, block_meta);
                            ColumnVector col;

                            Status status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
                            EXPECT_TRUE(status.ok());

                            for (size_t row_id = 0; row_id < 4096; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeInt(row_id));
                            }
                            for (size_t row_id = 4096; row_id < 8192; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeInt(row_id - 4096));
                            }
                        }
                        {
                            size_t column_idx = 1;
                            ColumnMeta column_meta(column_idx, block_meta);
                            ColumnVector col;

                            Status status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, col);
                            EXPECT_TRUE(status.ok());

                            for (size_t row_id = 0; row_id < 4096; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeVarchar(fmt::format("abc_{}", row_id)));
                            }
                            for (size_t row_id = 4096; row_id < 8192; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeVarchar(fmt::format("abc_{}", row_id - 4096)));
                            }
                        }
                    }
                    EXPECT_EQ(row_count, 8192);
                }
            }
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);
        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_and_create_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        Status status;
        std::vector<BlockID> *block_ids_ptr = nullptr;
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
        BlockMeta block_meta(0, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(4096));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        size_t row_count = state.block_offset_end();
        EXPECT_EQ(row_count, 4096);
    };
    auto check_table = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->at(0), "idx1");

        TableIndexMeta table_index_meta(index_id_strs_ptr->at(0), *table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        EXPECT_TRUE(index_status.ok());
        EXPECT_EQ(*index_base->index_name_, std::string("idx1"));
        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_segment_ids_ptr->size(), 1);
    };
    auto check_no_index = [&] {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 0);
    };
    auto check_no_data = [&] {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({}));

        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_names_ptr, std::vector<std::string>({"idx1"}));
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  create index  commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             create index     commit (fail)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        check_no_index();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  create index         commit (fail)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        check_no_index();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        create index (success)    commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                    commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                create index                 commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  create index  commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  create index   commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           create index   commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_and_drop_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        Status status;
        std::vector<BlockID> *block_ids_ptr = nullptr;
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));
        BlockMeta block_meta(0, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(4096));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        size_t row_count = state.block_offset_end();
        EXPECT_EQ(row_count, 4096);
    };

    auto check_index = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->at(0), "idx1");

        TableIndexMeta table_index_meta(index_id_strs_ptr->at(0), *table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        EXPECT_TRUE(index_status.ok());
        EXPECT_EQ(*index_base->index_name_, std::string("idx1"));
        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, std::vector<SegmentID>({0}));
    };

    auto check_table = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("scan"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, std::vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_id_strs_ptr, std::vector<std::string>({}));
        EXPECT_EQ(*index_names_ptr, std::vector<std::string>({}));
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop index  commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index();

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             drop index     commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        check_index();

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  drop index         commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        check_index();

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        drop index (success)    commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                    commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                drop index                 commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  drop index  commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  drop index   commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           drop index   commit (success)
    {
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
        auto *txn1 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        Status status = txn1->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        status = txn2->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<std::string>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, std::make_shared<std::string>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kDropIndex);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        auto input_block1 = MakeInputBlock1(4096);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("drop db"), TransactionType::kDropDB);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_and_compact) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<std::string> db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});
    auto PrepareForCompact = [&] {
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // For compact
        for (int i = 0; i < 2; ++i) {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>(fmt::format("import {}", i)), TransactionType::kDumpMemIndex);
                Status status = txn->DumpMemIndex(*db_name, *table_name, *index_name1, 0);
                EXPECT_TRUE(status.ok());
                status = new_txn_mgr->CommitTxn(txn);
                EXPECT_TRUE(status.ok());
            }
        }

        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
            std::shared_ptr<DataBlock> input_block = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
            Status status = txn->Append(*db_name, *table_name, input_block);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    auto CheckTable = [&](const std::vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<TableIndexMeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, std::vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        std::vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({2}));
    };

    auto CheckTableRollback = [&](const std::vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("check table"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<TableIndexMeta> table_index_meta;
        status = txn->GetTableIndexMeta(*index_name1, *table_meta, table_index_meta);
        EXPECT_TRUE(status.ok());

        std::shared_ptr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_base->index_name_, *index_name1);

        std::vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, std::vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        std::vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, std::vector<ChunkID>({0, 1}));
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kDropDB);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------------|
    //                           t2                  optimize index     commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             optimize           commit (fail)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        // Rollback
        EXPECT_FALSE(status.ok());

        CheckTableRollback({0});

        DropDB();
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    optimize         commit (fail)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTableRollback({0});

        DropDB();
    }

    //    t1      append                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize          commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //    t1                                      append                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                optimize                     commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //    t1                                                       append                                   commit (success)
    //    |-----------------------------------------------------------|------------------------------------------|
    //                    |----------------------|--------------|
    //                    t2                  optimize index   commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //                                                  t1                  append                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  optimize   commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //                                                           t1                  append                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           optimize index   commit (success)
    {
        PrepareForAppendAndOptimize();

        std::shared_ptr<DataBlock> input_block1 = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }
#endif
}
