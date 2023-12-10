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

#include "unit_test/base_test.h"
#include <string>

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;
import new_catalog;
import buffer_manager;
import table_collection_entry;
import column_vector;
import column_buffer;
import block_column_entry;
import table_collection_type;
import meta_state;
import status;

class TableCollectionEntryTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        BaseTest::TearDown();
    }
};

TEST_F(TableCollectionEntryTest, test1) {
    using namespace infinity;

    SharedPtr<String> table_dir = MakeShared<String>("/tmp/infinity/table");
    SharedPtr<TableDef> table_def{};

    // Define columns
    {
        Vector<SharedPtr<ColumnDef>> columns;
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }

        table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("t1"), columns);

        EXPECT_EQ(*table_def->table_name(), "t1");
        EXPECT_EQ(table_def->column_count(), 2);
        EXPECT_EQ(table_def->GetColIdByName("tiny_int_col"), 0);
        EXPECT_EQ(table_def->GetColIdByName("big_int_col"), 1);
    }

    SharedPtr<TableCollectionEntry> table_entry =
        MakeShared<TableCollectionEntry>(table_dir, table_def->table_name(), table_def->columns(), TableCollectionType::kTableEntry, nullptr, 0, 0);
}

TEST_F(TableCollectionEntryTest, test2) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
    BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->CreateTxn();

    // Txn1: Begin, OK
    new_txn->Begin();

    // Txn1: Create db1, OK
    BaseEntry* base_entry{nullptr};
    Status status = new_txn->CreateDatabase("db1", ConflictType::kError, base_entry);
    EXPECT_NE(base_entry, nullptr);
    EXPECT_EQ(base_entry->Committed(), false);

    // Txn1: Create tbl1, OK
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    UniquePtr<TableDef> tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
    BaseEntry* base_table_entry{nullptr};
    Status s1 = new_txn->CreateTable("db1", Move(tbl1_def), ConflictType::kError, base_table_entry);
    EXPECT_TRUE(s1.ok());
    EXPECT_NE(base_table_entry, nullptr);

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    {
        // Txn2: Create, OK
        new_txn = txn_mgr->CreateTxn();

        // Txn2: Begin, OK
        new_txn->Begin();

        // Txn2: Get db1, OK
        auto [base_table_entry, s2] = new_txn->GetTableByName("db1", "tbl1");
        EXPECT_TRUE(s2.ok());
        EXPECT_NE(base_table_entry, nullptr);

        // Prepare the input data block
        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

        SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();

        SizeT row_count = DEFAULT_VECTOR_SIZE;
        input_block->Init(column_types, row_count);

        for (SizeT i = 0; i < row_count; ++i) {
            input_block->AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
        }

        for (SizeT i = 0; i < row_count; ++i) {
            input_block->AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
        }

        for (SizeT i = 0; i < row_count; ++i) {
            input_block->AppendValue(2, Value::MakeDouble(static_cast<f64>(i)));
        }

        input_block->Finalize();
        EXPECT_EQ(input_block->Finalized(), true);

        new_txn->Append("db1", "tbl1", input_block);

        // Txn2: Commit, OK
        txn_mgr->CommitTxn(new_txn);
    }

    {
        // Txn2: Create, OK
        new_txn = txn_mgr->CreateTxn();

        // Txn2: Begin, OK
        new_txn->Begin();

        {
            // Get column 0 and column 2 from global storage;
            Vector<ColumnID> column_ids{0, 1, 2};

            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();
            new_txn->GetMetaTableState(read_table_meta.get(), "db1", "tbl1", column_ids);
            EXPECT_EQ(read_table_meta->local_blocks_.size(), 0);
            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
            for (const auto &segment_pair : read_table_meta->segment_map_) {
                EXPECT_EQ(segment_pair.first, 0);
                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
                EXPECT_EQ(segment_pair.second.segment_entry_->block_entries_.size(), 1);
                EXPECT_EQ(segment_pair.second.block_map_.size(), 1);
                for (const auto &block_pair : segment_pair.second.block_map_) {
                    //                    EXPECT_EQ(block_pair.first, 0);
                    EXPECT_NE(block_pair.second.block_entry_, nullptr);

                    EXPECT_EQ(block_pair.second.column_data_map_.size(), 3);
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(0));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(1));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(2));

                    BlockColumnEntry *column0 = block_pair.second.column_data_map_.at(0).block_column_;
                    BlockColumnEntry *column1 = block_pair.second.column_data_map_.at(1).block_column_;
                    BlockColumnEntry *column2 = block_pair.second.column_data_map_.at(2).block_column_;

                    SizeT row_count = block_pair.second.block_entry_->row_count_;
                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_mgr);
                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col0_ptr[row], (i8)(row));
                    }

                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_mgr);
                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col1_ptr[row], (i64)(row));
                    }

                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_mgr);
                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
                    }
                }
            }
        }

        // Append more data into local storage
        {
            // Prepare the input data block
            Vector<SharedPtr<DataType>> column_types;
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

            SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();

            SizeT row_count = DEFAULT_VECTOR_SIZE;
            input_block->Init(column_types, row_count);

            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
            }

            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
            }

            for (SizeT i = 0; i < row_count; ++i) {
                input_block->AppendValue(2, Value::MakeDouble(static_cast<f64>(i)));
            }

            input_block->Finalize();
            EXPECT_EQ(input_block->Finalized(), true);

            new_txn->Append("db1", "tbl1", input_block);
        }

        {
            // Get column 0 and column 2 from local and global storage;
            Vector<ColumnID> column_ids{0, 1, 2};

            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();
            new_txn->GetMetaTableState(read_table_meta.get(), "db1", "tbl1", column_ids);
            EXPECT_EQ(read_table_meta->local_blocks_.size(), 1);
            for (const auto &local_block_state : read_table_meta->local_blocks_) {
                EXPECT_NE(local_block_state.data_block_, nullptr);
                u16 row_count = local_block_state.data_block_->row_count();
                EXPECT_EQ(row_count, 8192);
                EXPECT_EQ(local_block_state.column_vector_map_.size(), 3);

                ColumnVector *column0 = local_block_state.column_vector_map_.at(0).column_vector_;
                i8 *col0_ptr = (i8 *)(column0->data());
                ColumnVector *column2 = local_block_state.column_vector_map_.at(2).column_vector_;
                f64 *col2_ptr = (f64 *)(column2->data());
                for (SizeT row = 0; row < row_count; ++row) {
                    EXPECT_EQ(col0_ptr[row], (i8)row);
                    EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
                }
            }

            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
            for (const auto &segment_pair : read_table_meta->segment_map_) {
                EXPECT_EQ(segment_pair.first, 0);
                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
                EXPECT_EQ(segment_pair.second.block_map_.size(), 1);
                for (const auto &block_pair : segment_pair.second.block_map_) {
                    //                    EXPECT_EQ(block_pair.first, 0);
                    EXPECT_NE(block_pair.second.block_entry_, nullptr);

                    EXPECT_EQ(block_pair.second.column_data_map_.size(), 3);
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(0));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(1));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(2));

                    BlockColumnEntry *column0 = block_pair.second.column_data_map_.at(0).block_column_;
                    BlockColumnEntry *column1 = block_pair.second.column_data_map_.at(1).block_column_;
                    BlockColumnEntry *column2 = block_pair.second.column_data_map_.at(2).block_column_;

                    SizeT row_count = block_pair.second.block_entry_->row_count_;
                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_mgr);
                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col0_ptr[row], (i8)(row));
                    }

                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_mgr);
                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col1_ptr[row], (i64)(row));
                    }

                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_mgr);
                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
                    }
                }
            }
        }

        {
            // Txn2: Rollback, OK
            txn_mgr->RollBackTxn(new_txn);
        }
    }

    {
        // Txn3: Create, OK
        new_txn = txn_mgr->CreateTxn();

        // Txn3: Begin, OK
        new_txn->Begin();

        {
            // Get column 0 and column 2 from global storage;
            Vector<ColumnID> column_ids{0, 1, 2};

            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();
            new_txn->GetMetaTableState(read_table_meta.get(), "db1", "tbl1", column_ids);
            EXPECT_EQ(read_table_meta->local_blocks_.size(), 0);
            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
            for (const auto &segment_pair : read_table_meta->segment_map_) {
                EXPECT_EQ(segment_pair.first, 0);
                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
                EXPECT_EQ(segment_pair.second.block_map_.size(), 1);
                for (const auto &block_pair : segment_pair.second.block_map_) {
                    //                    EXPECT_EQ(block_pair.first, 0);
                    EXPECT_NE(block_pair.second.block_entry_, nullptr);

                    EXPECT_EQ(block_pair.second.column_data_map_.size(), 3);
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(0));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(1));
                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(2));

                    BlockColumnEntry *column0 = block_pair.second.column_data_map_.at(0).block_column_;
                    BlockColumnEntry *column1 = block_pair.second.column_data_map_.at(1).block_column_;
                    BlockColumnEntry *column2 = block_pair.second.column_data_map_.at(2).block_column_;

                    SizeT row_count = block_pair.second.block_entry_->row_count_;
                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_mgr);
                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col0_ptr[row], (i8)(row));
                    }

                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_mgr);
                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col1_ptr[row], (i64)(row));
                    }

                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_mgr);
                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
                    }
                }
            }
        }

        // Txn3: Commit, OK
        txn_mgr->CommitTxn(new_txn);
    }
}
