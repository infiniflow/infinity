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

#include "gtest/gtest.h"

module infinity_core:ut.append;

import :base_test1;
import :stl;
import :third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
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
import constant_expr;
import :default_values;
import :index_secondary;
import create_index_info;
import :index_base;

using namespace infinity;

class TestTxnAppend : public BaseTestParamStr {
public:
    Tuple<SizeT, Status> GetTableRowCount(const String &db_name, const String &table_name);
};

Tuple<SizeT, Status> TestTxnAppend::GetTableRowCount(const String &db_name, const String &table_name) {
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get row count"), TransactionType::kNormal);
    TxnTimeStamp begin_ts = txn->BeginTS();
    TxnTimeStamp commit_ts = txn->CommitTS();

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = txn->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return {0, status};
    }

    SizeT row_count = 0;
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
        Vector<BlockID> *block_ids_ptr = nullptr;
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
            Pair<BlockOffset, BlockOffset> range;
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
        Status status = txn->CreateTable(*db_name, table_def, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto make_input_block = [&](const Value &v1, const Value &v2, SizeT row_cnt) {
        auto make_column = [&](SharedPtr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (SizeT i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = MakeShared<DataBlock>();
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 4);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };
    append();
    append();

    new_txn_mgr->PrintAllKeyValue();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, Vector<SegmentID>({0}));
        SegmentID segment_id = (*segment_ids_ptr)[0];

        SegmentMeta segment_meta(segment_id, *table_meta);

        SizeT segment_row_cnt = 0;
        std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
        EXPECT_EQ(segment_row_cnt, 8);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

        BlockID block_id = (*block_ids_ptr)[0];
        BlockMeta block_meta(block_id, segment_meta);
        SizeT block_row_cnt = 0;
        std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
        EXPECT_EQ(block_row_cnt, 8);

        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append1) {
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

    auto input_block = MakeShared<DataBlock>();
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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append again"), TransactionType::kNormal);

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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 1"), TransactionType::kNormal);
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 2"), TransactionType::kNormal);

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
    SizeT total_row_count = 8;

    // Check the appended data.
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
        EXPECT_EQ(*segment_ids, Vector<SegmentID>{0});
        SegmentMeta segment_meta((*segment_ids)[0], *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        BlockMeta block_meta((*block_ids_ptr)[0], segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(total_row_count));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        SizeT row_count = state.block_offset_end();

        {
            SizeT column_idx = 0;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;

            status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
            EXPECT_TRUE(status.ok());

            Value v1 = col.GetValueByIndex(0);
            EXPECT_EQ(v1, Value::MakeInt(1));
            Value v2 = col.GetValueByIndex(1);
            EXPECT_EQ(v2, Value::MakeInt(2));
        }

        {
            SizeT column_idx = 1;
            ColumnMeta column_meta(column_idx, block_meta);
            ColumnVector col;
            status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);

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

    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto input_block2 = MakeShared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(2 * i));
            }

            input_block2->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abcdefghijklmnopqrstuvwxyz_{}", i)));
            }
            input_block2->InsertVector(col2, 1);
        }
        input_block2->Finalize();
    }
    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append      commit (success)
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append again"), TransactionType::kNormal);

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
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 1"), TransactionType::kNormal);
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("concurrent append 2"), TransactionType::kNormal);

        Status status1 = txn->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status1.ok());
        Status status2 = txn2->Append(*db_name, *table_name, input_block2);
        EXPECT_TRUE(status2.ok());

        status1 = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status1.ok());
        status2 = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status2.ok());
    }
    SizeT total_row_count = insert_row * 4;

    // Check the appended data.
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
        EXPECT_EQ(segment_ids->size(), 1);
        SegmentID segment_id = segment_ids->at(0);
        EXPECT_EQ(segment_id, 0);
        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());

        SizeT block_count = (total_row_count / DEFAULT_BLOCK_CAPACITY) + (total_row_count % DEFAULT_BLOCK_CAPACITY == 0 ? 0 : 1);
        EXPECT_EQ(block_ids_ptr->size(), block_count);

        for (SizeT idx = 0; idx < block_count; ++idx) {
            BlockID block_id = block_ids_ptr->at(idx);
            EXPECT_EQ(block_id, idx);
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());

            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);

            EXPECT_EQ(range.second, DEFAULT_BLOCK_CAPACITY);

            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, DEFAULT_BLOCK_CAPACITY);

            {
                SizeT column_idx = 0;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;

                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                if (idx % 2 == 0) {
                    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeInt(row_id));
                    }
                } else {
                    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeInt(2 * row_id));
                    }
                }
            }

            {
                SizeT column_idx = 1;
                ColumnMeta column_meta(column_idx, block_meta);
                ColumnVector col;
                status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                EXPECT_TRUE(status.ok());

                if (idx % 2 == 0) {
                    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeVarchar(fmt::format("abc_{}", row_id)));
                    }
                } else {
                    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
                        Value v1 = col.GetValueByIndex(row_id);
                        EXPECT_EQ(v1, Value::MakeVarchar(fmt::format("abcdefghijklmnopqrstuvwxyz_{}", row_id)));
                    }
                }
            }
        }
    }
}

