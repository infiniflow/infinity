//
// Created by jinhai on 23-7-30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/table_def.h"
#include "storage/table/data_table.h"
#include "storage/txn/txn_manager.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class DataTableTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};


TEST_F(DataTableTest, test1) {
    using namespace infinity;

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    String temp_path = "/tmp/infinity/_tmp";
    String base_path = "/tmp/infinity/data";
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

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
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++,
                                                        DataType(LogicalType::kTinyInt),
                                                        "tiny_int_col",
                                                        constraints);
            columns.emplace_back(column_def_ptr);

        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++,
                                                        DataType(LogicalType::kBigInt),
                                                        "big_int_col",
                                                        constraints);
            columns.emplace_back(column_def_ptr);
        }

        table_def = TableDef::Make("t1", columns);

        EXPECT_EQ(table_def->table_name(), "t1");
        EXPECT_EQ(table_def->column_count(), 2);
        EXPECT_EQ(table_def->GetColIdByName("tiny_int_col"), 0);
        EXPECT_EQ(table_def->GetColIdByName("big_int_col"), 1);
        LOG_TRACE("\n{}", table_def->ToString());
    }

    SharedPtr<DataTable> data_table = DataTable::Make(table_dir, table_def, &buffer_mgr);
    EXPECT_EQ(data_table->table_name(), "t1");
    EXPECT_EQ(data_table->schema_name(), "default");
}

TEST_F(DataTableTest, test2) {
    using namespace infinity;

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    String temp_path = "/tmp/infinity/_tmp";
    String base_path = "/tmp/infinity/data";
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity/table");
    NewCatalog new_catalog(std::move(dir), &buffer_mgr, nullptr);
    TxnManager txn_mgr(&new_catalog);

    EntryResult create1_res, table1_res, get_res;

    // Txn1: Create, OK
    Txn* new_txn = txn_mgr.CreateTxn();

    // Txn1: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn1: Create db1, OK
    create1_res = new_txn->CreateDatabase("db1");
    EXPECT_EQ(create1_res.entry_->Committed(), false);
    EXPECT_NE(create1_res.entry_, nullptr);

    // Txn1: Create tbl1, OK
    // Define columns
    Vector<SharedPtr<ColumnDef>> columns;
    {
        i64 column_id = 0;
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kUnique);
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++,
                                                        DataType(LogicalType::kTinyInt),
                                                        "tiny_int_col",
                                                        constraints);
            columns.emplace_back(column_def_ptr);

        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kPrimaryKey);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++,
                                                        DataType(LogicalType::kBigInt),
                                                        "big_int_col",
                                                        constraints);
            columns.emplace_back(column_def_ptr);
        }
        {
            HashSet<ConstraintType> constraints;
            constraints.insert(ConstraintType::kNotNull);
            auto column_def_ptr = MakeShared<ColumnDef>(column_id++,
                                                        DataType(LogicalType::kDouble),
                                                        "double_col",
                                                        constraints);
            columns.emplace_back(column_def_ptr);
        }
    }

    UniquePtr<TableDef> tbl1_def = MakeUnique<TableDef>("tbl1", columns);
    table1_res = new_txn->CreateTable("db1", std::move(tbl1_def));
    EXPECT_NE(table1_res.entry_, nullptr);

    // Txn1: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Create, OK
    new_txn = txn_mgr.CreateTxn();

    // Txn2: Begin, OK
    new_txn->BeginTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Txn2: Get db1, OK
    get_res = new_txn->GetTableByName("db1", "tbl1");
    EXPECT_NE(get_res.entry_, nullptr);

    // Prepare the input data block
    Vector<DataType> column_types;
    column_types.emplace_back(LogicalType::kTinyInt);
    column_types.emplace_back(LogicalType::kBigInt);
    column_types.emplace_back(LogicalType::kDouble);

    SharedPtr<DataBlock> input_block = MakeShared<DataBlock>();

    SizeT row_count = DEFAULT_VECTOR_SIZE * 2;
    input_block->Init(column_types, row_count);

    for(SizeT i = 0; i < row_count; ++ i) {
        input_block->AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
    }

    for(SizeT i = 0; i < row_count; ++ i) {
        input_block->AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
    }

    for(SizeT i = 0; i < row_count; ++ i) {
        input_block->AppendValue(2, Value::MakeDouble(static_cast<f64>(i)));
    }

    input_block->Finalize();
    EXPECT_EQ(input_block->Finalized(), true);

    new_txn->Append("db1", "tbl1", input_block);

    // Txn2: Commit, OK
    new_txn->CommitTxn(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}