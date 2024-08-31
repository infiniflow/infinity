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
import geography_cast;
import bound_cast_func;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;

class CircleCastTest : public BaseTest {
    void SetUp() override {
    }

    void TearDown() override {
        BaseTest::TearDown();
    }
};

TEST_F(CircleCastTest, circle_cast0) {
    using namespace infinity;

    // Try to cast circle type to wrong type.
    {
        PointT p1(1, 1);
        f64 r(2);
        CircleT source(p1, r);
        TinyIntT target;
        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        PointT p1(1, 1);
        f64 r(2);
        CircleT source(p1, r);
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
}

TEST_F(CircleCastTest, circle_cast1) {
    using namespace infinity;

    // Call BindGeographyCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kCircle);
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindGeographyCast<CircleT>(source_type, target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kCircle);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i), static_cast<f64>(i));
        f64 r = static_cast<f64>(i);
        Value v = Value::MakeCircle(CircleT(p1, r));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i));
    }
    // cast circle column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindGeographyCast<CircleT>(*source_type, *target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }
}
