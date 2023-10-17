
#include "infinity_test.h"
#include "main/infinity.h"
#include "storage/data_block.h"
#include "storage/txn/txn_manager.h"
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

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
        txn5->DropDatabase("db4", ConflictType::kIgnore);
        txn5->CommitTxn();

        auto *txn6 = txn_mgr->CreateTxn();
        txn6->BeginTxn();
        txn6->Checkpoint(3, true);
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
        txn->CreateDatabase("db4", ConflictType::kInvalid);
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

        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        auto tbl2_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl2"), columns);
        auto *txn2 = txn_mgr->CreateTxn();
        txn2->BeginTxn();
        auto result2 = txn2->CreateTable("default", std::move(tbl2_def), ConflictType::kIgnore);
        EXPECT_NE(result2.entry_, nullptr);
        txn2->CommitTxn();

        auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);
        auto *txn3 = txn_mgr->CreateTxn();
        txn3->BeginTxn();
        auto result3 = txn3->CreateTable("default", std::move(tbl3_def), ConflictType::kIgnore);
        EXPECT_NE(result3.entry_, nullptr);
        txn3->CommitTxn();

        auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl4"), columns);
        auto *txn4 = txn_mgr->CreateTxn();
        txn4->BeginTxn();
        auto result4 = txn4->CreateTable("default", std::move(tbl4_def), ConflictType::kIgnore);
        EXPECT_NE(result4.entry_, nullptr);
        txn4->CommitTxn();

        auto *txn5 = txn_mgr->CreateTxn();
        txn5->BeginTxn();
        auto result5 = txn5->DropTableCollectionByName("default", "tbl4", ConflictType::kIgnore);
        EXPECT_NE(result5.entry_, nullptr);
        txn5->CommitTxn();

        auto *txn6 = txn_mgr->CreateTxn();
        txn6->BeginTxn();
        txn6->Checkpoint(3, true);
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

        auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl4"), columns);
        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->CreateTable("default", std::move(tbl4_def), ConflictType::kIgnore);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}

TEST_F(WaReplayTest, WalReplayAppend) {
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

    Vector<SharedPtr<DataType>> column_types;
    {
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));
    }

    {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);

        SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();
        SizeT row_count = 2;
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

        Storage *storage = infinity::Infinity::instance().storage();
        TxnManager *txn_mgr = storage->txn_manager();

        auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl1"), columns);
        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->CreateTable("default", std::move(tbl1_def), ConflictType::kIgnore);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        auto tbl3_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl3"), columns);
        auto *txn3 = txn_mgr->CreateTxn();
        txn3->BeginTxn();
        auto result3 = txn3->CreateTable("default", std::move(tbl3_def), ConflictType::kIgnore);
        EXPECT_NE(result3.entry_, nullptr);
        txn3->CommitTxn();

        auto tbl4_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl4"), columns);
        auto *txn4 = txn_mgr->CreateTxn();
        txn4->BeginTxn();
        auto result4 = txn4->CreateTable("default", std::move(tbl4_def), ConflictType::kIgnore);
        EXPECT_NE(result4.entry_, nullptr);
        txn4->CommitTxn();

        auto *txn5 = txn_mgr->CreateTxn();
        txn5->BeginTxn();
        txn5->Append("default", "tbl4", input_block);
        txn5->CommitTxn();

        auto *txn6 = txn_mgr->CreateTxn();
        txn6->BeginTxn();
        txn6->Checkpoint(3, true);
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

        auto tbl5_def = MakeUnique<TableDef>(MakeShared<String>("default"), MakeShared<String>("tbl5"), columns);
        auto *txn = txn_mgr->CreateTxn();
        txn->BeginTxn();
        auto result = txn->CreateTable("default", std::move(tbl5_def), ConflictType::kIgnore);
        EXPECT_NE(result.entry_, nullptr);
        txn->CommitTxn();

        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
}
