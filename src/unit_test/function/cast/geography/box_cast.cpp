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

class BoxCastTest : public BaseTest {
    void SetUp() override {
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
        BaseTest::TearDown();
    }
};

TEST_F(BoxCastTest, line_seg_cast0) {
    using namespace infinity;

    // Try to cast line seg type to wrong type.
    {
        PointT p1(1, 1);
        PointT p2(2, 2);
        BoxT source(p1, p2);
        TinyIntT target;
        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        PointT p1(1, 1);
        PointT p2(2, 2);
        LineSegT source(p1, p2);
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, col_varchar_ptr.get()), UnrecoverableException);
    }
}

TEST_F(BoxCastTest, line_seg_cast1) {
    using namespace infinity;

    // Call BindGeographyCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kBox);
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindGeographyCast<BoxT>(source_type, target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kBox);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i), static_cast<f64>(i));
        PointT p2(static_cast<f64>(i + 1), static_cast<f64>(i + 1));
        Value v = Value::MakeBox(BoxT(p1, p2));
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i + 1));
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i + 1));
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i));
    }
    // cast line seg column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindGeographyCast<BoxT>(*source_type, *target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), UnrecoverableException);
    }
}
