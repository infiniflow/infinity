//
// Created by jinhai on 22-12-23.
//

#include "unit_test/base_test.h"

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import new_catalog;
import parser;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import cast_table;
import column_vector;
import timestamp_cast;
import bound_cast_func;

class TimestampCastTest : public BaseTest {};

TEST_F(TimestampCastTest, datetime_cast0) {
    using namespace infinity;

    // Try to cast datetime type to wrong type.
    {
        TimestampT source;
        TinyIntT target;
        EXPECT_THROW(TimestampTryCastToFixlen::Run(source, target), FunctionException);
        EXPECT_THROW(TimestampTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        TimestampT source;
        DateT target;
        EXPECT_THROW(TimestampTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TimestampT source;
        TimeT target;
        EXPECT_THROW(TimestampTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TimestampT source;
        DateTimeT target;
        EXPECT_THROW(TimestampTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        TimestampT source;
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(TimestampTryCastToVarlen::Run(source, target, col_varchar_ptr), NotImplementException);
    }
}

TEST_F(TimestampCastTest, datetime_cast1) {
    using namespace infinity;

    // Call BindTimestapCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindTimestampCast(target_type), TypeException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kTimestamp);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeTimestamp(TimestampT(static_cast<i32>(i), static_cast<i32>(i)));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestamp);
        EXPECT_FLOAT_EQ(vx.value_.datetime.date, static_cast<i32>(i));
        EXPECT_FLOAT_EQ(vx.value_.datetime.time, static_cast<i32>(i));
    }

    // cast datetime column vector to date column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDate);
        auto source2target_ptr = BindTimestampCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
    // cast datetime column vector to time column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kTime);
        auto source2target_ptr = BindTimestampCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
    // cast datetime column vector to timestamp column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kDateTime);
        auto source2target_ptr = BindTimestampCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }

    // cast datetime column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindTimestampCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}
