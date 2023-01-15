//
// Created by jinhai on 23-1-15.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "executor/expression/expression_executor.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/scalar/add.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class ExpressionExecutorTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ExpressionExecutorTest, expression_test0) {
    using namespace infinity;
    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();
    RegisterAddFunction(catalog_ptr);
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName("+");
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    Vector<SharedPtr<BaseExpression>> exprs;
    // Column expression
    SharedPtr<ColumnExpression> col_expr = MakeShared<ColumnExpression>(DataType(LogicalType::kBigInt), "t1", "c1", 0, 0);
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

    SharedPtr<ColumnDef> col_def = ColumnDef::Make("c1", 0, DataType(LogicalType::kBigInt), Set<ConstrainType>());
    SharedPtr<TableDef> table_def = TableDef::Make("t1", {col_def}, false);
    SharedPtr<Table> input_table = Table::Make(table_def, TableType::kDataTable);
    SharedPtr<Table> output_table = Table::Make(table_def, TableType::kResult);

    expr_executor.Execute(input_table, output_table);
    EXPECT_EQ(output_table->row_count(), 0);

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(row_count, ColumnVectorType::kFlat);
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
}
