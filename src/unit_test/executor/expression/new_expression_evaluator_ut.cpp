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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.new_expression_evaluator;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :new_catalog;
import :scalar_function;
import :add;
import :subtract;
import :function_set;
import :scalar_function_set;
import :function;
import :base_expression;
import :value_expression;
import :reference_expression;
import :function_expression;
import :column_vector;
import :expression_state;
import :value;
import :expression_evaluator;
import :data_table;
import :table_def;
import :data_block;
import :default_values;
import :config;
import :status;
import :kv_store;

import global_resource_usage;
import logical_type;
import internal_types;
import column_def;
import data_type;

using namespace infinity;

class ExpressionEvaluatorTest : public BaseTest {};

TEST_F(ExpressionEvaluatorTest, add_bigint_constant_1) {
    using namespace infinity;
    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());
    RegisterAddFunction(catalog_ptr.get());

    std::string op = "+";
    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    std::vector<std::shared_ptr<BaseExpression>> exprs;
    // Column expression
    std::shared_ptr<ReferenceExpression> col_expr = ReferenceExpression::Make(DataType(LogicalType::kBigInt), "t1", "c1", std::string(), 0);
    // Value expression
    Value v = Value::MakeBigInt(1);
    std::shared_ptr<ValueExpression> value_expr = std::make_shared<ValueExpression>(v);

    exprs.emplace_back(col_expr);
    exprs.emplace_back(value_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("+(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(std::make_shared<DataType>(func.return_type()));
    result.Initialize();

    std::shared_ptr<FunctionExpression> func_expr = std::make_shared<FunctionExpression>(func, exprs);
    std::shared_ptr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "(c1 + 1)");

    ExpressionEvaluator expr_evaluator;

    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
    std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(0, data_type, "c1", std::set<ConstraintType>());
    std::shared_ptr<TableDef> table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("t1"), std::make_shared<std::string>(), {col_def});
    std::shared_ptr<DataTable> input_table = DataTable::Make(table_def, TableType::kDataTable);

    {
        std::shared_ptr<DataBlock> input_data_block = DataBlock::Make();

        input_data_block->Init({data_type});
        //        Vector<std::shared_ptr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(std::make_shared<DataType>(func_expr->Type()));

        output_column_vector->Initialize();
        //        Vector<std::shared_ptr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), 0u);
    }

    {
        std::shared_ptr<DataBlock> input_data_block = DataBlock::Make();
        input_data_block->Init({data_type});

        std::shared_ptr<ColumnVector> column_ptr = std::make_shared<ColumnVector>(data_type);
        size_t row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for (size_t i = 0; i < row_count; ++i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        input_data_block->Init({column_ptr});
        for (size_t i = 0; i < row_count; ++i) {
            Value value = input_data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        //        Vector<std::shared_ptr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(std::make_shared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<std::shared_ptr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);

        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for (size_t row_id = 0; row_id < row_count; ++row_id) {
            Value value = output_column_vector->GetValueByIndex(row_id);
            EXPECT_EQ(value.value_.big_int, (i64)(row_id + 1));
        }
    }
}

TEST_F(ExpressionEvaluatorTest, subtract_constant_8192_bigint) {
    using namespace infinity;
    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());
    RegisterSubtractFunction(catalog_ptr.get());

    std::string op = "-";
    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    // Input expressions
    std::vector<std::shared_ptr<BaseExpression>> exprs;
    // Column expression
    std::shared_ptr<ReferenceExpression> col_expr = std::make_shared<ReferenceExpression>(DataType(LogicalType::kBigInt), "t1", "c1", std::string(), 0);
    // Value expression
    Value v = Value::MakeBigInt(8192);
    std::shared_ptr<ValueExpression> value_expr = std::make_shared<ValueExpression>(v);

    exprs.emplace_back(value_expr);
    exprs.emplace_back(col_expr);

    ScalarFunction func = scalar_function_set->GetMostMatchFunction(exprs);
    EXPECT_STREQ("-(BigInt, BigInt)->BigInt", func.ToString().c_str());

    // Initialize result column vector
    ColumnVector result(std::make_shared<DataType>(func.return_type()));
    result.Initialize();

    std::shared_ptr<FunctionExpression> func_expr = std::make_shared<FunctionExpression>(func, exprs);
    std::shared_ptr<ExpressionState> expr_state = ExpressionState::CreateState(func_expr);

    EXPECT_EQ(func_expr->Type(), DataType(LogicalType::kBigInt));
    EXPECT_STREQ(func_expr->ToString().c_str(), "(8192 - c1)");

    ExpressionEvaluator expr_evaluator;

    std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(DataType(LogicalType::kBigInt)), "c1", std::set<ConstraintType>());
    std::shared_ptr<TableDef> table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("t1"), std::make_shared<std::string>(), {col_def});
    std::shared_ptr<DataTable> input_table = DataTable::Make(table_def, TableType::kDataTable);

    {
        std::shared_ptr<DataBlock> input_data_block = DataBlock::Make();
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
        input_data_block->Init({data_type});

        //        Vector<std::shared_ptr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(std::make_shared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<std::shared_ptr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), 0u);
    }

    {
        std::shared_ptr<DataBlock> input_data_block = DataBlock::Make();
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
        input_data_block->Init({data_type});

        std::shared_ptr<ColumnVector> column_ptr = std::make_shared<ColumnVector>(data_type);
        size_t row_count = DEFAULT_VECTOR_SIZE;
        column_ptr->Initialize(ColumnVectorType::kFlat, row_count);
        for (size_t i = 0; i < row_count; ++i) {
            Value value = Value::MakeBigInt(static_cast<BigIntT>(i));
            column_ptr->AppendValue(value);
        }
        input_data_block->Init({column_ptr});
        //        Vector<std::shared_ptr<DataBlock>> input_blocks;
        //        input_blocks.emplace_back(data_block);

        for (size_t i = 0; i < row_count; ++i) {
            Value value = input_data_block->GetValue(0, i);
            EXPECT_EQ(value.type().type(), LogicalType::kBigInt);
            EXPECT_EQ(value.value_.big_int, i64(i));
        }

        std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(std::make_shared<DataType>(func_expr->Type()));
        output_column_vector->Initialize();

        //        Vector<std::shared_ptr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_column_vector);

        expr_evaluator.Init(input_data_block.get());
        expr_evaluator.Execute(func_expr, expr_state, output_column_vector);
        // blocks_column[0] == output_column_vector
        EXPECT_EQ(output_column_vector->Size(), row_count);

        for (size_t row_id = 0; row_id < row_count; ++row_id) {
            Value value = output_column_vector->GetValueByIndex(row_id);
            EXPECT_EQ((u64)value.value_.big_int, row_count - row_id);
        }
    }
}
