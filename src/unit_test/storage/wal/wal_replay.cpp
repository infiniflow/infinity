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

import stl;
import global_resource_usage;
import storage;
import infinity_context;
import parser;
import txn_manager;
import table_def;
import data_block;
import value;
import segment_entry;
import block_entry;
import txn_store;
import buffer_manager;
import meta_state;
import block_column_entry;
import column_buffer;
import table_collection_entry;
import wal_entry;
import infinity_exception;

import column_vector;
import physical_import;
import txn;

class WalReplayTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override { system("tree  /tmp/infinity"); }
};

using namespace infinity;

namespace {
template <typename T>
void AppendSimpleData(BlockColumnEntry *column_data_entry, const StringView &str_view, SizeT dst_offset) {
    T ele = DataType::StringToValue<T>(str_view);
    BlockColumnEntry::AppendRaw(column_data_entry, dst_offset, reinterpret_cast<ptr_t>(&ele), sizeof(T));
}
} // namespace

TEST_F(WalReplayTest, WalReplayDatabase) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        txn->CreateDatabase("db1", ConflictType::kIgnore);
        txn->CommitTxn();

        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        txn2->CreateDatabase("db2", ConflictType::kIgnore);
        txn2->CommitTxn();

        auto *txn3 = txn_mgr->CreateTxn();
        txn3->BeginTxn();
        txn3->CreateDatabase("db3", ConflictType::kIgnore);
        txn3->CommitTxn();

        auto *txn4 = txn_mgr->CreateTxn();
        txn4->BeginTxn();
        txn4->CreateDatabase("db4", ConflictType::kIgnore);
        txn4->CommitTxn();

        auto *txn5 = txn_mgr->CreateTxn();
        txn5->BeginTxn();
        txn5->Checkpoint(txn3->CommitTS(), true);
        txn5->CommitTxn();

        auto *txn6 = txn_mgr->CreateTxn();
        txn6->BeginTxn();
        txn6->CreateDatabase("db5", ConflictType::kIgnore);
        txn6->CommitTxn();

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }

    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->DropDatabase("db4", ConflictType::kInvalid);
        EXPECT_EQ(result.err_, nullptr);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WalReplayTest, WalReplayTables) {

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
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", Move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }

        auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        auto result2 = txn2->CreateTable("default", Move(tbl2_def), ConflictType::kIgnore);
        EXPECT_NE(result2.entry_, nullptr);
        txn2->CommitTxn();

        {
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->DropTableCollectionByName("default", "tbl2", ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);
            auto *txn3 = txn_mgr->CreateTxn();
            txn3->BeginTxn();
            auto result3 = txn3->CreateTable("default", Move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn3->CommitTxn();
        }
        {
            auto *txn6 = txn_mgr->CreateTxn();
            txn6->BeginTxn();
            txn6->Checkpoint(txn2->CommitTS(), true);
            txn6->CommitTxn();
        }

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", Move(tbl2_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->DropTableCollectionByName("default", "tbl3", ConflictType::kInvalid);
            EXPECT_EQ(result.err_, nullptr);
            txn->CommitTxn();
        }

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WalReplayTest, WalReplayAppend) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

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
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
        }

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", Move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);
            auto *txn3 = txn_mgr->CreateTxn();
            txn3->BeginTxn();
            auto result3 = txn3->CreateTable("default", Move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn3->CommitTxn();
        }
        {
            auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl4"), columns);
            auto *txn4 = txn_mgr->CreateTxn();
            txn4->BeginTxn();
            auto result4 = txn4->CreateTable("default", Move(tbl4_def), ConflictType::kIgnore);
            EXPECT_NE(result4.entry_, nullptr);
            txn4->CommitTxn();
        }
        {
            auto *txn5 = txn_mgr->CreateTxn();
            SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();
            SizeT row_count = 2;

            // Prepare the input data block
            Vector<SharedPtr<DataType>> column_types;
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
            column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

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
            txn5->BeginTxn();
            txn5->Append("default", "tbl4", input_block);
            txn5->CommitTxn();
        }
        {
            auto *txn6 = txn_mgr->CreateTxn();
            txn6->BeginTxn();
            txn6->Checkpoint(3, true);
            txn6->CommitTxn();
        }
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    // Restart the db instance
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        BufferManager *buffer_manager = storage->buffer_manager();

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
        }
        {
            auto tbl5_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl5"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", Move(tbl5_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            Vector<ColumnID> column_ids{0, 1, 2};
            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();

            txn->GetMetaTableState(read_table_meta.get(), "default", "tbl4", column_ids);
            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);

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
                    ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_manager);
                    i8 *col0_ptr = (i8 *)(col0_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col0_ptr[row], (i8)(row));
                    }

                    ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_manager);
                    i64 *col1_ptr = (i64 *)(col1_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_EQ(col1_ptr[row], (i64)(row));
                    }

                    ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_manager);
                    f64 *col2_ptr = (f64 *)(col2_obj.GetAll());
                    for (SizeT row = 0; row < row_count; ++row) {
                        EXPECT_FLOAT_EQ(col2_ptr[row], row % 8192);
                    }
                }
            }
        }

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WalReplayTest, WalReplayImport) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        BufferManager *buffer_manager = storage->buffer_manager();

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
                auto column_def_ptr =
                    MakeShared<ColumnDef>(column_id++, MakeShared<DataType>(DataType(LogicalType::kDouble)), "double_col", constraints);
                columns.emplace_back(column_def_ptr);
            }
        }

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", Move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }

        auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        auto result2 = txn2->CreateTable("default", Move(tbl2_def), ConflictType::kIgnore);
        EXPECT_NE(result2.entry_, nullptr);
        txn2->CommitTxn();

        TxnTimeStamp tx4_commit_ts = txn2->CommitTS();
        {
            auto txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            txn->Checkpoint(tx4_commit_ts, true);
            txn->CommitTxn();
        }

        {
            auto *txn = txn_mgr->CreateTxn();
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);

            txn->BeginTxn();
            auto result3 = txn->CreateTable("default", Move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn->CommitTxn();
        }

        {
            auto txn4 = txn_mgr->CreateTxn();
            txn4->BeginTxn();

            TableCollectionEntry *table_collection_entry = nullptr;
            auto table_collection_entry_result = txn4->GetTableEntry("default", "tbl1", table_collection_entry);
            EXPECT_NE(table_collection_entry, nullptr);
            const String &db_name = *TableCollectionEntry::GetDBEntry(table_collection_entry)->db_name_;
            const String &table_name = *table_collection_entry->table_collection_name_;
            u64 segment_id = TableCollectionEntry::GetNextSegmentID(table_collection_entry);
            EXPECT_EQ(segment_id, 0);
            auto segment_entry = SegmentEntry::MakeNewSegmentEntry(table_collection_entry, segment_id, buffer_manager);
            EXPECT_EQ(segment_entry->segment_id_, 0);
            auto last_block_entry = segment_entry->block_entries_.back().get();

            Vector<SharedPtr<ColumnVector>> columns_vector;
            {
                SharedPtr<ColumnVector> column_vector = ColumnVector::Make(MakeShared<DataType>(LogicalType::kTinyInt));
                column_vector->Initialize();
                Value v = Value::MakeTinyInt(static_cast<TinyIntT>(1));
                column_vector->AppendValue(v);
                columns_vector.push_back(column_vector);
            }
            {
                SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kBigInt));
                column->Initialize();
                Value v = Value::MakeBigInt(static_cast<BigIntT>(22));
                column->AppendValue(v);
                columns_vector.push_back(column);
            }
            {
                SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kDouble));
                column->Initialize();
                Value v = Value::MakeDouble(static_cast<DoubleT>(f64(3)) + 0.33f);
                column->AppendValue(v);
                columns_vector.push_back(column);
            }

            {
                auto block_column_entry1 = last_block_entry->columns_[0].get();
                auto column_type1 = block_column_entry1->column_type_.get();
                EXPECT_EQ(column_type1->type(), LogicalType::kTinyInt);
                SizeT data_type_size = columns_vector[0]->data_type_size_;
                EXPECT_EQ(data_type_size, 1);
                ptr_t src_ptr = columns_vector[0].get()->data();
                SizeT data_size = 1 * data_type_size;
                BlockColumnEntry::AppendRaw(block_column_entry1, 0, src_ptr, data_size);
            }
            {
                auto block_column_entry2 = last_block_entry->columns_[1].get();
                auto column_type2 = block_column_entry2->column_type_.get();
                EXPECT_EQ(column_type2->type(), LogicalType::kBigInt);
                SizeT data_type_size = columns_vector[1]->data_type_size_;
                EXPECT_EQ(data_type_size, 8);
                ptr_t src_ptr = columns_vector[1].get()->data();
                SizeT data_size = 1 * data_type_size;
                BlockColumnEntry::AppendRaw(block_column_entry2, 0, src_ptr, data_size);
            }
            {
                auto block_column_entry3 = last_block_entry->columns_[2].get();
                auto column_type3 = block_column_entry3->column_type_.get();
                EXPECT_EQ(column_type3->type(), LogicalType::kDouble);
                SizeT data_type_size = columns_vector[2]->data_type_size_;
                EXPECT_EQ(data_type_size, 8);
                ptr_t src_ptr = columns_vector[2].get()->data();
                SizeT data_size = 1 * data_type_size;
                BlockColumnEntry::AppendRaw(block_column_entry3, 0, src_ptr, data_size);
            }

            last_block_entry->row_count_ = 1;
            segment_entry->row_count_ = 1;

            auto txn_store = txn4->GetTxnTableStore(table_collection_entry);
            PhysicalImport::SaveSegmentData(txn_store, segment_entry);
            txn4->CommitTxn();
        }

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    // Restart the db instance
    system("tree  /tmp/infinity");
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);

        Storage *storage = infinity::InfinityContext::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();
        BufferManager *buffer_manager = storage->buffer_manager();

        {
            auto txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            Vector<ColumnID> column_ids{0, 1, 2};
            TableCollectionEntry *table_collection_entry = nullptr;
            txn->GetTableEntry("default", "tbl1", table_collection_entry);
            EXPECT_NE(table_collection_entry, nullptr);
            auto segment_entry = table_collection_entry->segments_[0].get();
            EXPECT_EQ(segment_entry->segment_id_, 0);
            auto block_id = segment_entry->block_entries_[0]->block_id_;
            EXPECT_EQ(block_id, 0);
            auto block_entry = segment_entry->block_entries_[0].get();
            EXPECT_EQ(block_entry->row_count_, 1);

            BlockColumnEntry *column0 = block_entry->columns_[0].get();
            BlockColumnEntry *column1 = block_entry->columns_[1].get();
            BlockColumnEntry *column2 = block_entry->columns_[2].get();

            ColumnBuffer col0_obj = BlockColumnEntry::GetColumnData(column0, buffer_manager);
            col0_obj.GetAll();
            DataType *col0_type = column0->column_type_.get();
            i8 *col0_ptr = (i8 *)(col0_obj.GetValueAt(0, *col0_type));
            EXPECT_EQ(*col0_ptr, (1));

            ColumnBuffer col1_obj = BlockColumnEntry::GetColumnData(column1, buffer_manager);
            DataType *col1_type = column1->column_type_.get();
            i8 *col1_ptr = (i8 *)(col1_obj.GetValueAt(0, *col1_type));
            EXPECT_EQ(*col1_ptr, (i64)(22));

            ColumnBuffer col2_obj = BlockColumnEntry::GetColumnData(column2, buffer_manager);
            DataType *col2_type = column2->column_type_.get();
            EXPECT_EQ(col2_type->type(), LogicalType::kDouble);
            f64 *col2_ptr = (f64 *)(col2_obj.GetValueAt(0, *col2_type));
            EXPECT_EQ(col2_ptr[0], (f64)(3) + 0.33f);

            txn->CommitTxn();
        }

        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}
