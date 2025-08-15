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
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.sum_functions;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :stl;
import :infinity_context;
import :new_catalog;
import :sum;
import :function_set;
import :aggregate_function_set;
import :aggregate_function;
import :function;
import :column_expression;
import :value;
import :default_values;
import :data_block;
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;
class SumFunctionTest : public BaseTest {};

TEST_F(SumFunctionTest, sum_func) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterSumFunction(catalog_ptr.get());

    String op = "sum";
    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    std::shared_ptr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kTinyInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(TinyInt)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        i64 sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<TinyIntT>(i)));
            sum += static_cast<TinyIntT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(sum, result);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kSmallInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(SmallInt)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        i64 sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<SmallIntT>(i)));
            sum += static_cast<SmallIntT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(sum, result);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kInteger);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(Integer)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        i64 sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<IntegerT>(i)));
            sum += static_cast<IntegerT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(sum, result);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(BigInt)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        i64 sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<BigIntT>(i)));
            sum += static_cast<BigIntT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(sum, result);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kFloat);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(Float)->Double", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<FloatT>(i)));
            sum += static_cast<FloatT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT *)func.finalize_func_(data_state.get());

        EXPECT_FLOAT_EQ(sum, result);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDouble);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("SUM(Double)->Double", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<DoubleT>(i)));
            sum += static_cast<DoubleT>(i);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT *)func.finalize_func_(data_state.get());

        EXPECT_FLOAT_EQ(sum, result);
    }

    {
        DataType data_type(LogicalType::kBoolean);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(data_type, "t1", 1, "c1", 0, 0);

        EXPECT_THROW_WITHOUT_STACKTRACE(aggregate_function_set->GetMostMatchFunction(col_expr_ptr), RecoverableException);
    }
}
