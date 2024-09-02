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

import logger;
import column_vector;
import value;

import default_values;
import third_party;
import stl;
import selection;
import vector_buffer;
import infinity_context;
import global_resource_usage;
import internal_types;
import logical_type;
import data_type;
import compilation_config;

#if 0
class ColumnVectorGeoTest : public BaseTest {
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         ColumnVectorGeoTest,
                         ::testing::Values((std::string(infinity::test_data_path()) + "/config/test_close_bgtask_silent.toml").c_str(),
                                           (std::string(infinity::test_data_path()) + "/config/test_close_bgtask_silent_vfs_off.toml").c_str()));

TEST_P(ColumnVectorGeoTest, flat_point) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPoint);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), u64(DEFAULT_VECTOR_SIZE));
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        column_vector.AppendByPtr((ptr_t)(&point));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_point) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPoint);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, point_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPoint);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(2 * i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(2 * i) - 0.8f);
    }
}

TEST_P(ColumnVectorGeoTest, point_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPoint);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(src_idx) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(src_idx) - 0.8f);
    }
}

TEST_P(ColumnVectorGeoTest, flat_line) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLine);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        column_vector.AppendByPtr((ptr_t)(&line));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_line) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLine);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, line_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLine);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(2 * i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(2 * i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(2 * i) - 5.3f);
    }
}

TEST_P(ColumnVectorGeoTest, line_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLine);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        LineT line(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f, static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(src_idx) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(src_idx) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(src_idx) - 5.3f);
    }
}

TEST_P(ColumnVectorGeoTest, flat_line_seg) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLineSeg);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        column_vector.AppendByPtr((ptr_t)(&line_seg));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_line_seg) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLineSeg);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, line_seg_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLineSeg);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(2 * i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(2 * i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(2 * i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(2 * i) + 7.9f);
    }
}

TEST_P(ColumnVectorGeoTest, line_seg_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kLineSeg);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(src_idx) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(src_idx) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(src_idx) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(src_idx) + 7.9f);
    }
}

TEST_P(ColumnVectorGeoTest, flat_box) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBox);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        column_vector.AppendByPtr((ptr_t)(&box));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_box) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBox);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 32u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, box_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBox);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(2 * i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(2 * i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(2 * i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(2 * i) + 7.9f);
    }
}

TEST_P(ColumnVectorGeoTest, box_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBox);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(src_idx) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(src_idx) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(src_idx) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(src_idx) + 7.9f);
    }
}
#if 0
TEST_P(ColumnVectorGeoTest, flat_path) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPath);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }

    EXPECT_EQ(column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        column_vector.AppendByPtr((ptr_t)(&path));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_path) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPath);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    auto tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    tmp_ptr = column_vector.data();
    EXPECT_EQ(tmp_ptr, column_vector.data());
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, path_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPath);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        PointT p1(static_cast<f64>(2 * i) + 0.1f, static_cast<f64>(2 * i) - 0.3f);
        PointT p2(static_cast<f64>(2 * i) + 0.5f, static_cast<f64>(2 * i) - 0.7f);
        PointT p3(static_cast<f64>(2 * i) + 0.2f, static_cast<f64>(2 * i) - 0.4f);
        PointT p4(static_cast<f64>(2 * i) + 0.6f, static_cast<f64>(2 * i) - 0.8f);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }
}

TEST_P(ColumnVectorGeoTest, path_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPath);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        PointT p1(static_cast<f64>(src_idx) + 0.1f, static_cast<f64>(src_idx) - 0.3f);
        PointT p2(static_cast<f64>(src_idx) + 0.5f, static_cast<f64>(src_idx) - 0.7f);
        PointT p3(static_cast<f64>(src_idx) + 0.2f, static_cast<f64>(src_idx) - 0.4f);
        PointT p4(static_cast<f64>(src_idx) + 0.6f, static_cast<f64>(src_idx) - 0.8f);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.path.ptr) + 3), p4);
    }
}

TEST_P(ColumnVectorGeoTest, flat_polygon) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPolygon);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 48);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

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
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

    EXPECT_EQ(column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
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
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 48);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

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
        column_vector.AppendByPtr((ptr_t)(&polygon));
        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT *)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_polygon) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPolygon);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 48);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    auto tmp_ptr = column_vector.data();
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data());

    for (i64 i = 0; i < 1; ++i) {
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
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 48);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    tmp_ptr = column_vector.data();
    EXPECT_EQ(tmp_ptr, column_vector.data());
    for (i64 i = 0; i < 1; ++i) {
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
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, polygon_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPolygon);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

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
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        PointT p1(static_cast<f64>(2 * i) + 0.1f, static_cast<f64>(2 * i) - 0.3f);
        PointT p2(static_cast<f64>(2 * i) + 0.5f, static_cast<f64>(2 * i) - 0.7f);
        PointT p3(static_cast<f64>(2 * i) + 0.2f, static_cast<f64>(2 * i) - 0.4f);
        PointT p4(static_cast<f64>(2 * i) + 0.6f, static_cast<f64>(2 * i) - 0.8f);

        Value vx = target_column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(2 * i) + 0.1f, static_cast<f64>(2 * i) - 0.3f),
                          PointT(static_cast<f64>(2 * i) + 0.6f, static_cast<f64>(2 * i) - 0.8f));
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
}

TEST_P(ColumnVectorGeoTest, polygon_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kPolygon);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

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
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = column_vector.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
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

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        PointT p1(static_cast<f64>(src_idx) + 0.1f, static_cast<f64>(src_idx) - 0.3f);
        PointT p2(static_cast<f64>(src_idx) + 0.5f, static_cast<f64>(src_idx) - 0.7f);
        PointT p3(static_cast<f64>(src_idx) + 0.2f, static_cast<f64>(src_idx) - 0.4f);
        PointT p4(static_cast<f64>(src_idx) + 0.6f, static_cast<f64>(src_idx) - 0.8f);

        BoxT bounding_box(PointT(static_cast<f64>(src_idx) + 0.1f, static_cast<f64>(src_idx) - 0.3f),
                          PointT(static_cast<f64>(src_idx) + 0.6f, static_cast<f64>(src_idx) - 0.8f));

        Value vx = target_column_vector.GetValue(i);

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
}
#endif
TEST_P(ColumnVectorGeoTest, flat_circle) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kCircle);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        column_vector.AppendByPtr((ptr_t)(&circle));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorGeoTest, contant_circle) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kCircle);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 24u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorGeoTest, circle_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kCircle);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(2 * i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(2 * i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(2 * i) + 7.9f);
    }
}

TEST_P(ColumnVectorGeoTest, circle_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kCircle);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(src_idx) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(src_idx) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(src_idx) + 7.9f);
    }
}
#endif