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
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.fisrt_functions;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :stl;
import :infinity_context;
import :first;
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

#if 0
using namespace infinity;
class FirstFunctionTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         FirstFunctionTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));


TEST_F(FirstFunctionTest, first_func) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterFirstFunction(catalog_ptr.get());

    String op = "first";
    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    std::shared_ptr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBoolean);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Boolean)->Boolean", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BooleanT result;
        result = *(BooleanT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, true);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kTinyInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(TinyInt)->TinyInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<TinyIntT>(i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        TinyIntT result;
        result = *(TinyIntT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kSmallInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(SmallInt)->SmallInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<SmallIntT>(i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        SmallIntT result;
        result = *(SmallIntT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kInteger);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Integer)->Integer", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<IntegerT>(i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        IntegerT result;
        result = *(IntegerT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBigInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(BigInt)->BigInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<BigIntT>(i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kHugeInt);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(HugeInt)->HugeInt", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeHugeInt(HugeIntT(i, i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        HugeIntT result;
        result = *(HugeIntT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.lower, 0);
        EXPECT_EQ(result.upper, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kFloat);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Float)->Float", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<FloatT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        FloatT result;
        result = *(FloatT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDouble);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Double)->Double", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<DoubleT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kVarchar);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Varchar)->Varchar", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t idx = 0; idx < row_count; ++idx) {
            String s = "hello" + std::to_string(idx);
            VarcharT varchar_value;
            varchar_value.InitAsValue(s);
            Value v = Value::MakeVarchar(varchar_value);
            data_block.AppendValue(0, v);
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        VarcharT result;
        result = *(VarcharT *)func.finalize_func_(func.GetState());

        EXPECT_STREQ(result.GetDataPtr(), "hello0");
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDate);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Date)->Date", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDate(static_cast<DateT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DateT result;
        result = *(DateT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.value, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kTime);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Time)->Time", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTime(static_cast<TimeT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        TimeT result;
        result = *(TimeT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.value, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kDateTime);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(DateTime)->DateTime", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDateTime(DateTimeT(i, i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DateTimeT result;
        result = *(DateTimeT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.time, 0);
        EXPECT_EQ(result.date, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kTimestamp);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Timestamp)->Timestamp", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTimestamp(TimestampT(i, i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        TimestampT result;
        result = *(TimestampT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.time, 0);
        EXPECT_EQ(result.date, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kInterval);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Interval)->Interval", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            IntervalT interval(i);
            interval.unit = TimeUnit::kDay;
            data_block.AppendValue(0, Value::MakeInterval(interval));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        IntervalT result;
        result = *(IntervalT *)func.finalize_func_(func.GetState());

        EXPECT_EQ(result.unit, TimeUnit::kDay);
        EXPECT_EQ(result.value, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kPoint);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Point)->Point", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakePoint(PointT(static_cast<float>(i), static_cast<float>(i))));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        PointT result;
        result = *(PointT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result.x, 0);
        EXPECT_FLOAT_EQ(result.y, 0);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kLine);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Line)->Line", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeLine(LineT(static_cast<float>(i), static_cast<float>(i + 1), static_cast<float>(i + 2))));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        LineT result = *(LineT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result.a, 0);
        EXPECT_FLOAT_EQ(result.b, 1);
        EXPECT_FLOAT_EQ(result.c, 2);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kLineSeg);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(LineSegment)->LineSegment", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            PointT p1(static_cast<f64>(i), static_cast<f64>(i));
            PointT p2(static_cast<f64>(i + 1), static_cast<f64>(i + 1));
            data_block.AppendValue(0, Value::MakeLineSegment(LineSegT(p1, p2)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        LineSegT result = *(LineSegT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result.point1.x, 0);
        EXPECT_FLOAT_EQ(result.point1.y, 0);
        EXPECT_FLOAT_EQ(result.point2.x, 1);
        EXPECT_FLOAT_EQ(result.point2.y, 1);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBox);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Box)->Box", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            PointT p1(static_cast<f64>(i), static_cast<f64>(i));
            PointT p2(static_cast<f64>(i + 1), static_cast<f64>(i + 1));
            data_block.AppendValue(0, Value::MakeBox(BoxT(p1, p2)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        LineSegT result = *(LineSegT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result.point1.x, 0);
        EXPECT_FLOAT_EQ(result.point1.y, 0);
        EXPECT_FLOAT_EQ(result.point2.x, 1);
        EXPECT_FLOAT_EQ(result.point2.y, 1);
    }

    {
        std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kCircle);
        std::shared_ptr<ColumnExpression> col_expr_ptr = std::make_shared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Circle)->Circle", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            PointT p1(static_cast<f64>(i), static_cast<f64>(i));
            f64 d1 = static_cast<f64>(i);
            data_block.AppendValue(0, Value::MakeCircle(CircleT(p1, d1)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        CircleT result = *(CircleT *)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result.center.x, 0);
        EXPECT_FLOAT_EQ(result.center.y, 0);
        EXPECT_FLOAT_EQ(result.radius, 0);
    }
}
#endif
