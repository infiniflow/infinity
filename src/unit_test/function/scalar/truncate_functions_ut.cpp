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

#ifdef CI
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.truncate_functions;

import :ut.base_test;

import :infinity_exception;
import :infinity_context;
import :new_catalog;
import :logger;
import :default_values;
import :value;
import :base_expression;
import :column_expression;
import :column_vector;
import :data_block;
import :function_set;
import :function;
import :scalar_function;
import :scalar_function_set;
import :trunc;
import third_party;
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import data_type;
import internal_types;
import logical_type;

using namespace infinity;

class TruncateFunctionsTest : public BaseTest {};

TEST_F(TruncateFunctionsTest, truncate_func) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterTruncFunction(catalog_ptr.get());

    std::string op = "trunc";

    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kFloat);
        DataType data_type2(LogicalType::kBigInt);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kVarchar);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data_type1, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("trunc(Float, BigInt)->Varchar", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(std::make_shared<DataType>(data_type1));
        column_types.emplace_back(std::make_shared<DataType>(data_type2));

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i)));
            data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kFloat);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBigInt);
            EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i));
            EXPECT_EQ(v2.value_.big_int, static_cast<i64>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kVarchar);
        }
    }

    {
        std::vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kDouble);
        DataType data_type2(LogicalType::kBigInt);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kVarchar);
        std::shared_ptr<ColumnExpression> col1_expr_ptr = std::make_shared<ColumnExpression>(data_type1, "t1", 1, "c1", 0, 0);
        std::shared_ptr<ColumnExpression> col2_expr_ptr = std::make_shared<ColumnExpression>(data_type2, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("trunc(Double, BigInt)->Varchar", func.ToString().c_str());

        std::vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(std::make_shared<DataType>(data_type1));
        column_types.emplace_back(std::make_shared<DataType>(data_type2));

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i)));
            data_block.AppendValue(1, Value::MakeBigInt(static_cast<i64>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kDouble);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBigInt);
            EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i));
            EXPECT_EQ(v2.value_.big_int, static_cast<i64>(i));
        }

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kVarchar);
        }
    }
}
