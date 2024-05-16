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
import max;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import internal_types;
import logical_type;
import data_type;

class MaxFunctionTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        RemoveDbDirs();
        BaseTest::TearDown();
    }
};

TEST_F(MaxFunctionTest, max_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>(MakeShared<String>(GetDataDir()));

    RegisterMaxFunction(catalog_ptr);

    String op = "max";
    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    SharedPtr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(Boolean)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BooleanT result;
        result = *(BooleanT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(result, true);
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(TinyInt)->TinyInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<TinyIntT>(i)));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        TinyIntT result;
        result = *(TinyIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(result, 127);
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(SmallInt)->SmallInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<SmallIntT>(i)));
        }
        data_block.Finalize();

        for (i64 i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v.value_.small_int, static_cast<SmallIntT>(i));
        }

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        SmallIntT result;
        result = *(SmallIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ((i64)result, row_count - 1);
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(Integer)->Integer", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<IntegerT>(2 * i)));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        IntegerT result;
        result = *(IntegerT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(result, 2 * (row_count - 1));
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(BigInt)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<BigIntT>(2 * i)));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(result, 2 * (row_count - 1));
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(Float)->Float", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<FloatT>(2 * i)));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        FloatT result;
        result = *(FloatT *)func.finalize_func_(data_state.get());

        EXPECT_FLOAT_EQ(result, 2 * (row_count - 1));
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(Double)->Double", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<DoubleT>(2 * i)));
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT *)func.finalize_func_(data_state.get());

        EXPECT_FLOAT_EQ(result, 2 * (row_count - 1));
    }

    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("MAX(HugeInt)->HugeInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            HugeIntT input(0, 2 * i);
            Value v = Value::MakeHugeInt(input);
            data_block.AppendValue(0, v);
        }
        data_block.Finalize();

        auto data_state = func.InitState();
        func.init_func_(data_state.get());
        func.update_func_(data_state.get(), data_block.column_vectors[0]);
        HugeIntT result;
        result = *(HugeIntT *)func.finalize_func_(data_state.get());

        EXPECT_EQ(result.lower, 2 * (row_count - 1));
    }

    {
        DataType data_type(LogicalType::kVarchar);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type, "t1", 1, "c1", 0, 0);

        EXPECT_THROW(aggregate_function_set->GetMostMatchFunction(col_expr_ptr), RecoverableException);
    }
}
