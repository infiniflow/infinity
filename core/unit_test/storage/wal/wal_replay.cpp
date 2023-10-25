
#include "unit_test/base_test.h"

import stl;
import global_resource_usage;
import storage;
import infinity;
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
import infinity_assert;

class WalReplayTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override {
        system("tree  /tmp/infinity");
        system("rm -rf /tmp/infinity");
    }
};

using namespace infinity;

namespace {
template <typename T>
void AppendSimpleData(BlockColumnEntry *column_data_entry, const StringView &str_view, SizeT dst_offset) {
    T ele = DataType::StringToValue<T>(str_view);
    BlockColumnEntry::AppendRaw(column_data_entry, dst_offset, reinterpret_cast<ptr_t>(&ele), sizeof(T));
}
}

TEST_F(WalReplayTest, WalReplayDatabase) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
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

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }

    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->DropDatabase("db4", ConflictType::kInvalid);
        EXPECT_EQ(result.err_, nullptr);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        infinity::Infinity::instance().UnInit();
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
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }

        auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        auto result2 = txn2->CreateTable("default", std::move(tbl2_def), ConflictType::kIgnore);
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
            auto result3 = txn3->CreateTable("default", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn3->CommitTxn();
        }
        {
            auto *txn6 = txn_mgr->CreateTxn();
            txn6->BeginTxn();
            txn6->Checkpoint(txn2->CommitTS(), true);
            txn6->CommitTxn();
        }

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            auto result = txn->CreateTable("default", std::move(tbl2_def), ConflictType::kIgnore);
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

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WalReplayTest, WalReplayAppend) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
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
            auto result = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);
            auto *txn3 = txn_mgr->CreateTxn();
            txn3->BeginTxn();
            auto result3 = txn3->CreateTable("default", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn3->CommitTxn();
        }
        {
            auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl4"), columns);
            auto *txn4 = txn_mgr->CreateTxn();
            txn4->BeginTxn();
            auto result4 = txn4->CreateTable("default", std::move(tbl4_def), ConflictType::kIgnore);
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
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    // Restart the db instance
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
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
            auto result = txn->CreateTable("default", std::move(tbl5_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            Vector<ColumnID> column_ids{0, 1, 2};
            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();

            txn->GetMetaTableState(read_table_meta.get(), "default", "tbl4", column_ids);

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

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WalReplayTest, WalReplayImport) {
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
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
            auto result = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
            auto *txn2 = txn_mgr->CreateTxn();
            txn2->BeginTxn();
            auto result3 = txn2->CreateTable("default", std::move(tbl2_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn2->CommitTxn();
        }

        {
            auto *txn3 = txn_mgr->CreateTxn();
            auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);

            txn3->BeginTxn();
            auto result3 = txn3->CreateTable("default", std::move(tbl3_def), ConflictType::kIgnore);
            EXPECT_NE(result3.entry_, nullptr);
            txn3->CommitTxn();
        }

        auto *txn4 = txn_mgr->CreateTxn();

        {
            txn4->BeginTxn();

            TableCollectionEntry *table_collection_entry = nullptr;
            txn4->GetTableEntry("default", "tbl1", table_collection_entry);
            EXPECT_NE(table_collection_entry, nullptr);
            txn4->AddTxnTableStore("tbl1", MakeUnique<TxnTableStore>("tbl1", table_collection_entry, txn4));

            TxnTableStore *txn_store = txn4->GetTxnTableStore("tbl1");
            EXPECT_NE(txn_store, nullptr);

            u64 segment_id = TableCollectionEntry::GetNextSegmentID(table_collection_entry);
            auto segment_entry = SegmentEntry::MakeNewSegmentEntry(table_collection_entry, segment_id, buffer_manager);

            // process segment entry

            BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();

            SizeT need_write_row = 1;
            Vector<StringView> write_data{"1", "22", "3.33"};
            for (SizeT column_idx = 0; column_idx < /*column_count*/ 3; ++column_idx) {
                BlockColumnEntry *block_column_entry = last_block_entry->columns_[column_idx].get();
                auto column_type = block_column_entry->column_type_.get();
                SizeT dst_offset = need_write_row * column_type->Size();
                // test data
                // 1 22 3.33
                switch (column_type->type()) {
                    case kBoolean: {
                        AppendSimpleData<BooleanT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kTinyInt: {
                        AppendSimpleData<TinyIntT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kSmallInt: {
                        AppendSimpleData<SmallIntT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kInteger: {
                        AppendSimpleData<IntegerT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kBigInt: {
                        AppendSimpleData<BigIntT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kFloat: {
                        AppendSimpleData<FloatT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kDouble: {
                        AppendSimpleData<DoubleT>(block_column_entry, write_data[column_idx], dst_offset);
                        break;
                    }
                    case kMissing:
                    case kInvalid: {
                        Error<ExecutorException>("Invalid data type", __FILE_NAME__, __LINE__);
                    }
                    default: {
                        Error<NotImplementException>("Not supported now in append data in column", __FILE_NAME__, __LINE__);
                    }
                }
            }

            ++last_block_entry->row_count_;
            ++segment_entry->row_count_;

            txn4->AddWalCmd(MakeShared<WalCmdImport>("default",
                                                     "tbl1",
                                                     *segment_entry->segment_dir_,
                                                     segment_entry->segment_id_,
                                                     segment_entry->block_entries_.size()));
            txn_store->Import(segment_entry);
            txn4->CommitTxn();
        }

        i64 tx4_commit_ts = txn4->CommitTS();

        {
            auto *txn6 = txn_mgr->CreateTxn();
            txn6->BeginTxn();
            txn6->Checkpoint(tx4_commit_ts, true);
            txn6->CommitTxn();
        }

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
    // Restart the db instance
    system("tree  /tmp/infinity");
    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        Storage *storage = infinity::Infinity::instance().storage();
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
            auto result = txn->CreateTable("default", std::move(tbl5_def), ConflictType::kIgnore);
            EXPECT_NE(result.entry_, nullptr);
            txn->CommitTxn();
        }
        {
            auto *txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            Vector<ColumnID> column_ids{0, 1, 2};
            UniquePtr<MetaTableState> read_table_meta = MakeUnique<MetaTableState>();

            txn->GetMetaTableState(read_table_meta.get(), "default", "tbl1", column_ids);

            EXPECT_EQ(read_table_meta->local_blocks_.size(), 0);
            EXPECT_EQ(read_table_meta->segment_map_.size(), 1);
            for (const auto &segment_pair : read_table_meta->segment_map_) {
                EXPECT_EQ(segment_pair.first, 0);
                EXPECT_NE(segment_pair.second.segment_entry_, nullptr);
                for (const auto &block_pair : segment_pair.second.block_map_) {
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

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}
