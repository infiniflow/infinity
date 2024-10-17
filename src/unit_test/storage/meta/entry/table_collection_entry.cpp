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

#include "gtest/gtest.h"
import base_test;

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import value;

import data_block;
import default_values;
import txn_manager;
import txn;

import buffer_manager;
import column_vector;
import table_entry_type;
import meta_state;
import status;
import internal_types;
import logical_type;
import extra_ddl_info;
import column_def;
import data_type;

import table_entry;

using namespace infinity;
class TableEntryTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TableEntryTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(TableEntryTest, test1) {
    using namespace infinity;

    SharedPtr<String> table_dir = MakeShared<String>(GetTmpDir());
    SharedPtr<TableDef> table_def{};

    // Define columns
    {
        Vector<SharedPtr<ColumnDef>> columns;
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }

        table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("t1"), MakeShared<String>(), columns);

        EXPECT_EQ(*table_def->table_name(), "t1");
        EXPECT_EQ(table_def->column_count(), 2ul);
        EXPECT_EQ(table_def->GetColIdByName("tiny_int_col"), 0ul);
        EXPECT_EQ(table_def->GetColIdByName("big_int_col"), 1ul);
    }

    SharedPtr<TableEntry> table_entry = MakeShared<TableEntry>(false,
                                                               table_dir,
                                                               table_def->table_name(),
                                                               table_def->columns(),
                                                               TableEntryType::kTableEntry,
                                                               nullptr,
                                                               0 /*txn_id*/,
                                                               0 /*begin_ts*/,
                                                               INVALID_SEGMENT_ID /*unsealed_id*/,
                                                               0 /*next_segment_id*/,
                                                               0 /*next_column_id*/);
}

