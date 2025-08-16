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

#ifdef CI
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.mul_functions;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;

import :infinity_context;
import :new_catalog;
import :scalar_function;
import :scalar_function_set;
import :function_set;
import :function;
import :column_expression;
import :value;
import :default_values;
import :data_block;
import :base_expression;
import :column_vector;
import :multiply;
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import logical_type;
import internal_types;
import data_type;

using namespace infinity;
class MulFunctionsTest : public BaseTest {};

TEST_F(MulFunctionsTest, mul_func) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterMulFunction(catalog_ptr.get());

    std::string op = "*";
    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kTinyInt);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kTinyInt);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(TinyInt, TinyInt)->TinyInt", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
            data_block.AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v1.value_.tiny_int, static_cast<i8>(i));
            EXPECT_EQ(v2.value_.tiny_int, static_cast<i8>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            i16 res = static_cast<i16>(static_cast<i8>(i)) * static_cast<i16>(static_cast<i8>(i));
            if (res < std::numeric_limits<i8>::min() || res > std::numeric_limits<i8>::max()) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(res));
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kSmallInt);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kSmallInt);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(SmallInt, SmallInt)->SmallInt", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(i)));
            data_block.AppendValue(1, Value::MakeSmallInt(static_cast<i16>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v1.value_.small_int, static_cast<i16>(i));
            EXPECT_EQ(v2.value_.small_int, static_cast<i16>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            i32 res = static_cast<i32>(static_cast<i16>(i)) * static_cast<i32>(static_cast<i16>(i));
            if (res < std::numeric_limits<i16>::min() || res > std::numeric_limits<i16>::max()) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_EQ(v.value_.small_int, static_cast<i16>(res));
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kInteger);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kInteger);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Integer, Integer)->Integer", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(i)));
            data_block.AppendValue(1, Value::MakeInt(static_cast<i32>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v2.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v1.value_.integer, static_cast<i32>(i));
            EXPECT_EQ(v2.value_.integer, static_cast<i32>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
            i64 res = static_cast<i64>(static_cast<i32>(i)) * static_cast<i64>(static_cast<i32>(i));
            if (res < std::numeric_limits<i32>::min() || res > std::numeric_limits<i32>::max()) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_EQ(v.value_.integer, static_cast<i32>(res));
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(BigInt, BigInt)->BigInt", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(i)));
            data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v1.value_.big_int, static_cast<i64>(i));
            EXPECT_EQ(v2.value_.big_int, static_cast<i64>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            i64 res;
            if (__builtin_mul_overflow(static_cast<i64>(i), static_cast<i64>(i), &res)) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_EQ(v.value_.big_int, static_cast<i64>(res));
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kFloat);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kFloat);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Float, Float)->Float", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i)));
            data_block.AppendValue(1, Value::MakeFloat(static_cast<f32>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kFloat);
            EXPECT_EQ(v2.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i));
            EXPECT_FLOAT_EQ(v2.value_.float32, static_cast<f32>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kFloat);
            f32 res = static_cast<f32>(i) * static_cast<f32>(i);
            if (std::isinf(res) || std::isnan(res)) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_FLOAT_EQ(v.value_.float32, res);
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDouble);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kDouble);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Double, Double)->Double", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i)));
            data_block.AppendValue(1, Value::MakeDouble(static_cast<f64>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kDouble);
            EXPECT_EQ(v2.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i));
            EXPECT_FLOAT_EQ(v2.value_.float64, static_cast<f64>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDouble);
            f64 res = static_cast<f64>(i) * static_cast<f64>(i);
            if (std::isinf(res) || std::isnan(res)) {
                EXPECT_FALSE(result->nulls_ptr_->IsTrue(i));
            } else {
                EXPECT_FLOAT_EQ(v.value_.float64, res);
            }
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDecimal);
        DataType result_type(LogicalType::kDecimal);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Decimal, Decimal)->Decimal", func.ToString().c_str());
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data1_type(LogicalType::kMixed);
        DataType data2_type(LogicalType::kBigInt);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data1_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data2_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Heterogeneous, BigInt)->Heterogeneous", func.ToString().c_str());
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data1_type(LogicalType::kBigInt);
        DataType data2_type(LogicalType::kMixed);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data1_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data2_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(BigInt, Heterogeneous)->Heterogeneous", func.ToString().c_str());
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data1_type(LogicalType::kMixed);
        DataType data2_type(LogicalType::kDouble);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data1_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data2_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Heterogeneous, Double)->Heterogeneous", func.ToString().c_str());
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data1_type(LogicalType::kDouble);
        DataType data2_type(LogicalType::kMixed);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data1_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data2_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Double, Heterogeneous)->Heterogeneous", func.ToString().c_str());
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data1_type(LogicalType::kMixed);
        DataType data2_type(LogicalType::kMixed);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data1_type, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data2_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("*(Heterogeneous, Heterogeneous)->Heterogeneous", func.ToString().c_str());
    }
}
