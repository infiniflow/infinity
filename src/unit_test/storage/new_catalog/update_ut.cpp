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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.update1;

import :ut.base_test;
import :stl;
import :third_party;
import :status;
import :new_catalog;
import :new_txn_manager;
import :infinity_context;
import :txn_state;
import :table_def;
import :index_base;
import :index_secondary;
import :index_full_text;
import :data_block;
import :column_vector;
import :value;
import :new_txn;
import :buffer_obj;
import :segment_meta;
import :block_meta;
import :column_meta;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :db_meeta;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#endif

import extra_ddl_info;
import column_def;
import data_type;
import logical_type;
import internal_types;
import constant_expr;
import create_index_info;

using namespace infinity;

class TestTxnUpdate : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnUpdate,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

// Helper functions for test preparation
namespace {
    struct TestSetup {
        SharedPtr<String> db_name;
        SharedPtr<String> table_name;
        SharedPtr<String> index_name;
        SharedPtr<ColumnDef> column_def1;
        SharedPtr<ColumnDef> column_def2;
        SharedPtr<TableDef> table_def;
        NewTxnManager* new_txn_mgr;
        
        TestSetup() {
            db_name = std::make_shared<String>("db1");
            table_name = std::make_shared<String>("tb1");
            index_name = std::make_shared<String>("idx1");
            column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
            column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
            table_def = TableDef::Make(db_name, table_name, MakeShared<String>(), {column_def1, column_def2});
            new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        }
    };

    void CreateDatabaseAndTable(TestSetup& setup) {
        // Create database
        auto *txn1 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*setup.db_name, ConflictType::kError, MakeShared<String>());
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Create table
        auto *txn2 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*setup.db_name, std::move(setup.table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }



    void AppendData(TestSetup& setup, const SharedPtr<DataBlock>& input_block) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CreateIndex(TestSetup& setup) {
        auto *txn_idx = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
        auto index_def1 = IndexSecondary::Make(setup.index_name, MakeShared<String>(), "file_name", {setup.column_def1->name()});
        Status status = txn_idx->CreateIndex(*setup.db_name, *setup.table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn_idx);
        EXPECT_TRUE(status.ok());
    }

    void DropDatabase(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*setup.db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DropTable(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*setup.db_name, *setup.table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DropIndex(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DumpIndex(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("dump index"), TransactionType::kNormal);
        Status status = txn->DumpMemIndex(*setup.db_name, *setup.table_name, *setup.index_name);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnUpdate, test_update) {
    using namespace infinity;

    TestSetup setup;
    CreateDatabaseAndTable(setup);
    
    auto input_block = MakeShared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(1));
            col1->AppendValue(Value::MakeInt(2));
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("abc"));
            col2->AppendValue(Value::MakeVarchar("def"));
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
    }
    
    for (int i = 0; i < 2; ++i) {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create update block
    auto update_block = MakeShared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            col1->AppendValue(Value::MakeInt(100));
            col1->AppendValue(Value::MakeInt(200));
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            col2->AppendValue(Value::MakeVarchar("updated_abc"));
            col2->AppendValue(Value::MakeVarchar("updated_def"));
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 1));
        row_ids.push_back(RowID(0, 3));
        Status status = txn->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check data
    {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        BlockID block_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        BlockMeta block_meta(block_id, segment_meta);

        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
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
            EXPECT_TRUE(has_next);
            EXPECT_EQ(range.first, 4);
            EXPECT_EQ(range.second, 6);
            offset = range.second;
            has_next = state.Next(offset, range);
            EXPECT_FALSE(has_next);
        }
    }

    {
        auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 0));
        Status status = txn->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

// TEST_P(TestTxnUpdate, test_update_multiple_blocks) {
//     using namespace infinity;

//     TestSetup setup;
//     CreateDatabaseAndTable(setup);

