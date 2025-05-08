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
import data_block;
import column_vector;
import value;
import new_txn;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import db_meeta;
import constant_expr;
import default_values;
import internal_types;
import logger;
import index_secondary;

using namespace infinity;

class TestTxnDelete : public BaseTestParamStr {
public:
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnDelete,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnDelete, test_delete) {
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
    for (int i = 0; i < 2; ++i) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    TxnTimeStamp first_delete_ts = 0;
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn, &first_delete_ts);
        EXPECT_TRUE(status.ok());
    }

    // Check data
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        {
            TxnTimeStamp first_delete_ts = 0;
            Status status = segment_meta.GetFirstDeleteTS(first_delete_ts);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(first_delete_ts, first_delete_ts);
        }

        BlockMeta block_meta(block_id, segment_meta);

        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
        EXPECT_TRUE(status.ok());
        {
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            bool has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 0);
            EXPECT_EQ(range.second, 1);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 2);
            EXPECT_EQ(range.second, 3);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 0));
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn, &first_delete_ts);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        {
            TxnTimeStamp first_delete_ts = 0;
            Status status = segment_meta.GetFirstDeleteTS(first_delete_ts);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(first_delete_ts, first_delete_ts);
        }
    }
}

TEST_P(TestTxnDelete, test_delete_multiple_blocks) {
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
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
        Status status = txn->Append(*db_name, *table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();
    append();

    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 2 * 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check data
    {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    }
}

TEST_P(TestTxnDelete, test_delete_and_drop_db) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete           commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                            commit (success)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                        delete                                            commit (success)
    //    |-------------------------------------------|----------------------------------------------------|
    //         |----------------------|----------|
    //        t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
}

