//
// Created by jinhai on 23-2-9.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "executor/expression/expression_evaluator.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/scalar/subtract.h"
#include "function/scalar/add.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class ExpressionEvaluatorTest : public BaseTest {
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

TEST_F(ExpressionEvaluatorTest, add_bigint_constant_1) {
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
    SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "+(t1.c1, BigInt(1))");

    ExpressionEvaluator expr_evaluator;

    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0,
                                                         DataType(LogicalType::kBigInt),
                                                         "c1",
                                                         HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make("t1", {col_def});
    SharedPtr<Table> input_table = Table::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(func_expr->Type());

        output_column_vector->Initialize();
        expr_evaluator.Execute(func_expr,
                               expr_state,
                               data_block,
                               0,
                               output_column_vector);
        EXPECT_EQ(output_column_vector->Size(), 0);
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

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(func_expr->Type());
        output_column_vector->Initialize();
        expr_evaluator.Execute(func_expr, expr_state, data_block, row_count, output_column_vector);
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            Value value = output_column_vector->GetValue(row_id);
            EXPECT_EQ(value.value_.big_int, row_id + 1);
        }
    }
}

TEST_F(ExpressionEvaluatorTest, subtract_constant_8192_bigint) {
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
    SharedPtr<ReferenceExpression> col_expr = MakeShared<ReferenceExpression>(DataType(LogicalType::kBigInt),
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
    SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "-(BigInt(8192), t1.c1)");

    ExpressionEvaluator expr_evaluator;

    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0,
                                                         DataType(LogicalType::kBigInt),
                                                         "c1",
                                                         HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make("t1", {col_def});
    SharedPtr<Table> input_table = Table::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        DataType data_type(LogicalType::kBigInt);
        data_block->Init({data_type});

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(func_expr->Type());
        output_column_vector->Initialize();
        expr_evaluator.Execute(func_expr, expr_state, data_block, 0, output_column_vector);
        EXPECT_EQ(output_column_vector->Size(), 0);
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

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(func_expr->Type());
        output_column_vector->Initialize();
        expr_evaluator.Execute(func_expr, expr_state, data_block, row_count, output_column_vector);
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            Value value = output_column_vector->GetValue(row_id);
            EXPECT_EQ(value.value_.big_int, row_count - row_id);
        }
    }
}
