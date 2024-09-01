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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import catalog;
import inequals;
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
import logical_type;
import internal_types;
import data_type;

using namespace infinity;
class InEqualsFunctionsTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, InEqualsFunctionsTest, ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));

TEST_P(InEqualsFunctionsTest, inequals_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterInEqualsFunction(catalog_ptr);

    String op = "<>";
    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(Boolean, Boolean)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeBool(true));
                data_block.AppendValue(1, Value::MakeBool(false));
            } else {
                data_block.AppendValue(0, Value::MakeBool(false));
                data_block.AppendValue(1, Value::MakeBool(true));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.boolean, true);
                EXPECT_EQ(v2.value_.boolean, false);
            } else {
                EXPECT_EQ(v1.value_.boolean, false);
                EXPECT_EQ(v2.value_.boolean, true);
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();

        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v.value_.boolean, true);
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTinyInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(TinyInt, TinyInt)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
                data_block.AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
                data_block.AppendValue(1, Value::MakeTinyInt(static_cast<i8>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kTinyInt);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.tiny_int, static_cast<i8>(i));
                EXPECT_EQ(v2.value_.tiny_int, static_cast<i8>(i));
            } else {
                EXPECT_EQ(v1.value_.tiny_int, static_cast<i8>(i));
                EXPECT_EQ(v2.value_.tiny_int, static_cast<i8>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kSmallInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(SmallInt, SmallInt)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(i)));
                data_block.AppendValue(1, Value::MakeSmallInt(static_cast<i16>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(i)));
                data_block.AppendValue(1, Value::MakeSmallInt(static_cast<i16>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kSmallInt);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.small_int, static_cast<i16>(i));
                EXPECT_EQ(v2.value_.small_int, static_cast<i16>(i));
            } else {
                EXPECT_EQ(v1.value_.small_int, static_cast<i16>(i));
                EXPECT_EQ(v2.value_.small_int, static_cast<i16>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(Integer, Integer)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(i)));
                data_block.AppendValue(1, Value::MakeInt(static_cast<i32>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(i)));
                data_block.AppendValue(1, Value::MakeInt(static_cast<i32>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v2.type_.type(), LogicalType::kInteger);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.integer, static_cast<i32>(i));
                EXPECT_EQ(v2.value_.integer, static_cast<i32>(i));
            } else {
                EXPECT_EQ(v1.value_.integer, static_cast<i32>(i));
                EXPECT_EQ(v2.value_.integer, static_cast<i32>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(BigInt, BigInt)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(i)));
                data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(i)));
                data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBigInt);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.big_int, static_cast<i64>(i));
                EXPECT_EQ(v2.value_.big_int, static_cast<i64>(i));
            } else {
                EXPECT_EQ(v1.value_.big_int, static_cast<i64>(i));
                EXPECT_EQ(v2.value_.big_int, static_cast<i64>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kHugeInt);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(HugeInt, HugeInt)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeHugeInt(HugeIntT(static_cast<i64>(i), static_cast<i64>(i))));
                data_block.AppendValue(1, Value::MakeHugeInt(HugeIntT(static_cast<i64>(i), static_cast<i64>(i))));
            } else {
                data_block.AppendValue(0, Value::MakeHugeInt(HugeIntT(static_cast<i64>(i), static_cast<i64>(i))));
                data_block.AppendValue(1, Value::MakeHugeInt(HugeIntT(static_cast<i64>(-i), static_cast<i64>(i))));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kHugeInt);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.huge_int, HugeIntT(static_cast<i64>(i), static_cast<i64>(i)));
                EXPECT_EQ(v2.value_.huge_int, HugeIntT(static_cast<i64>(i), static_cast<i64>(i)));
            } else {
                EXPECT_EQ(v1.value_.huge_int, HugeIntT(static_cast<i64>(i), static_cast<i64>(i)));
                EXPECT_EQ(v2.value_.huge_int, HugeIntT(static_cast<i64>(-i), static_cast<i64>(i)));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(Float, Float)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i)));
                data_block.AppendValue(1, Value::MakeFloat(static_cast<f32>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i)));
                data_block.AppendValue(1, Value::MakeFloat(static_cast<f32>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kFloat);
            EXPECT_EQ(v2.type_.type(), LogicalType::kFloat);
            if (i % 2 == 0) {
                EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i));
                EXPECT_FLOAT_EQ(v2.value_.float32, static_cast<f32>(i));
            } else {
                EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i));
                EXPECT_FLOAT_EQ(v2.value_.float32, static_cast<f32>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(Double, Double)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i)));
                data_block.AppendValue(1, Value::MakeDouble(static_cast<f64>(i)));
            } else {
                data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i)));
                data_block.AppendValue(1, Value::MakeDouble(static_cast<f64>(-i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kDouble);
            EXPECT_EQ(v2.type_.type(), LogicalType::kDouble);
            if (i % 2 == 0) {
                EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i));
                EXPECT_FLOAT_EQ(v2.value_.float64, static_cast<f64>(i));
            } else {
                EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i));
                EXPECT_FLOAT_EQ(v2.value_.float64, static_cast<f64>(-i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }
#if 0
    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type1 = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<DataType> data_type2 = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type1, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("<>(Varchar, Varchar)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type1);
        column_types.emplace_back(data_type2);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeVarchar("Helloworld" + std::to_string(i)));
                data_block.AppendValue(1, Value::MakeVarchar("Helloworld" + std::to_string(i)));
            } else {
                data_block.AppendValue(0, Value::MakeVarchar("Helloworld" + std::to_string(i)));
                data_block.AppendValue(1, Value::MakeVarchar("helloworld" + std::to_string(i)));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kVarchar);
            EXPECT_EQ(v2.type_.type(), LogicalType::kVarchar);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.varchar.ToString(), "Helloworld" + std::to_string(i));
                EXPECT_EQ(v2.value_.varchar.ToString(), "Helloworld" + std::to_string(i));
            } else {
                EXPECT_EQ(v1.value_.varchar.ToString(), "Helloworld" + std::to_string(i));
                EXPECT_EQ(v2.value_.varchar.ToString(), "helloworld" + std::to_string(i));
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v.value_.boolean, false);
            } else {
                EXPECT_EQ(v.value_.boolean, true);
            }
        }
    }
#endif
}