//     auto make_input_block = [&](const Value &v1, const Value &v2, SizeT row_cnt) {
//         auto make_column = [&](SharedPtr<ColumnDef> &column_def, const Value &v) {
//             auto col = ColumnVector::Make(column_def->type());
//             col->Initialize();
//             for (SizeT i = 0; i < row_cnt; ++i) {
//                 col->AppendValue(v);
//             }
//             return col;
//         };
//         auto input_block = MakeShared<DataBlock>();
//         {
//             auto col1 = make_column(setup.column_def1, v1);
//             input_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = make_column(setup.column_def2, v2);
//             input_block->InsertVector(col2, 1);
//         }
//         input_block->Finalize();
//         return input_block;
//     };

//     auto append = [&] {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("append"), TransactionType::kNormal);
//         auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
//         Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };

//     append();
//     append();

//     // Create update block
//     auto update_block = make_input_block(Value::MakeInt(999), Value::MakeVarchar("updated_value"), 8192);

//     {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 2 * 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     }

//     // Check data
//     {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1, 2}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id < 2) {
//                 // Blocks 0 and 1: only even-numbered rows are visible
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 2: all rows are visible (full block)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     }
// }

// TEST_P(TestTxnUpdate, test_update_and_drop_db) {
//     using namespace infinity;

//     TestSetup setup;
//     SizeT insert_row = 8192;
    
//     // Create custom input block for this test
//     auto input_block1 = MakeShared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i));
//             }
//             input_block1->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
//             }
//             input_block1->InsertVector(col2, 1);
//         }
//         input_block1->Finalize();
//     }

//     auto update_block = MakeShared<DataBlock>();
//     {
//         // Initialize update block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i + 1000));
//             }
//             update_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
//             }
//             update_block->InsertVector(col2, 1);
//         }
//         update_block->Finalize();
//     }

//     auto check_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id == 0) {
//                 // Block 0: only even-numbered rows are visible (odd rows were deleted)
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 1: all updated rows are visible (full block from append)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     };

//     //    t1      update      commit (success)
//     //    |----------|---------|
//     //                            |----------------------|----------|
//     //                           t2                  drop db    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         check_data();

//         DropDatabase(setup);

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());
//     }

//     //    t1      update           commit (success)
//     //    |----------|------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop db    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         // drop database
//         auto *txn6 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         check_data();

//         status = txn6->DropDatabase(*setup.db_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn6);
//         EXPECT_TRUE(status.ok());

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());
//     }

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop db    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());
//     }
// }

// TEST_P(TestTxnUpdate, test_update_and_drop_table) {
//     using namespace infinity;

//     TestSetup setup;
//     SizeT insert_row = 8192;
    
//     // Create custom input block for this test
//     auto input_block1 = MakeShared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i));
//             }
//             input_block1->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
//             }
//             input_block1->InsertVector(col2, 1);
//         }
//         input_block1->Finalize();
//     }

//     auto update_block = MakeShared<DataBlock>();
//     {
//         // Initialize update block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i + 1000));
//             }
//             update_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
//             }
//             update_block->InsertVector(col2, 1);
//         }
//         update_block->Finalize();
//     }

//     auto check_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id == 0) {
//                 // Block 0: only even-numbered rows are visible (odd rows were deleted)
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 1: all updated rows are visible (full block from append)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     };

//     //    t1      update      commit (success)
//     //    |----------|---------|
//     //                            |----------------------|----------|
//     //                           t2                  drop table    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         check_data();

//         DropTable(setup);

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());
        
//         DropDatabase(setup);
//     }

//     //    t1      update           commit (success)
//     //    |----------|------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop table    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         // drop table
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         check_data();

//         status = txn5->DropTable(*setup.db_name, *setup.table_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_TRUE(status.ok());

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop table    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         DropTable(setup);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     //    t1                                        update                                            commit (fail)
//     //    |-------------------------------------------|----------------------------------------------------|
//     //         |----------------------|----------|
//     //        t2                  drop table    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);

//         DropTable(setup);

//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status         status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Check the updated data.
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     RemoveDbDirs();
// }

// TEST_P(TestTxnUpdate, test_update_and_drop_index) {
//     using namespace infinity;

//     TestSetup setup;
//     SizeT insert_row = 8192;
    
