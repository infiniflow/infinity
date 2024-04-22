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

import logger;
import stl;
import infinity_context;
import catalog;
import scalar_function;
import scalar_function_set;
import function_set;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import base_expression;
import column_vector;
import modulo;
import logical_type;
import internal_types;
import data_type;

class ModuloFunctionsTest : public BaseTest {};

TEST_F(ModuloFunctionsTest, mod_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>(MakeShared<String>(GetDataDir()));

    RegisterModuloFunction(catalog_ptr);

    String op = "%";
    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTinyInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(TinyInt, TinyInt)->TinyInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i + 2)));
            data_block.AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v1.value_.tiny_int, static_cast<i8>(i + 2));
            EXPECT_EQ(v2.value_.tiny_int, static_cast<i8>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {

            if (i % 256 == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
                EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(i + 2) % static_cast<i8>(i));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kSmallInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(SmallInt, SmallInt)->SmallInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(i + 10)));
            data_block.AppendValue(1, Value::MakeSmallInt(static_cast<i16>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v1.value_.small_int, static_cast<i16>(i + 10));
            EXPECT_EQ(v2.value_.small_int, static_cast<i16>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
                EXPECT_EQ(v.value_.small_int, static_cast<i16>(i + 10) % static_cast<i16>(i));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(Integer, Integer)->Integer", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(i + 9)));
            data_block.AppendValue(1, Value::MakeInt(static_cast<i32>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v2.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v1.value_.integer, static_cast<i32>(i + 9));
            EXPECT_EQ(v2.value_.integer, static_cast<i32>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
                EXPECT_EQ(v.value_.integer, static_cast<i32>(i + 9) % static_cast<i32>(i));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(BigInt, BigInt)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(i + 5)));
            data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v1.value_.big_int, static_cast<i64>(i + 5));
            EXPECT_EQ(v2.value_.big_int, static_cast<i64>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
                EXPECT_EQ(v.value_.big_int, static_cast<i64>(i + 5) % static_cast<i64>(i));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kHugeInt);
        DataType result_type(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(data_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(HugeInt, HugeInt)->HugeInt", func.ToString().c_str());

        // TODO: need to complete it.
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(Float, Float)->Float", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i + 2)));
            data_block.AppendValue(1, Value::MakeFloat(static_cast<f32>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kFloat);
            EXPECT_EQ(v2.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i + 2));
            EXPECT_FLOAT_EQ(v2.value_.float32, static_cast<f32>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kFloat);
                EXPECT_FLOAT_EQ(v.value_.float32, std::fmod(static_cast<f32>(i + 2), static_cast<f32>(i)));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("%(Double, Double)->Double", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i + 3)));
            data_block.AppendValue(1, Value::MakeDouble(static_cast<f64>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kDouble);
            EXPECT_EQ(v2.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i + 3));
            EXPECT_FLOAT_EQ(v2.value_.float64, static_cast<f64>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i == 0) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                Value v = result->GetValue(i);
                EXPECT_TRUE(result->nulls_ptr_->IsTrue(i));
                EXPECT_EQ(v.type_.type(), LogicalType::kDouble);
                EXPECT_EQ(v.value_.float64, std::fmod(static_cast<f64>(i + 3), static_cast<f64>(i)));
            }
        }
    }
}