TEST_P(TableEntryTest, test2) {
    using namespace infinity;

    TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();

    // Txn1: Create, OK
    Txn *new_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db1"));

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase(MakeShared<String>("db1"), ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());

    // Txn1: Create tbl1, OK
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kTinyInt)), "tiny_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr =
                MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "big_int_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            std::set<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    UniquePtr<TableDef> tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
    Status s1 = new_txn->CreateTable("db1", std::move(tbl1_def), ConflictType::kError);
    EXPECT_TRUE(s1.ok());

    // Txn1: Commit, OK
    txn_mgr->CommitTxn(new_txn);

    {
        // Txn2: Create, OK
        new_txn = txn_mgr->BeginTxn(MakeUnique<String>("insert data"));

        // Txn2: Get db1, OK
        auto [table_entry, s2] = new_txn->GetTableByName("db1", "tbl1");
        EXPECT_TRUE(s2.ok());
        EXPECT_NE(table_entry, nullptr);

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

        Status append_status = new_txn->Append(table_entry, input_block);
        EXPECT_TRUE(append_status.ok());
        // Txn2: Commit, OK
        txn_mgr->CommitTxn(new_txn);
    }

    {
        // Txn2: Create, OK
        new_txn = txn_mgr->BeginTxn(MakeUnique<String>("insert data"));

        //        {
        //            // Get column 0 and column 2 from global storage;
        //            Vector<ColumnID> column_ids{0, 1, 2};
        //
        //            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();
        //            new_txn->GetMetaTableState(read_table_meta.get(), "db1", "tbl1", column_ids);
        //            EXPECT_EQ(read_table_meta->local_blocks_.size(), 0);
        //            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
        //            for (const auto &segment_pair : read_table_meta->segment_map_) {
        //                EXPECT_EQ(segment_pair.first, 0);
        //                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
        //                EXPECT_EQ(segment_pair.second.segment_entry_->block_entries_.size(), 1);
        //                EXPECT_EQ(segment_pair.second.block_map_.size(), 1);
        //                for (const auto &block_pair : segment_pair.second.block_map_) {
        //                    //                    EXPECT_EQ(block_pair.first, 0);
        //                    EXPECT_NE(block_pair.second.block_entry_, nullptr);
        //
        //                    EXPECT_EQ(block_pair.second.column_data_map_.size(), 3);
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(0));
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(1));
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(2));
        //
        //                    BlockColumnEntry *column0 = block_pair.second.column_data_map_.at(0).block_column_;
        //                    BlockColumnEntry *column1 = block_pair.second.column_data_map_.at(1).block_column_;
        //                    BlockColumnEntry *column2 = block_pair.second.column_data_map_.at(2).block_column_;
        //
        //                    SizeT row_count = block_pair.second.block_entry_->row_count_;
        //                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_mgr);
        //                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_EQ(col0_ptr[row], (i8)(row));
        //                    }
        //
        //                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_mgr);
        //                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_EQ(col1_ptr[row], (i64)(row));
        //                    }
        //
        //                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_mgr);
        //                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
        //                    }
        //                }
        //            }
        //        }

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

            auto [table_entry, status] = new_txn->GetTableByName("db1", "tbl1");
            ASSERT_TRUE(status.ok());
            new_txn->Append(table_entry, input_block);
        }

        //        {
        //            // Get column 0 and column 2 from local and global storage;
        //            Vector<ColumnID> column_ids{0, 1, 2};
        //
        //            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();
        //            new_txn->GetMetaTableState(read_table_meta.get(), "db1", "tbl1", column_ids);
        //            EXPECT_EQ(read_table_meta->local_blocks_.size(), 1);
        //            for (const auto &local_block_state : read_table_meta->local_blocks_) {
        //                EXPECT_NE(local_block_state.data_block_, nullptr);
        //                u16 row_count = local_block_state.data_block_->row_count();
        //                EXPECT_EQ(row_count, 8192);
        //                EXPECT_EQ(local_block_state.column_vector_map_.size(), 3);
        //
        //                ColumnVector *column0 = local_block_state.column_vector_map_.at(0).column_vector_;
        //                i8 *col0_ptr = (i8 *)(column0->data());
        //                ColumnVector *column2 = local_block_state.column_vector_map_.at(2).column_vector_;
        //                f64 *col2_ptr = (f64 *)(column2->data());
        //                for (SizeT row = 0; row < row_count; ++row) {
        //                    EXPECT_EQ(col0_ptr[row], (i8)row);
        //                    EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
        //                }
        //            }
        //
        //            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
        //            for (const auto &segment_pair : read_table_meta->segment_map_) {
        //                EXPECT_EQ(segment_pair.first, 0);
        //                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
        //                EXPECT_EQ(segment_pair.second.block_map_.size(), 1);
        //                for (const auto &block_pair : segment_pair.second.block_map_) {
        //                    //                    EXPECT_EQ(block_pair.first, 0);
        //                    EXPECT_NE(block_pair.second.block_entry_, nullptr);
        //
        //                    EXPECT_EQ(block_pair.second.column_data_map_.size(), 3);
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(0));
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(1));
        //                    EXPECT_TRUE(block_pair.second.column_data_map_.contains(2));
        //
        //                    BlockColumnEntry *column0 = block_pair.second.column_data_map_.at(0).block_column_;
        //                    BlockColumnEntry *column1 = block_pair.second.column_data_map_.at(1).block_column_;
        //                    BlockColumnEntry *column2 = block_pair.second.column_data_map_.at(2).block_column_;
        //
        //                    SizeT row_count = block_pair.second.block_entry_->row_count_;
        //                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_mgr);
        //                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_EQ(col0_ptr[row], (i8)(row));
        //                    }
        //
        //                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_mgr);
        //                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_EQ(col1_ptr[row], (i64)(row));
        //                    }
        //
        //                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_mgr);
        //                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
        //                    for (SizeT row = 0; row < row_count; ++row) {
        //                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
        //                    }
        //                }
        //            }
        //        }

        {
            // Txn2: Rollback, OK
            txn_mgr->RollBackTxn(new_txn);
        }
    }
}
