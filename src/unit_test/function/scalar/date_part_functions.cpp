// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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


#include "gtest/gtest.h"

import stl;
import base_test;
import infinity_exception;
import infinity_context;

import catalog;
import logger;

import default_values;
import value;

import base_expression;
import column_expression;
import column_vector;
import data_block;

import function_set;
import function;

import global_resource_usage;

import data_type;
import internal_types;
import logical_type;

import scalar_function;
import scalar_function_set;

import date_part;
import third_party;

using namespace infinity;

class DatePartFunctionsTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, DatePartFunctionsTest, ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));

TEST_P(DatePartFunctionsTest, datepart_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterDatePartFunction(catalog_ptr);

    String op = "datepart";

    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kVarchar);
        DataType data_type2(LogicalType::kDate);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type1, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("datepart(Varchar, Date)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(data_type1));
        column_types.emplace_back(MakeShared<DataType>(data_type2));

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            std::string str = "year";
            data_block.AppendValue(0, Value::MakeVarchar(str));
            data_block.AppendValue(1, Value::MakeDate(static_cast<DateT>(2 * i)));
        }
        data_block.Finalize();
        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kVarchar);
            EXPECT_EQ(v2.type_.type(), LogicalType::kDate);
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kVarchar);
        DataType data_type2(LogicalType::kDateTime);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type1, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("datepart(Varchar, DateTime)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(data_type1));
        column_types.emplace_back(MakeShared<DataType>(data_type2));

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            std::string str = "year";
            data_block.AppendValue(0, Value::MakeVarchar(str));
            data_block.AppendValue(1, Value::MakeDateTime(DateTimeT(i, i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kVarchar);
            EXPECT_EQ(v2.type_.type(), LogicalType::kDateTime);
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kVarchar);
        DataType data_type2(LogicalType::kTimestamp);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type1, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("datepart(Varchar, Timestamp)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(data_type1));
        column_types.emplace_back(MakeShared<DataType>(data_type2));

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            std::string str = "year";
            data_block.AppendValue(0, Value::MakeVarchar(str));
            data_block.AppendValue(1, Value::MakeTimestamp(TimestampT(i, i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kVarchar);
            EXPECT_EQ(v2.type_.type(), LogicalType::kTimestamp);
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
        }
    }
}