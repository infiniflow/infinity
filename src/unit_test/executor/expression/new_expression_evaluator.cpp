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

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import new_catalog;
import scalar_function;
import add;
import substract;
import function_set;
import scalar_function_set;
import function;
import base_expression;
import value_expression;
import reference_expression;
import function_expression;
import column_vector;
import expression_state;
import value;
import expression_evaluator;
import data_table;
import table_def;
import data_block;
import default_values;

class ExpressionEvaluatorTest : public BaseTest {};

TEST_F(ExpressionEvaluatorTest, add_bigint_constant_1) {
    using namespace infinity;
    UniquePtr<NewCatalog> catalog_ptr = MakeUnique<NewCatalog>(nullptr);
    RegisterAddFunction(catalog_ptr);

    String op = "+";
    SharedPtr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    Vector<SharedPtr<BaseExpression>> exprs;
    // Column expression
    SharedPtr<ReferenceExpression> col_expr = ReferenceExpression::Make(DataType(LogicalType::kBigInt), "t1", "c1", String(), 0);
    // Value expression
    Value v = Value::MakeBigInt(1);
    SharedPtr<ValueExpression> value_expr = MakeShared<ValueExpression>(v);

    exprs.emplace_back(col_expr);
    exprs.emplace_back(value_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("+(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(MakeShared<DataType>(func.return_type()));
    result.Initialize();

    SharedPtr<FunctionExpression> func_expr = MakeShared<FunctionExpression>(func, exprs);
    SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "(c1 + 1)");

    ExpressionEvaluator expr_evaluator;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0, data_type, "c1", HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("t1"), {col_def});
    SharedPtr<DataTable> input_table = DataTable::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<DataBlock> input_data_block = DataBlock::Make();

        input_data_block->Init({data_type});
        //        Vector<SharedPtr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(MakeShared<DataType>(func_expr->Type()));

        output_column_vector->Initialize();
        //        Vector<SharedPtr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), 0);
    }

    {
        SharedPtr<DataBlock> input_data_block = DataBlock::Make();
        input_data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for (i64 i = 0; i < row_count; ++i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        input_data_block->Init({column_ptr});
        for (SizeT i = 0; i < row_count; ++i) {
            Value value = input_data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        //        Vector<SharedPtr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(MakeShared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<SharedPtr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);

        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            Value value = output_column_vector->GetValue(row_id);
            EXPECT_EQ(value.value_.big_int, row_id + 1);
        }
    }
}

TEST_F(ExpressionEvaluatorTest, subtract_constant_8192_bigint) {
    using namespace infinity;
    UniquePtr<NewCatalog> catalog_ptr = MakeUnique<NewCatalog>(nullptr);
    RegisterSubtractFunction(catalog_ptr);

    String op = "-";
    SharedPtr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    Vector<SharedPtr<BaseExpression>> exprs;
    // Column expression
    SharedPtr<ReferenceExpression> col_expr = MakeShared<ReferenceExpression>(DataType(LogicalType::kBigInt), "t1", "c1", String(), 0);
    // Value expression
    Value v = Value::MakeBigInt(8192);
    SharedPtr<ValueExpression> value_expr = MakeShared<ValueExpression>(v);

    exprs.emplace_back(value_expr);
    exprs.emplace_back(col_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("-(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(MakeShared<DataType>(func.return_type()));
    result.Initialize();

    SharedPtr<FunctionExpression> func_expr = MakeShared<FunctionExpression>(func, exprs);
    SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "(8192 - c1)");

    ExpressionEvaluator expr_evaluator;

    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0, MakeShared<DataType>(DataType(LogicalType::kBigInt)), "c1", HashSet<ConstraintType>());
    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("t1"), {col_def});
    SharedPtr<DataTable> input_table = DataTable::Make(table_def, TableType::kDataTable);

    {
        SharedPtr<DataBlock> input_data_block = DataBlock::Make();
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        input_data_block->Init({data_type});

        //        Vector<SharedPtr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(MakeShared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<SharedPtr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), 0);
    }

    {
        SharedPtr<DataBlock> input_data_block = DataBlock::Make();
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        input_data_block->Init({data_type});

        SharedPtr<ColumnVector> column_ptr = MakeShared<ColumnVector>(data_type);
        SizeT row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for (i64 i = 0; i < row_count; ++i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        input_data_block->Init({column_ptr});
        //        Vector<SharedPtr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        for (SizeT i = 0; i < row_count; ++i) {
            Value value = input_data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(MakeShared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<SharedPtr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            Value value = output_column_vector->GetValue(row_id);
            EXPECT_EQ(value.value_.big_int, row_count - row_id);
        }
    }
}
