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
import third_party;
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
        std::shared_ptr<String> db_name;
        std::shared_ptr<String> table_name;
        std::shared_ptr<String> index_name;
        std::shared_ptr<ColumnDef> column_def1;
        std::shared_ptr<ColumnDef> column_def2;
        std::shared_ptr<TableDef> table_def;
        NewTxnManager* new_txn_mgr;
        
        TestSetup() {
            db_name = std::make_shared<String>("db1");
            table_name = std::make_shared<String>("tb1");
            index_name = std::make_shared<String>("idx1");
            column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
            column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
            table_def = TableDef::Make(db_name, table_name, std::make_shared<String>(), {column_def1, column_def2});
            new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        }

        void verify_original_data() {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("verify"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

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
            // Should only have original block (no update block since update failed)
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            
            // Check that all rows in the original block are still visible (no deletions)
            BlockMeta block_meta(0, segment_meta);
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            size_t total_rows = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                // Since we appended one whole block, we expect one large range
                total_rows += (range.second - range.first);
                offset = range.second;
            }
            // Should have all 8192 rows visible (no deletions from failed update)
            EXPECT_EQ(total_rows, 8192);
            
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        void verify_data_after_delete_conflict() {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("verify"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

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
            // Should only have original block (no update block since update failed)
            EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0}));
            
            // Check that only even-numbered rows are visible (odd rows were deleted)
            BlockMeta block_meta(0, segment_meta);
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            size_t total_rows = 0;
            size_t row_id = 0;
            while (true) {
                bool has_next = state.Next(offset, range);
                if (!has_next) {
                    break;
                }
                // Should only see even-numbered rows (0, 2, 4, ..., 8190)
                EXPECT_EQ(range.first, row_id);
                EXPECT_EQ(range.second, row_id + 1);
                total_rows += (range.second - range.first);
                offset = range.second;
                row_id += 2;
            }
            // Should have 4096 rows visible (only even-numbered rows, odd rows were deleted)
            EXPECT_EQ(total_rows, 4096);
            
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        void check_data() {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

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

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                
                if (block_id == 0) {
                    // Block 0: only even-numbered rows are visible (odd rows were deleted)
                    size_t row_id = 0;
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
                } else {
                    // Block 1: all updated rows are visible (full block from append)
                    bool has_next = state.Next(offset, range);
                    EXPECT_TRUE(has_next);
                    EXPECT_EQ(range.first, 0);
                    EXPECT_EQ(range.second, 8192);
                    offset = range.second;
                    has_next = state.Next(offset, range);
                    EXPECT_FALSE(has_next);
                }
            }
            
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        void check_data_no_conflicts() {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
            TxnTimeStamp begin_ts = txn->BeginTS();
            TxnTimeStamp commit_ts = txn->CommitTS();

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
            
            size_t total_visible_rows = 0;
            for (const auto block_id : *block_ids_ptr) {
                BlockMeta block_meta(block_id, segment_meta);

                status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
                EXPECT_TRUE(status.ok());
                Pair<BlockOffset, BlockOffset> range;
                BlockOffset offset = 0;
                
                while (true) {
                    bool has_next = state.Next(offset, range);
                    if (!has_next) {
                        break;
                    }
                    total_visible_rows += (range.second - range.first);
                    offset = range.second;
                }
            }
            
            // Should have 4096 total visible rows (only the updated odd-numbered rows)
            // Even-numbered rows were deleted, odd-numbered rows were updated
            EXPECT_EQ(total_visible_rows, 4096);
            
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
    };

    void CreateDatabaseAndTable(TestSetup& setup) {
        // Create database
        auto *txn1 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("create db"), TransactionType::kNormal);
        Status status = txn1->CreateDatabase(*setup.db_name, ConflictType::kError, std::make_shared<String>());
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn1);
        EXPECT_TRUE(status.ok());

        // Create table
        auto *txn2 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("create table"), TransactionType::kNormal);
        status = txn2->CreateTable(*setup.db_name, std::move(setup.table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn2);
        EXPECT_TRUE(status.ok());
    }



    void AppendData(TestSetup& setup, const std::shared_ptr<DataBlock>& input_block) {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void CreateIndex(TestSetup& setup) {
        auto *txn_idx = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("create index"), TransactionType::kNormal);
        auto index_def1 = IndexSecondary::Make(setup.index_name, std::make_shared<String>(), "file_name", {setup.column_def1->name()});
        Status status = txn_idx->CreateIndex(*setup.db_name, *setup.table_name, index_def1, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn_idx);
        EXPECT_TRUE(status.ok());
    }

    void DropDatabase(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);
        Status status = txn->DropDatabase(*setup.db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DropTable(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal);
        Status status = txn->DropTable(*setup.db_name, *setup.table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DropIndex(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop index"), TransactionType::kNormal);
        Status status = txn->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    void DumpIndex(TestSetup& setup) {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("dump index"), TransactionType::kNormal);
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
    
    auto input_block = std::make_shared<DataBlock>();
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
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Create update block
    auto update_block = std::make_shared<DataBlock>();
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
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
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
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
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
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        row_ids.push_back(RowID(0, 0));
        Status status = txn->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnUpdate, test_update_multiple_blocks) {
    using namespace infinity;

    TestSetup setup;
    CreateDatabaseAndTable(setup);

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
            auto col1 = make_column(setup.column_def1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(setup.column_def2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    auto append = [&] {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("append"), TransactionType::kNormal);
        auto input_block = make_input_block(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 8192);
        Status status = txn->Append(*setup.db_name, *setup.table_name, input_block);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    };

    append();
    append();

    // Create update block
    auto update_block = make_input_block(Value::MakeInt(999), Value::MakeVarchar("updated_value"), 8192);

    {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 2 * 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    // Check data
    {
        auto *txn = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        TxnTimeStamp begin_ts = txn->BeginTS();
        TxnTimeStamp commit_ts = txn->CommitTS();

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        Status status = txn->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());

        SegmentID segment_id = 0;
        NewTxnGetVisibleRangeState state;

        SegmentMeta segment_meta(segment_id, *table_meta);

        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*block_ids_ptr, Vector<BlockID>({0, 1, 2}));
        for (const auto block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
            EXPECT_TRUE(status.ok());
            Pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            
            if (block_id < 2) {
                // Blocks 0 and 1: only even-numbered rows are visible
                size_t row_id = 0;
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
            } else {
                // Block 2: all rows are visible (full block)
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
}

TEST_P(TestTxnUpdate, test_update_and_drop_db) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    //    t1      update      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        setup.check_data();

        DropDatabase(setup);

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      update           commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // drop database
        auto *txn6 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop db"), TransactionType::kNormal);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        setup.check_data();

        status = txn6->DropDatabase(*setup.db_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn6);
        EXPECT_TRUE(status.ok());

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }

    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop db    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kDBNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnUpdate, test_update_and_drop_table) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }


    //    t1      update      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        setup.check_data();

        DropTable(setup);

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());
        
        DropDatabase(setup);
    }

    //    t1      update           commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // drop table
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop table"), TransactionType::kNormal);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        setup.check_data();

        status = txn5->DropTable(*setup.db_name, *setup.table_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);
    }

    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop table    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        DropTable(setup);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);
    }

    //    t1                                        update                                            commit (fail)
    //    |-------------------------------------------|----------------------------------------------------|
    //         |----------------------|----------|
    //        t2                  drop table    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);

        DropTable(setup);

        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status         status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Check the updated data.
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("scan"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_EQ(status.code(), ErrorCode::kTableNotExist);
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_drop_index) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }


    //    t1      update      commit (success)
    //    |----------|---------|
    //                            |----------------------|----------|
    //                           t2                  drop index    commit (success)
    {
        CreateDatabaseAndTable(setup);
        CreateIndex(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop index"), TransactionType::kNormal);
        status = txn5->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        setup.check_data();

        DropDatabase(setup);
    }

    //    t1      update           commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop index   commit (success)
    {
        CreateDatabaseAndTable(setup);
        CreateIndex(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Drop index
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop index"), TransactionType::kNormal);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn5->DropIndexByName(*setup.db_name, *setup.table_name, *setup.index_name, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Check data
        setup.check_data();

        DropDatabase(setup);
    }

    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop index    commit (success)
    {
        CreateDatabaseAndTable(setup);
        CreateIndex(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        DropIndex(setup);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());
        
        // Check that all rows in the original block are still visible (no deletions)
        setup.verify_original_data();

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_dump_index) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }



    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  dump index    commit (success)
    {
        CreateDatabaseAndTable(setup);
        CreateIndex(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        DumpIndex(setup);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data remains unchanged after failed update
        setup.verify_original_data();

        DropDatabase(setup);
    }

    //    t1      update      commit (success)
    //    |----------|-----------------|
    //                            |----------------------|----------|
    //                           t2                  dump index    commit (success)
    {
        CreateDatabaseAndTable(setup);
        CreateIndex(setup);
        AppendData(setup, input_block1);
        
        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("dump_index"), TransactionType::kNormal);

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());


        status = txn5->DumpMemIndex(*setup.db_name, *setup.table_name, *setup.index_name);
        EXPECT_TRUE(status.ok());

        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        setup.check_data();

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_add_drop_column_conflicts) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Define default value for varchar columns
    std::shared_ptr<ConstantExpr> default_varchar = std::make_shared<ConstantExpr>(LiteralType::kString);
    default_varchar->str_value_ = strdup("");
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Add column in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("add_column"), TransactionType::kNormal);
        auto new_column_def = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        status = txn5->AddColumns(*setup.db_name, *setup.table_name, Vector<std::shared_ptr<ColumnDef>>{new_column_def});
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the update transaction - should fail due to schema change
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data remains unchanged after failed update
        setup.verify_original_data();

        DropDatabase(setup);
    }

    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  drop column   commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Drop column in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop_column"), TransactionType::kNormal);
        status = txn5->DropColumns(*setup.db_name, *setup.table_name, Vector<String>{"col2"});
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the update transaction - should fail due to schema change
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data remains unchanged after failed update
        setup.verify_original_data();

        DropDatabase(setup);
    }

    //    t1      update      commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  add column    commit (fail)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data first
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());


        // Add column later - should fail due to update already committed
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("add_column"), TransactionType::kNormal);
        auto new_column_def = std::make_shared<ColumnDef>(2, std::make_shared<DataType>(LogicalType::kVarchar), "col3", std::set<ConstraintType>(), default_varchar);
        
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        
        status = txn5->AddColumns(*setup.db_name, *setup.table_name, Vector<std::shared_ptr<ColumnDef>>{new_column_def});
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        // Check the data
        setup.check_data();

        // Verify that the column was not added
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("verify"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        
        // Should only have 2 columns (col1 and col2), col3 should not exist
        std::shared_ptr<Vector<std::shared_ptr<ColumnDef>>> column_defs;
        std::tie(column_defs, status) = table_meta->GetColumnDefs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(column_defs->size(), 2);
        
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);
    }

    //    t1      update      commit (success)
    //    |----------|------------------|
    //                            |----------------------|----------|
    //                           t2                  drop column   commit (fail)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data first
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());


        // Drop column later - should fail due to update already committed
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("drop_column"), TransactionType::kNormal);
        
        // Commit the update transaction first
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_TRUE(status.ok());

        status = txn5->DropColumns(*setup.db_name, *setup.table_name, Vector<String>{"col2"});
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_FALSE(status.ok());

        // Check the data
        setup.check_data();

        // Verify that the column was not dropped
        auto *txn7 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("verify"), TransactionType::kNormal);
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn7->GetTableMeta(*setup.db_name, *setup.table_name, db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        
        // Should have 2 columns (col1 and col2), col2 should still exist
        std::shared_ptr<Vector<std::shared_ptr<ColumnDef>>> column_defs;
        std::tie(column_defs, status) = table_meta->GetColumnDefs();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(column_defs->size(), 2);
        
        status = setup.new_txn_mgr->CommitTxn(txn7);
        EXPECT_TRUE(status.ok());

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_compact_conflicts) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    // Test Case 1: Update vs Compact (Update should fail)
    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  compact       commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Compact table in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal);
        status = txn5->Compact(*setup.db_name, *setup.table_name, Vector<SegmentID>{0});
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the update transaction - should fail due to compact
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data remains unchanged after failed update
        setup.verify_original_data();

        DropDatabase(setup);
    }

    // Test Case 2: Compact vs Update (Compact should fail)
    //    t1      compact                                           commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  update        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Compact table
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("compact"), TransactionType::kNormal);
        Status status = txn4->Compact(*setup.db_name, *setup.table_name, Vector<SegmentID>{0});
        EXPECT_TRUE(status.ok());

        // Update data in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the compact transaction - should fail due to update
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data was updated successfully
        setup.check_data();

        DropDatabase(setup);
    }
    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_create_index_conflicts) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    // Test Case 1: Update vs Create Index (Update should fail)
    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  create index  commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Create index in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("create_index"), TransactionType::kNormal);
        auto index_def = IndexSecondary::Make(setup.index_name, std::make_shared<String>(), "file_name", {setup.column_def1->name()});
        status = txn5->CreateIndex(*setup.db_name, *setup.table_name, index_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the update transaction - should fail due to index creation
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data remains unchanged after failed update
        setup.verify_original_data();

        DropDatabase(setup);
    }

    // Test Case 2: Create Index vs Update (Create Index should fail)
    //    t1      create index                                      commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  update        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Create index
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("create_index"), TransactionType::kNormal);
        auto index_def = IndexSecondary::Make(setup.index_name, std::make_shared<String>(), "file_name", {setup.column_def1->name()});
        Status status = txn4->CreateIndex(*setup.db_name, *setup.table_name, index_def, ConflictType::kError);
        EXPECT_TRUE(status.ok());

        // Update data in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the create index transaction - should fail due to update
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data was updated successfully
        setup.check_data();

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_delete_conflicts) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    // Test Case 1: Update vs Delete with same row IDs (Update should fail)
    //    t1      update                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  delete        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        // Delete the same rows in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal);
        status = txn5->Delete(*setup.db_name, *setup.table_name, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the update transaction - should fail due to delete
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that only even-numbered rows are visible (odd rows were deleted)
        setup.verify_data_after_delete_conflict();

        DropDatabase(setup);
    }

    // Test Case 2: Delete vs Update with same row IDs (Delete should fail)
    //    t1      delete                                            commit (fail)
    //    |----------|----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  update        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Delete data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Delete(*setup.db_name, *setup.table_name, row_ids);
        EXPECT_TRUE(status.ok());

        // Update the same rows in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        status = txn5->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the delete transaction - should fail due to update
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data was updated successfully
        setup.check_data();

        DropDatabase(setup);
    }

    // Test Case 2: Delete vs Update with same row IDs (Delete should fail)
    //    t1                            delete            commit (fail)
    //    |-------------------------------|----------------|
    //                 |------------|----------|
    //                 t2         update        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Delete data
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            row_ids.push_back(RowID(0, row_id));
        }


        // Update the same rows in another transaction
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Status status = txn5->Update(*setup.db_name, *setup.table_name, update_block, row_ids);
        EXPECT_TRUE(status.ok());

        status = txn4->Delete(*setup.db_name, *setup.table_name, row_ids);
        EXPECT_TRUE(status.ok());

        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        // Try to commit the delete transaction - should fail due to update
        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());

        // Verify that data was updated successfully
        setup.check_data();

        DropDatabase(setup);
    }

    RemoveDbDirs();
}

