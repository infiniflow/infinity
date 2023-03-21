//
// Created by jinhai on 23-1-15.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "executor/expression/expression_executor.h"

#include "storage/catalog.h"
#include "function/scalar/subtract.h"
#include "function/scalar/add.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class ExpressionExecutorTest : public BaseTest {
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

TEST_F(ExpressionExecutorTest, add_bigint_constant_1) {
    using namespace infinity;
    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();
    RegisterAddFunction(catalog_ptr);

    String op = "+";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    Vector<SharedPtr<BaseExpression>> exprs;
    // Column expression
    SharedPtr<ReferenceExpression> col_expr =
            ReferenceExpression::Make(DataType(LogicalType::kBigInt),
                                      "t1",
                                      "c1",
                                      0);
    // Value expression
    Value v = Value::MakeBigInt(1);
    SharedPtr<ValueExpression> value_expr = MakeShared<ValueExpression>(v);

    exprs.emplace_back(col_expr);
    exprs.emplace_back(value_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("+(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(func.return_type());
    result.Initialize();

    SharedPtr<FunctionExpression> func_expr = MakeShared<FunctionExpression>(func, exprs);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "+(t1.c1, BigInt(1))");

    ExpressionExecutor expr_executor;
    expr_executor.Init({func_expr});

    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0,
                                                         DataType(LogicalType::kBigInt),
                                                         "c1",
                                                         HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make("t1", {col_def});
    SharedPtr<Table> input_table = Table::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), 0);
    }

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for(i64 i = 0; i < row_count; ++ i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        data_block->Init({column_ptr});
        for(SizeT i = 0; i < row_count; ++ i) {
            Value value = data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        input_table->Append(data_block);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), row_count);
        EXPECT_EQ(output_table->DataBlockCount(), 1);
        for(SizeT block_id = 0; block_id < output_table->DataBlockCount(); ++ block_id) {
            SharedPtr<DataBlock> data_block_ptr = output_table->GetDataBlockById(block_id);
            EXPECT_EQ(data_block_ptr->column_count(), 1);
            for(SizeT column_id = 0; column_id < data_block_ptr->column_count(); ++ column_id) {
                SharedPtr<ColumnVector> output_column_vector_ptr = data_block_ptr->column_vectors[column_id];
                for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
                    Value value = output_column_vector_ptr->GetValue(row_id);
                    EXPECT_EQ(value.value_.big_int, row_id + 1);
                }
            }
        }
    }

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        SizeT row_start_idx = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for(i64 i = row_start_idx; i < row_count + row_start_idx; ++ i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        data_block->Init({column_ptr});
        for(SizeT i = 0; i < row_count; ++ i) {
            Value value = data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i + row_start_idx));
        }

        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        input_table->Append(data_block);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), 2 * row_count);
        EXPECT_EQ(output_table->DataBlockCount(), 2);
        for(SizeT block_id = 0; block_id < output_table->DataBlockCount(); ++ block_id) {
            SharedPtr<DataBlock> data_block_ptr = output_table->GetDataBlockById(block_id);
            EXPECT_EQ(data_block_ptr->column_count(), 1);
            for(SizeT column_id = 0; column_id < data_block_ptr->column_count(); ++ column_id) {
                SharedPtr<ColumnVector> output_column_vector_ptr = data_block_ptr->column_vectors[column_id];
                for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
                    Value value = output_column_vector_ptr->GetValue(row_id);
//                    LOG_TRACE("{} {} {}", value.value_.big_int, row_id + 1, column_id * row_count);
                    EXPECT_EQ(value.value_.big_int, row_id + 1 + block_id * row_count);
                }
            }
        }
    }
}

TEST_F(ExpressionExecutorTest, subtract_constant_8192_bigint) {
    using namespace infinity;
    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();
    RegisterSubtractFunction(catalog_ptr);

    String op = "-";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    Vector<SharedPtr<BaseExpression>> exprs;
    // Column expression
    SharedPtr<ReferenceExpression> col_expr =
            ReferenceExpression::Make(DataType(LogicalType::kBigInt),
                                      "t1",
                                      "c1",
                                      0);
    // Value expression
    Value v = Value::MakeBigInt(8192);
    SharedPtr<ValueExpression> value_expr = MakeShared<ValueExpression>(v);

    exprs.emplace_back(value_expr);
    exprs.emplace_back(col_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("-(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(func.return_type());
    result.Initialize();

    SharedPtr<FunctionExpression> func_expr = MakeShared<FunctionExpression>(func, exprs);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "-(BigInt(8192), t1.c1)");

    ExpressionExecutor expr_executor;
    expr_executor.Init({func_expr});

    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0,
                                                         DataType(LogicalType::kBigInt),
                                                         "c1",
                                                         HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make("t1", {col_def});
    SharedPtr<Table> input_table = Table::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), 0);
    }

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for(i64 i = 0; i < row_count; ++ i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        data_block->Init({column_ptr});
        for(SizeT i = 0; i < row_count; ++ i) {
            Value value = data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        input_table->Append(data_block);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), row_count);
        EXPECT_EQ(output_table->DataBlockCount(), 1);
        for(SizeT block_id = 0; block_id < output_table->DataBlockCount(); ++ block_id) {
            SharedPtr<DataBlock> data_block_ptr = output_table->GetDataBlockById(block_id);
            EXPECT_EQ(data_block_ptr->column_count(), 1);
            for(SizeT column_id = 0; column_id < data_block_ptr->column_count(); ++ column_id) {
                SharedPtr<ColumnVector> output_column_vector_ptr = data_block_ptr->column_vectors[column_id];
                for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
                    Value value = output_column_vector_ptr->GetValue(row_id);
                    EXPECT_EQ(value.value_.big_int, row_count - row_id);
                }
            }
        }
    }

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        SizeT row_start_idx = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for(i64 i = row_start_idx; i < row_count + row_start_idx; ++ i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        data_block->Init({column_ptr});
        for(SizeT i = 0; i < row_count; ++ i) {
            Value value = data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i + row_start_idx));
        }

        SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);
        input_table->Append(data_block);
        expr_executor.Execute(input_table, output_table);
        EXPECT_EQ(output_table->row_count(), 2 * row_count);
        EXPECT_EQ(output_table->DataBlockCount(), 2);
        for(SizeT block_id = 0; block_id < output_table->DataBlockCount(); ++ block_id) {
            SharedPtr<DataBlock> data_block_ptr = output_table->GetDataBlockById(block_id);
            EXPECT_EQ(data_block_ptr->column_count(), 1);
            for(SizeT column_id = 0; column_id < data_block_ptr->column_count(); ++ column_id) {
                SharedPtr<ColumnVector> output_column_vector_ptr = data_block_ptr->column_vectors[column_id];
                for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
                    Value value = output_column_vector_ptr->GetValue(row_id);
                    EXPECT_EQ(value.value_.big_int, row_count - row_id - block_id * row_count);
                }
            }
        }
    }
}