TEST_P(TestTxnAppend, test_append_drop_db) {
    using namespace infinity;

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  dropdb      commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
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

        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

            Status status = txn->Append(*db_name, *table_name, input_block1);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            // drop database
            auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
            Status status = txn6->DropDatabase("db1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn6);
            EXPECT_TRUE(status.ok());
        }

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        }
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                                   commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_drop_table) {
    using namespace infinity;

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  droptable      commit (success)
    {
        SharedPtr<String> db_name = std::make_shared<String>("db1");
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

        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

            Status status = txn->Append(*db_name, *table_name, input_block1);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            // drop table
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
            Status status = txn->DropTable("db1", "tb1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        {
            // drop database
            auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
            Status status = txn6->DropDatabase("db1", ConflictType::kError);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn6);
            EXPECT_TRUE(status.ok());
        }

        // Check the appended data.
        {
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        }
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  droptable      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop table   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop table   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append                                   commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  dropdb      commit (success)
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

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn6->DropTable("db1", "tb1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn4->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn5->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
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
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
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

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (fail)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

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

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  add column              commit (fail)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        add column        commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                 commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

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

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column  commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

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

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
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
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop column    commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (fail)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  drop column           commit (fail)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (fail)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        drop column      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        SizeT row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 0);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                drop column                 commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (fail)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        SizeT row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 0);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (fail)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  drop column   commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn4->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
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
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 8192;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (conflict)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                    commit (conflict)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (conflict)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  rename table  commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // rename table
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn4->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                           t1                  append(fail)                          rollback (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|----------|
    //                    t2           rename table   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_FALSE(status.ok());

        status = new_txn_mgr->RollBackTxn(txn3);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
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
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 4096;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append    commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
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
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
            SegmentMeta segment_meta(0, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |----------------------|----------|
    //                    t2                  append    commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        SizeT row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 8192);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  append             commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
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
            EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
            SegmentMeta segment_meta(0, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      append                                   commit (success)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        append           commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        SizeT row_count = 0;
        std::tie(row_count, status) = GetTableRowCount(*db_name, *table_name);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(row_count, 8192);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                      append                                   commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  append   commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
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
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1                                                   append                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  add column  commit (success)
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

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
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
            auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
            EXPECT_TRUE(status.ok());

            auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
            EXPECT_TRUE(seg_status.ok());
            EXPECT_EQ(*segment_ids, Vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            Vector<BlockID> *block_ids_ptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //                                                  t1                  append                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  append   commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
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
            EXPECT_EQ(*segment_ids, Vector<SegmentID>{0});
            SegmentMeta segment_meta(0, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>{0});
            BlockMeta block_meta(0, segment_meta);

            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            {
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_FALSE(has_next);
            }

            SizeT row_count = state.block_offset_end();
            EXPECT_EQ(row_count, 8192);
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

#if 0

// FIXME: Open it when append is refactored.
TEST_P(TestTxnAppend, test_append_append_concurrent) {
    using namespace infinity;

    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 4096;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    //               t1                  append    commit (success)
    //  thread1      |--------------------|----------|
    //  thread2      |------------------- |----------|
    //               t2                  append   commit (success)
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

        std::vector<std::thread> worker_threads;
        SizeT thread_count = 4;
        SizeT loop_count = 128;
        SizeT block_num = thread_count * loop_count / 2;
        for (SizeT i = 0; i < thread_count; ++i) {
            worker_threads.emplace_back([&, i] {
                for (SizeT j = 0; j < loop_count; ++j) {
                    Status status;
                    int k = 0;
                    do {
                        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("append_{}_{}_{}", i, j, k)), TransactionType::kNormal);
                        status = txn3->Append(*db_name, *table_name, input_block1);
                        EXPECT_TRUE(status.ok());
                        status = new_txn_mgr->CommitTxn(txn3);
                        ++k;
                    } while (!status.ok());
                }
            });
        }
        for (SizeT i = 0; i < thread_count; ++i) {
            worker_threads[i].join();
        }

        // Check the appended data.
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
            EXPECT_EQ(segment_ids->size(), 1);
            {
                SegmentID segment_id = segment_ids->at(0);
                EXPECT_EQ(segment_id, 0);
                SegmentMeta segment_meta(segment_id, *table_meta);

                Vector<BlockID> *block_ids_ptr = nullptr;
                std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

                EXPECT_TRUE(status.ok());
                EXPECT_EQ(block_ids_ptr->size(), block_num);
                for (SizeT idx = 0; idx < block_ids_ptr->size(); ++idx) {
                    BlockID block_id = block_ids_ptr->at(idx);
                    EXPECT_EQ(block_id, idx);
                    BlockMeta block_meta(block_id, segment_meta);

                    NewTxnGetVisibleRangeState state;
                    status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
                    EXPECT_TRUE(status.ok());
                    SizeT row_count = state.block_offset_end();
                    {
                        Pair<BlockOffset, BlockOffset> range;
                        BlockOffset offset = 0;
                        bool has_next = state.Next(offset, range);
                        EXPECT_TRUE(has_next);
                        EXPECT_EQ(range.first, 0);
                        EXPECT_EQ(range.second, static_cast<BlockOffset>(8192));
                        offset = range.second;
                        has_next = state.Next(offset, range);
                        EXPECT_FALSE(has_next);

                        {
                            SizeT column_idx = 0;
                            ColumnMeta column_meta(column_idx, block_meta);
                            ColumnVector col;

                            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                            EXPECT_TRUE(status.ok());

                            for (SizeT row_id = 0; row_id < 4096; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeInt(row_id));
                            }
                            for (SizeT row_id = 4096; row_id < 8192; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeInt(row_id - 4096));
                            }
                        }
                        {
                            SizeT column_idx = 1;
                            ColumnMeta column_meta(column_idx, block_meta);
                            ColumnVector col;

                            Status status = NewCatalog::GetColumnVector(column_meta, row_count, ColumnVectorMode::kReadOnly, col);
                            EXPECT_TRUE(status.ok());

                            for (SizeT row_id = 0; row_id < 4096; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeVarchar(fmt::format("abc_{}", row_id)));
                            }
                            for (SizeT row_id = 4096; row_id < 8192; ++row_id) {
                                EXPECT_EQ(col.GetValueByIndex(row_id), Value::MakeVarchar(fmt::format("abc_{}", row_id - 4096)));
                            }
                        }
                    }
                    EXPECT_EQ(row_count, 8192);
                }
            }
        }

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