//     // Create custom input block for this test
//     auto input_block1 = MakeShared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i));
//             }
//             input_block1->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
//             }
//             input_block1->InsertVector(col2, 1);
//         }
//         input_block1->Finalize();
//     }

//     auto update_block = MakeShared<DataBlock>();
//     {
//         // Initialize update block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i + 1000));
//             }
//             update_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
//             }
//             update_block->InsertVector(col2, 1);
//         }
//         update_block->Finalize();
//     }

//     auto check_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id == 0) {
//                 // Block 0: only even-numbered rows are visible (odd rows were deleted)
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 1: all updated rows are visible (full block from append)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     };

//     //    t1      update      commit (success)
//     //    |----------|---------|
//     //                            |----------------------|----------|
//     //                           t2                  drop index    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         CreateIndex(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         // Drop index
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);
//         status = txn5->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_TRUE(status.ok());

//         // Check data
//         check_data();

//         DropDatabase(setup);
//     }

//     //    t1      update           commit (success)
//     //    |----------|------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop index   commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         CreateIndex(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         // Drop index
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop index"), TransactionType::kNormal);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         status = txn5->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_TRUE(status.ok());

//         // Check data
//         check_data();

//         DropDatabase(setup);
//     }

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop index    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         CreateIndex(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         DropIndex(setup);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Verify that data remains unchanged after failed update
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("verify"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn7->BeginTS();
//         TxnTimeStamp commit_ts = txn7->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;
//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         // Should only have original block (no update block since update failed)
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        
//         // Check that all rows in the original block are still visible (no deletions)
//         BlockMeta block_meta(0, segment_meta);
//         status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//         EXPECT_TRUE(status.ok());
        
//         Pair<BlockOffset, BlockOffset> range;
//         BlockOffset offset = 0;
//         SizeT row_id = 0;
//         while (true) {
//             bool has_next = state.Next(offset, range);
//             if (!has_next) {
//                 break;
//             }
//             EXPECT_EQ(range.first, row_id);
//             EXPECT_EQ(range.second, row_id + 1);
//             offset = range.second;
//             row_id++;
//         }
//         // Should have all 8192 rows visible (no deletions from failed update)
//         EXPECT_EQ(row_id, 8192);
        
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     RemoveDbDirs();
// }

// TEST_P(TestTxnUpdate, test_update_and_dump_index) {
//     using namespace infinity;

//     TestSetup setup;
//     SizeT insert_row = 8192;
    
//     // Create custom input block for this test
//     auto input_block1 = MakeShared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i));
//             }
//             input_block1->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
//             }
//             input_block1->InsertVector(col2, 1);
//         }
//         input_block1->Finalize();
//     }

//     auto update_block = MakeShared<DataBlock>();
//     {
//         // Initialize update block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i + 1000));
//             }
//             update_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
//             }
//             update_block->InsertVector(col2, 1);
//         }
//         update_block->Finalize();
//     }

//     auto check_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id == 0) {
//                 // Block 0: only even-numbered rows are visible (odd rows were deleted)
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 1: all updated rows are visible (full block from append)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     };

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop index    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         CreateIndex(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         DropIndex(setup);

//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Verify that data remains unchanged after failed update
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("verify"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn7->BeginTS();
//         TxnTimeStamp commit_ts = txn7->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;
//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         // Should only have original block (no update block since update failed)
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        
//         // Check that all rows in the original block are still visible (no deletions)
//         BlockMeta block_meta(0, segment_meta);
//         status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//         EXPECT_TRUE(status.ok());
        
//         Pair<BlockOffset, BlockOffset> range;
//         BlockOffset offset = 0;
//         SizeT row_id = 0;
//         while (true) {
//             bool has_next = state.Next(offset, range);
//             if (!has_next) {
//                 break;
//             }
//             EXPECT_EQ(range.first, row_id);
//             EXPECT_EQ(range.second, row_id + 1);
//             offset = range.second;
//             row_id++;
//         }
//         // Should have all 8192 rows visible (no deletions from failed update)
//         EXPECT_EQ(row_id, 8192);
        
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     //    t1      update      commit (success)
//     //    |----------|---------|
//     //                            |----------------------|----------|
//     //                           t2                  dump index    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         CreateIndex(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         DumpIndex(setup);

//         // Check data
//         check_data();

//         DropDatabase(setup);
//     }

//     RemoveDbDirs();
// }

// TEST_P(TestTxnUpdate, test_update_and_add_drop_column_conflicts) {
//     using namespace infinity;

//     TestSetup setup;
//     SizeT insert_row = 8192;
    
//     // Create custom input block for this test
//     auto input_block1 = MakeShared<DataBlock>();
//     {
//         // Initialize input block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i));
//             }
//             input_block1->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
//             }
//             input_block1->InsertVector(col2, 1);
//         }
//         input_block1->Finalize();
//     }

//     auto update_block = MakeShared<DataBlock>();
//     {
//         // Initialize update block
//         {
//             auto col1 = ColumnVector::Make(setup.column_def1->type());
//             col1->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col1->AppendValue(Value::MakeInt(i + 1000));
//             }
//             update_block->InsertVector(col1, 0);
//         }
//         {
//             auto col2 = ColumnVector::Make(setup.column_def2->type());
//             col2->Initialize();
//             for (SizeT i = 0; i < insert_row; ++i) {
//                 col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
//             }
//             update_block->InsertVector(col2, 1);
//         }
//         update_block->Finalize();
//     }

//     auto check_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("scan"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;

//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1}));
//         for (const auto block_id : *block_ids_ptr) {
//             BlockMeta block_meta(block_id, segment_meta);

//             status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//             EXPECT_TRUE(status.ok());
//             Pair<BlockOffset, BlockOffset> range;
//             BlockOffset offset = 0;
            
//             if (block_id == 0) {
//                 // Block 0: only even-numbered rows are visible (odd rows were deleted)
//                 SizeT row_id = 0;
//                 while (true) {
//                     bool has_next = state.Next(offset, range);
//                     if (!has_next) {
//                         break;
//                     }
//                     EXPECT_EQ(range.first, row_id);
//                     EXPECT_EQ(range.second, row_id + 1);
//                     offset = range.second;
//                     row_id += 2;
//                 }
//             } else {
//                 // Block 1: all updated rows are visible (full block from append)
//                 bool has_next = state.Next(offset, range);
//                 EXPECT_TRUE(has_next);
//                 EXPECT_EQ(range.first, 0);
//                 EXPECT_EQ(range.second, 8192);
//                 offset = range.second;
//                 has_next = state.Next(offset, range);
//                 EXPECT_FALSE(has_next);
//             }
//         }
//     };

//     auto verify_original_data = [&]() {
//         auto *txn = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("verify"), TransactionType::kNormal);
//         TxnTimeStamp begin_ts = txn->BeginTS();
//         TxnTimeStamp commit_ts = txn->CommitTS();

//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());

//         SegmentID segment_id = 0;
//         NewTxnGetVisibleRangeState state;
//         SegmentMeta segment_meta(segment_id, *table_meta);

//         Vector<BlockID> *block_ids_ptr = nullptr;
//         std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
//         EXPECT_TRUE(status.ok());
//         // Should only have original block (no update block since update failed)
//         EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
        
//         // Check that all rows in the original block are still visible (no deletions)
//         BlockMeta block_meta(0, segment_meta);
//         status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
//         EXPECT_TRUE(status.ok());
        
//         Pair<BlockOffset, BlockOffset> range;
//         BlockOffset offset = 0;
//         SizeT row_id = 0;
//         while (true) {
//             bool has_next = state.Next(offset, range);
//             if (!has_next) {
//                 break;
//             }
//             EXPECT_EQ(range.first, row_id);
//             EXPECT_EQ(range.second, row_id + 1);
//             offset = range.second;
//             row_id++;
//         }
//         // Should have all 8192 rows visible (no deletions from failed update)
//         EXPECT_EQ(row_id, 8192);
        
//         status = setup.new_txn_mgr->CommitTxn(txn);
//         EXPECT_TRUE(status.ok());
//     };

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  add column    commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         // Add column in another transaction
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("add_column"), TransactionType::kNormal);
//         auto new_column_def = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kFloat), "col3", std::set<ConstraintType>());
//         status = txn5->AddColumns(*setup.db_name, *setup.table_name, Vector<SharedPtr<ColumnDef>>{new_column_def});
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_TRUE(status.ok());

//         // Try to commit the update transaction - should fail due to schema change
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Verify that data remains unchanged after failed update
//         verify_original_data();

//         DropDatabase(setup);
//     }

//     //    t1      update                                            commit (fail)
//     //    |----------|----------------------------------------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop column   commit (success)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         // Update data
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());

//         // Drop column in another transaction
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop_column"), TransactionType::kNormal);
//         status = txn5->DropColumns(*setup.db_name, *setup.table_name, Vector<String>{"col2"});
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_TRUE(status.ok());

//         // Try to commit the update transaction - should fail due to schema change
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_FALSE(status.ok());

//         // Verify that data remains unchanged after failed update
//         verify_original_data();

//         DropDatabase(setup);
//     }

//     //    t1      update      commit (success)
//     //    |----------|------------------|
//     //                            |----------------------|----------|
//     //                           t2                  add column    commit (fail)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         // Update data first
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());