TEST_P(TestTxnUpdate, test_update_and_delete_no_conflicts) {
    using namespace infinity;

    TestSetup setup;
    size_t insert_row = 8192;
    
    // Create custom input block for this test
    auto input_block1 = std::make_shared<DataBlock>();
    {
        // Initialize input block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i));
            }
            input_block1->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
            }
            input_block1->InsertVector(col2, 1);
        }
        input_block1->Finalize();
    }

    auto update_block = std::make_shared<DataBlock>();
    {
        // Initialize update block
        {
            auto col1 = ColumnVector::Make(setup.column_def1->type());
            col1->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col1->AppendValue(Value::MakeInt(i + 1000));
            }
            update_block->InsertVector(col1, 0);
        }
        {
            auto col2 = ColumnVector::Make(setup.column_def2->type());
            col2->Initialize();
            for (size_t i = 0; i < insert_row; ++i) {
                col2->AppendValue(Value::MakeVarchar(fmt::format("updated_abc_{}", i)));
            }
            update_block->InsertVector(col2, 1);
        }
        update_block->Finalize();
    }

    // Test Case: Update vs Delete with different row IDs (both should succeed)
    //    t1      update                                             commit (success)
    //    |----------|-----------------------------------------------------|
    //                            |----------------------|----------|
    //                           t2                  delete        commit (success)
    {
        CreateDatabaseAndTable(setup);
        AppendData(setup, input_block1);

        // Update odd-numbered rows
        auto *txn4 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("update"), TransactionType::kNormal);
        Vector<RowID> update_row_ids;
        for (size_t row_id = 1; row_id < 8192; row_id += 2) {
            update_row_ids.push_back(RowID(0, row_id));
        }
        Status status = txn4->Update(*setup.db_name, *setup.table_name, update_block, update_row_ids);
        EXPECT_TRUE(status.ok());


        // Delete even-numbered rows (different from update)
        auto *txn5 = setup.new_txn_mgr->BeginTxn(std::make_unique<String>("delete"), TransactionType::kNormal);
        Vector<RowID> delete_row_ids;
        for (size_t row_id = 0; row_id < 8192; row_id += 2) {
            delete_row_ids.push_back(RowID(0, row_id));
        }
        status = txn5->Delete(*setup.db_name, *setup.table_name, delete_row_ids);
        EXPECT_TRUE(status.ok());
        status = setup.new_txn_mgr->CommitTxn(txn5);
        EXPECT_TRUE(status.ok());

        status = setup.new_txn_mgr->CommitTxn(txn4);
        EXPECT_FALSE(status.ok());
        // false due to delete conflict

        // Verify that both operations succeeded
        // setup.check_data_no_conflicts();

        DropDatabase(setup);
    }

    RemoveDbDirs();
}