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

module infinity_core:ut.column_vector_row;

import :ut.base_test;
import :infinity_exception;
import :logger;
import :column_vector;
import :value;
import :default_values;
import :third_party;
import :stl;
import :selection;
import :vector_buffer;
import :infinity_context;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;
import compilation_config;

using namespace infinity;

class ColumnVectorRowTest : public BaseTest {
    void SetUp() override {
        using namespace infinity;

        LoggerConfig logger_config;
        logger_config.log_level_ = LogLevel::kOff;
        Logger::Initialize(logger_config);
    }

    void TearDown() override {
        using namespace infinity;

        Logger::Shutdown();
    }
};

TEST_F(ColumnVectorRowTest, flat_row) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kRowID);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), u64(DEFAULT_VECTOR_SIZE));
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        Value v = Value::MakeRow(row_id);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);

        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
        EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(i + 1), UnrecoverableException);
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
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);
    //    EXPECT_EQ(column_vector.data_type(), DataType(LogicalType::kInvalid));
    //    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kInvalid);

    // ====
    //    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.Initialize(), UnrecoverableException);
    //    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.Initialize(), UnrecoverableException);
    //    column_vector.SetVectorType(ColumnVectorType::kFlat);
    column_vector.Initialize();

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        column_vector.AppendByPtr((ptr_t)(&row_id));
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
        EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValueByIndex(0, column_vector.GetValueByIndex(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValueByIndex(0);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorRowTest, contant_row) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kRowID);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        Value v = Value::MakeRow(row_id);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
        EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);

    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(0), UnrecoverableException);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        Value v = Value::MakeRow(row_id);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
        EXPECT_THROW_WITHOUT_STACKTRACE(column_vector.GetValueByIndex(i + 1), UnrecoverableException);
    }
}

TEST_F(ColumnVectorRowTest, row_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kRowID);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        Value v = Value::MakeRow(row_id);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
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
        Value vx = target_column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(2 * i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(2 * i));
    }
}

TEST_F(ColumnVectorRowTest, row_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kRowID);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        RowID row_id{static_cast<u32>(i), static_cast<u32>(i)};
        Value v = Value::MakeRow(row_id);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kRowID);
        EXPECT_EQ(vx.value_.row.segment_id_, static_cast<u32>(i));
        EXPECT_EQ(vx.value_.row.segment_offset_, static_cast<u32>(i));
    }
}