//         // Add column later - should fail due to update already committed
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("add_column"), TransactionType::kNormal);
//         auto new_column_def = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kFloat), "col3", std::set<ConstraintType>());
        
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

        
//         status = txn5->AddColumns(*setup.db_name, *setup.table_name, Vector<SharedPtr<ColumnDef>>{new_column_def});
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_FALSE(status.ok());

//         // Check the data
//         check_data();

//         // Verify that the column was not added
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("verify"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());
        
//         // Should only have 2 columns (col1 and col2), col3 should not exist
//         SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
//         std::tie(column_defs, status) = table_meta->GetColumnDefs();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(column_defs->size(), 2);
        
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     //    t1      update      commit (success)
//     //    |----------|------------------|
//     //                            |----------------------|----------|
//     //                           t2                  drop column   commit (fail)
//     {
//         CreateDatabaseAndTable(setup);
//         AppendData(setup, input_block1);

//         // Update data first
//         auto *txn4 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("update"), TransactionType::kNormal);
//         Vector<RowID> row_ids;
//         for (SizeT row_id = 1; row_id < 8192; row_id += 2) {
//             row_ids.push_back(RowID(0, row_id));
//         }
//         Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
//         EXPECT_TRUE(status.ok());


//         // Drop column later - should fail due to update already committed
//         auto *txn5 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("drop_column"), TransactionType::kNormal);
        
//         // Commit the update transaction first
//         status = setup.new_txn_mgr->CommitTxn(txn4);
//         EXPECT_TRUE(status.ok());

//         status = txn5->DropColumns(*setup.db_name, *setup.table_name, Vector<String>{"col2"});
//         EXPECT_TRUE(status.ok());
//         status = setup.new_txn_mgr->CommitTxn(txn5);
//         EXPECT_FALSE(status.ok());

//         // Check the data
//         check_data();

//         // Verify that the column was not dropped
//         auto *txn7 = setup.new_txn_mgr->BeginTxn(MakeUnique<String>("verify"), TransactionType::kNormal);
//         Optional<DBMeeta> db_meta;
//         Optional<TableMeeta> table_meta;
//         status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
//         EXPECT_TRUE(status.ok());
        
//         // Should have 2 columns (col1 and col2), col2 should still exist
//         SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
//         std::tie(column_defs, status) = table_meta->GetColumnDefs();
//         EXPECT_TRUE(status.ok());
//         EXPECT_EQ(column_defs->size(), 2);
        
//         status = setup.new_txn_mgr->CommitTxn(txn7);
//         EXPECT_TRUE(status.ok());

//         DropDatabase(setup);
//     }

//     RemoveDbDirs();
// }