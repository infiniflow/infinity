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

import first;
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
#if 0
class FirstFunctionTest : public BaseTest {};

TEST_F(FirstFunctionTest, first_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>(MakeShared<String>("/var/infinity/data"));

    RegisterFirstFunction(catalog_ptr);

    String op = "first";
    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    SharedPtr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Boolean)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(TinyInt)->TinyInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(SmallInt)->SmallInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Integer)->Integer", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(BigInt)->BigInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(HugeInt)->HugeInt", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Float)->Float", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Double)->Double", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Varchar)->Varchar", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT idx = 0; idx < row_count; ++idx) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDate);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Date)->Date", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTime);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Time)->Time", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDateTime);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(DateTime)->DateTime", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kTimestamp);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Timestamp)->Timestamp", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kInterval);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Interval)->Interval", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPoint);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Point)->Point", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLine);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Line)->Line", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLineSeg);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(LineSegment)->LineSegment", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBox);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Box)->Box", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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
        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kCircle);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("FIRST(Circle)->Circle", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
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