#endif

TEST_P(TestTxnAppend, test_append_and_create_index) {

    using namespace infinity;

    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    std::shared_ptr<ConstantExpr> default_int = std::make_shared<ConstantExpr>(LiteralType::kInteger);
    default_int->integer_value_ = 2;
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 4096;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        Status status;
        Vector<BlockID> *block_ids_ptr = nullptr;
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        BlockMeta block_meta(0, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(4096));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        SizeT row_count = state.block_offset_end();
        EXPECT_EQ(row_count, 4096);
    };
    auto check_table = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->at(0), "idx1");

        TableIndexMeeta table_index_meta(index_id_strs_ptr->at(0), *table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        EXPECT_TRUE(index_status.ok());
        EXPECT_EQ(*index_base->index_name_, String("idx1"));
        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_segment_ids_ptr->size(), 1);
    };
    auto check_no_index = [&] {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 0);
    };
    auto check_no_data = [&] {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({}));

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_names_ptr, Vector<String>({"idx1"}));
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  create index  commit (success)
    {
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        new_txn_mgr->PrintAllKeyValue();
        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        check_no_index();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_FALSE(status.ok());

        check_no_index();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        check_no_data();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block1 = MakeShared<DataBlock>();
    SizeT insert_row = 4096;
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto check_segment = [&](SegmentMeta &segment_meta, NewTxn *txn) {
        Status status;
        Vector<BlockID> *block_ids_ptr = nullptr;
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();

        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        BlockMeta block_meta(0, segment_meta);

        NewTxnGetVisibleRangeState state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, static_cast<BlockOffset>(4096));
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }

        SizeT row_count = state.block_offset_end();
        EXPECT_EQ(row_count, 4096);
    };

    auto check_index = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(index_id_strs_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->size(), 1);
        EXPECT_EQ(index_names_ptr->at(0), "idx1");

        TableIndexMeeta table_index_meta(index_id_strs_ptr->at(0), *table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        EXPECT_TRUE(index_status.ok());
        EXPECT_EQ(*index_base->index_name_, String("idx1"));
        EXPECT_EQ(index_base->index_type_, IndexType::kSecondary);

        Vector<SegmentID> *index_segment_ids_ptr = nullptr;
        std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0}));
    };

    auto check_table = [&]() {
        // Check the appended data.
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        auto [segment_ids, seg_status] = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(seg_status.ok());
        EXPECT_EQ(*segment_ids, Vector<SegmentID>({0}));
        SegmentMeta segment_meta(0, *table_meta);

        check_segment(segment_meta, txn);

        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*index_id_strs_ptr, Vector<String>({}));
        EXPECT_EQ(*index_names_ptr, Vector<String>({}));
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop index  commit (success)
    {
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_index();

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        check_index();

        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        check_index();

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
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
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn7->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_table();

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnAppend, test_append_and_compact) {

    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});

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

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      append      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  compact  commit (success)
    {
        PrepareForCompact();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kAppend);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());
        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             compact           commit
    {
        PrepareForCompact();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  compact                   commit (success)
    {
        PrepareForCompact();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1      append                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        compact          commit (success)
    {
        PrepareForCompact();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1                                      append                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                compact                     commit (success)
    {
        PrepareForCompact();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //    t1                                                   append                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  compact commit (success)
    {
        PrepareForCompact();

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //                                                  t1                  append                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  compact   commit (success)
    {
        PrepareForCompact();

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }

    //                                                           t1                  append                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           compact   commit (success)
    {
        PrepareForCompact();

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        CheckTable({2, 3});

        DropDB();
    }
}

TEST_P(TestTxnAppend, test_append_and_optimize_index) {

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

    auto PrepareForAppendAndOptimize = [&] {
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
        EXPECT_EQ(*index_segment_ids_ptr, Vector<SegmentID>({0}));

        SegmentIndexMeta segment_index_meta((*index_segment_ids_ptr)[0], *table_index_meta);
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*chunk_ids_ptr, Vector<ChunkID>({2}));
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    /* FIXME: PostRollback() for dump index is not implemented.
    //    t1      append      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             optimize           commit
    {
        PrepareForAppendAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }

    //    t1      append                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                    optimize         commit (success)
    {
        PrepareForAppendAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }
    */

    //    t1      append                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize          commit (success)
    {
        PrepareForAppendAndOptimize();

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

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

        SharedPtr<DataBlock> input_block1 = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn4->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();
    }
}