TEST_P(TestTxnDelete, test_delete_and_drop_table) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_data();

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable("db1", *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete           commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        check_data();

        status = txn5->DropTable("db1", *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                            commit (success)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable("db1", *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                        delete                                            commit (success)
    //    |-------------------------------------------|----------------------------------------------------|
    //         |----------------------|----------|
    //        t2                  drop db    commit (success)
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

        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        status = txn5->DropTable("db1", *table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = new_txn_mgr->RollBackTxn(txn7);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn6->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }
}

TEST_P(TestTxnDelete, test_delete_and_add_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  add column          commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        add column       commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                               commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->RollBackTxn(txn4);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Add columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("add column"), TransactionType::kNormal);
        auto column_def3 =
            std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        auto column_def4 =
            std::make_shared<ColumnDef>(3, std::make_shared<DataType>(LogicalType::kVarchar), "col4", std::set<ConstraintType>(), default_varchar);
        Vector<SharedPtr<ColumnDef>> columns;
        columns.emplace_back(column_def3);
        columns.emplace_back(column_def4);
        status = txn5->AddColumns(*db_name, *table_name, columns);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_drop_column) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  drop column         commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
    //    |----------|------------------------------------------|
    //                    |-------------|-------------------|
    //                    t2        drop column     commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                               commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   append                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Delete columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Drop columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                          t1                  delete(fail)                          rollback (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Drop columns
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop column"), TransactionType::kNormal);
        Vector<String> column_names;
        column_names.push_back("col2");
        status = txn5->DropColumns(*db_name, *table_name, column_names);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_rename) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, "table2", db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|----------|
    //                    t2                  rename  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|----------|
    //                    t2           rename   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Rename table
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("rename table"), TransactionType::kNormal);
        status = txn5->RenameTable(*db_name, *table_name, "table2");
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, "table2", row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_create_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // create index idx1
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_name1 = std::make_shared<String>("idx1");
        auto index_def1 = IndexSecondary::Make(index_name1, MakeShared<String>(), "file_name", {column_def1->name()});
        status = txn5->CreateIndex(*db_name, *table_name, index_def1, ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn4 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn4->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

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

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_drop_index) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Drop index
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        status = txn6->DropIndexByName(*db_name, *table_name, *index_name1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_import) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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
    auto make_block = [&]() {
        auto input_block2 = MakeShared<DataBlock>();
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block2->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block2->InsertVector(col2, 1);
        }
        input_block2->Finalize();
        return input_block2;
    };

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        {
            SegmentID segment_id = 0;
            NewTxnGetVisibleRangeState state;

            SegmentMeta segment_meta(segment_id, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            for (const auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                SizeT row_id = 0;
                while (true) {
                    bool has_next = state.Next(offset, range);
                    if (!has_next) {
                        break;
                    }
                    EXPECT_EQ(range.first, row_id);
                    EXPECT_EQ(range.second, row_id + 1);
                    offset = range.second;
                    row_id += 2;
                }
            }
        }

        {
            SegmentID segment_id = 1;
            NewTxnGetVisibleRangeState state;

            SegmentMeta segment_meta(segment_id, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            for (const auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                bool has_next = state.Next(offset, range);
                EXPECT_TRUE(has_next);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, 8192);
                offset = range.second;
                has_next = state.Next(offset, range);
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, 8192);
                EXPECT_FALSE(has_next);
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  import  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             import     commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  import         commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        import (success)    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                import                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  import  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  import   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           import   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Import data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("import"), TransactionType::kNormal);
        Vector<SharedPtr<DataBlock>> input_blocks = {make_block()};
        status = txn6->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_append) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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
    auto make_block = [&]() {
        auto input_block2 = MakeShared<DataBlock>();
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(column_def1->type());
            col1->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }

            input_block2->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(column_def2->type());
            col2->Initialize();
            for (SizeT i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }

            //            col2->AppendValue(Value::MakeVarchar("abc"));
            //            col2->AppendValue(Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"));
            input_block2->InsertVector(col2, 1);
        }
        input_block2->Finalize();
        return input_block2;
    };

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        {
            SegmentID segment_id = 0;
            NewTxnGetVisibleRangeState state;

            SegmentMeta segment_meta(segment_id, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));

            {
                BlockMeta block_meta(0, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                SizeT row_id = 0;
                while (true) {
                    bool has_next = state.Next(offset, range);
                    if (!has_next) {
                        break;
                    }
                    EXPECT_EQ(range.first, row_id);
                    EXPECT_EQ(range.second, row_id + 1);
                    offset = range.second;
                    row_id += 2;
                }
            }
            {
                BlockMeta block_meta(1, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                SizeT row_id = 0;
                while (true) {
                    bool has_next = state.Next(offset, range);
                    if (!has_next) {
                        break;
                    }
                    EXPECT_EQ(range.first, row_id);
                    EXPECT_EQ(range.second, 8192);
                    offset = range.second;
                    row_id += 2;
                }
            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  append  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             append     commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  append         commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (success)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        append (success)    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                append                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Append data
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  append  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           append   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
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

        // Append data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn6->Append(*db_name, *table_name, make_block());
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_delete) {
    using namespace infinity;
    NewTxnManager *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<String> db_name = std::make_shared<String>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
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

    auto check_data = [&]() {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        {
            SegmentID segment_id = 0;
            NewTxnGetVisibleRangeState state;

            SegmentMeta segment_meta(segment_id, *table_meta);

            Vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));

            {
                BlockMeta block_meta(0, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                SizeT row_id = 0;
                while (true) {
                    bool has_next = state.Next(offset, range);
                    if (!has_next) {
                        break;
                    }
                    EXPECT_EQ(range.first, row_id);
                    EXPECT_EQ(range.second, row_id + 1);
                    offset = range.second;
                    row_id += 2;
                }
            }
            //            {
            //                BlockMeta block_meta(1, segment_meta);
            //
            //                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            //                EXPECT_TRUE(status.ok());
            //                Pair<BlockOffset, BlockOffset> range;
            //                BlockOffset offset = 0;
            //                SizeT row_id = 0;
            //                while (true) {
            //                    bool has_next = state.Next(offset, range);
            //                    if (!has_next) {
            //                        break;
            //                    }
            //                    EXPECT_EQ(range.first, row_id);
            //                    EXPECT_EQ(range.second, 8192);
            //                    offset = range.second;
            //                    row_id += 2;
            //                }
            //            }
        }
    };

    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  delete  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        row_ids.clear();
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             delete     commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        row_ids.clear();
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }

        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        // Check data
        //        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  delete         commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        row_ids.clear();
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                   commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        delete (success)    commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        row_ids.clear();
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (fail)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                delete                   commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        row_ids.clear();
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  append  commit (success)
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

        // Append data
        auto *txn3 = new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        status = txn3->Append(*db_name, *table_name, input_block1);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn3);
        EXPECT_TRUE(status.ok());

        // Delete data
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // Delete data
        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        row_ids.clear();
        for (SizeT row_id = 0; row_id < 8192; row_id += 4) {
            row_ids.push_back(RowID(0, row_id));
        }
        row_ids.push_back(RowID(0, 8191));
        status = txn5->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        // Check data
        check_data();

        // drop database
        auto *txn7 = new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        //        row_ids.clear();
        //        for (SizeT row_id = 0; row_id < 8192; row_id += 1) {
        //            row_ids.push_back(RowID(0, row_id));
        //        }
        //        txn7->PrintVersion(*db_name, *table_name, row_ids, false);
        status = txn7->DropDatabase("db1", ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        // Check the appended data.
        auto *txn8 = new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn8->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = new_txn_mgr->RollBackTxn(txn8);
        EXPECT_TRUE(status.ok());

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_compact) {

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
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        SegmentMeta segment_meta(segment_ids_ptr->at(0), *table_meta);

        NewTxnGetVisibleRangeState state;
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_EQ(block_ids_ptr->size(), 1);
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, 0);
                EXPECT_EQ(range.second, 8192);
                offset = range.second;
            }
        }
    };

    auto CheckTableNoCompact = [&](const Vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);
        EXPECT_EQ(segment_ids_ptr->size(), 2);
    };

    auto CheckTableCompactedWithoutDelete = [&](const Vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        SegmentMeta segment_meta(segment_ids_ptr->at(0), *table_meta);

        NewTxnGetVisibleRangeState state;
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_EQ(block_ids_ptr->size(), 2);
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  compact  commit (success)
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             compact           commit (fail)
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTableNoCompact({0, 1});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  compact               commit (fail)
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_FALSE(status.ok());

        CheckTableNoCompact({0, 1});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                     commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        compact            commit
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        CheckTableCompactedWithoutDelete({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                compact                     commit (success)
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        CheckTableCompactedWithoutDelete({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  compact commit (success)
    {
        PrepareForCompact();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        CheckTableCompactedWithoutDelete({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  compact   commit (success)
    {
        PrepareForCompact();

        // compact
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("compact"), TransactionType::kNormal);
        Status status = txn2->Compact(*db_name, *table_name, {0, 1});
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_FALSE(status.ok());

        CheckTableCompactedWithoutDelete({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
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

        Vector<RowID> row_ids;
        for (SizeT segment_id = 0; segment_id < 2; ++segment_id) {
            for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
                row_ids.push_back(RowID(segment_id, row_id));
            }
        }

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTableCompactedWithoutDelete({2});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnDelete, test_delete_and_optimize_index) {

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

    auto CheckTable = [&](const Vector<SegmentID> &segment_ids) {
        auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, segment_ids);
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        SegmentMeta segment_meta(segment_ids_ptr->at(0), *table_meta);

        NewTxnGetVisibleRangeState state;
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_EQ(block_ids_ptr->size(), 3);
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            SizeT row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                offset = range.second;
                row_id += 2;
            }
        }
    };

    auto DropDB = [&] {
        // drop database
        auto *txn5 = new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn5->DropDatabase("db1", ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());
    };

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  optimize  commit (success)
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete      commit (success)
    //    |----------|---------|
    //                    |------------------|----------------|
    //                    t2             optimize           commit (fail)
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                       commit (success)
    //    |----------|--------------------------------|
    //                    |-----------------------|-------------------------|
    //                    t2                  optimize               commit (fail)
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1      delete                                     commit (fail)
    //    |----------|-----------------------------------------------|
    //                    |-------------|-----------------------|
    //                    t2        optimize                commit
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        Status status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                      delete                                    commit (success)
    //    |------------------------------------------|------------------------------------------|
    //                    |----------------------|------------------------------|
    //                    t2                optimize                     commit (success)
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //    t1                                                   delete                                   commit (success)
    //    |------------------------------------------------------|------------------------------------------|
    //                    |----------------------|------------|
    //                    t2                  optimize  commit (success)
    {
        PrepareForCompactAndOptimize();

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                  t1                  delete                                   commit (success)
    //                                                  |--------------------|------------------------------------------|
    //                    |----------------------|---------------|
    //                    t2                  optimize index  commit (success)
    {
        PrepareForCompactAndOptimize();

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);

        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    //                                                           t1                  delete                             commit (success)
    //                                                          |--------------------|------------------------------------------|
    //                    |-----------------|------------|
    //                    t2           optimize   commit (success)
    {
        PrepareForCompactAndOptimize();

        // optimize index
        auto *txn2 = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
        Status status = txn2->OptimizeIndex(*db_name, *table_name, *index_name1, 0);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());

        auto *txn6 = new_txn_mgr->BeginTxn(MakeUnique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (SizeT block_id = 0; block_id < 3; ++block_id) {
            for (SizeT row_id = block_id * 8192 + 1; row_id < (block_id * 8192 + 8192); row_id += 2) {
                row_ids.push_back(RowID(0, row_id));
            }
        }
        status = txn6->Delete(*db_name, *table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        CheckTable({0});

        DropDB();

        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        EXPECT_EQ(new_catalog->GetTableWriteCount(), 0);
    }

    RemoveDbDirs();
}
