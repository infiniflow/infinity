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
#include "gtest/gtest.h"
#include <iomanip>
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"
#include <iomanip>

module infinity_core:ut.extract_function;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;

import :infinity_context;
import :new_catalog;
import :extract;
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
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import logical_type;
import internal_types;
import data_type;

using namespace infinity;
class ExtractFunctionTest : public BaseTest {};

TEST_F(ExtractFunctionTest, extract_year_test) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterExtractFunction(catalog_ptr.get());

    {
        String op = "extract_year";
        std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
        EXPECT_EQ(function_set->type_, FunctionType::kScalar);
        std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

        Vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDate);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("extract_year(Date)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            DateT date_value;
            std::stringstream ss;
            ss << i + 1 << "-1-1";
            date_value.FromString(ss.str());
            data_block.AppendValue(0, Value::MakeDate(date_value));
        }
        data_block.Finalize();

        for (i64 i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDate);
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(4) << i + 1 << "-01-01";
            EXPECT_EQ(v.value_.date.ToString(), ss.str());
        }

        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (i64 i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, i + 1);
        }
    }

    {
        String op = "extract_month";
        std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
        EXPECT_EQ(function_set->type_, FunctionType::kScalar);
        std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

        Vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDate);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("extract_month(Date)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            DateT date_value;
            std::stringstream ss;
            ss << i + 1 << "-" << i % 12 + 1 << "-1";
            date_value.FromString(ss.str());
            data_block.AppendValue(0, Value::MakeDate(date_value));
        }
        data_block.Finalize();

        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (i64 i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, i % 12 + 1);
        }
    }

    {
        String op = "extract_day";
        std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
        EXPECT_EQ(function_set->type_, FunctionType::kScalar);
        std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

        Vector<std::shared_ptr<BaseExpression>> inputs;

        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDate);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("extract_day(Date)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            DateT date_value;
            std::stringstream ss;
            ss << "1-1"
               << "-" << i % 28 + 1;
            date_value.FromString(ss.str());
            data_block.AppendValue(0, Value::MakeDate(date_value));
        }
        data_block.Finalize();

        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (i64 i = 0; i < row_count; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, i % 28 + 1);
        }
    }
}
