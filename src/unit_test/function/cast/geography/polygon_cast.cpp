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
import parser;
import logger;
import stl;
import infinity_context;
import catalog;
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
import geography_cast;
import bound_cast_func;
#if 0
class PolygonCastTest : public BaseTest {};

TEST_F(PolygonCastTest, polygon_cast0) {
    using namespace infinity;

    // Try to cast circle type to wrong type.
    {
        PointT p1(static_cast<f64>(1) + 0.1f, static_cast<f64>(1) - 0.3f);
        PointT p2(static_cast<f64>(1) + 0.5f, static_cast<f64>(1) - 0.7f);
        PointT p3(static_cast<f64>(1) + 0.2f, static_cast<f64>(1) - 0.4f);
        PointT p4(static_cast<f64>(1) + 0.6f, static_cast<f64>(1) - 0.8f);
        PolygonT source;
        source.Initialize(4);
        source.SetPoint(0, p1);
        source.SetPoint(1, p2);
        source.SetPoint(2, p3);
        source.SetPoint(3, p4);
        TinyIntT target;
        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        PointT p1(static_cast<f64>(1) + 0.1f, static_cast<f64>(1) - 0.3f);
        PointT p2(static_cast<f64>(1) + 0.5f, static_cast<f64>(1) - 0.7f);
        PointT p3(static_cast<f64>(1) + 0.2f, static_cast<f64>(1) - 0.4f);
        PointT p4(static_cast<f64>(1) + 0.6f, static_cast<f64>(1) - 0.8f);
        PolygonT source;
        source.Initialize(4);
        source.SetPoint(0, p1);
        source.SetPoint(1, p2);
        source.SetPoint(2, p3);
        source.SetPoint(3, p4);

        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, col_varchar_ptr), NotImplementException);
    }
}

TEST_F(PolygonCastTest, polygon_cast1) {
    using namespace infinity;

    // Call BindGeographyCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kPolygon);
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindGeographyCast<PolygonT>(source_type, target_type), TypeException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kPolygon);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PolygonT polygon;
        polygon.Initialize(4);
        polygon.SetPoint(0, p1);
        polygon.SetPoint(1, p2);
        polygon.SetPoint(2, p3);
        polygon.SetPoint(3, p4);
        Value v = Value::MakePolygon(polygon);
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));

        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPolygon);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);
    }
    // cast circle column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindGeographyCast<PolygonT>(*source_type, *target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}
#endif