
#include "infinity_test.h"
#include "main/infinity.h"
#include "storage/buffer/column_buffer.h"
#include "storage/data_block.h"
#include "storage/meta/entry/block_column_entry.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/meta/meta_state.h"
#include "storage/txn/txn_manager.h"

class WaReplayTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }

    void TearDown() override { system("rm -rf /tmp/infinity"); }
};

using namespace infinity;

TEST_F(WaReplayTest, WalReplayDatabase) {
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

TEST_F(WaReplayTest, WalReplayTables) {

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

TEST_F(WaReplayTest, WalReplayAppend) {
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
