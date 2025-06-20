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

#if 0

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

using namespace infinity;
class ColumnVectorBlobTest : public BaseTest {};

TEST_F(ColumnVectorBlobTest, flat_blob) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBlob);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        //        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.tail_index_, clone_column_vector.tail_index_);
    EXPECT_EQ(column_vector.capacity_, clone_column_vector.capacity_);
    EXPECT_EQ(column_vector.data_type_, clone_column_vector.data_type_);
    EXPECT_EQ(column_vector.data_ptr_, clone_column_vector.data_ptr_);
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type_, clone_column_vector.vector_type_);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        //        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);

    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        //        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);

    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        column_vector.AppendByPtr((ptr_t)(&b1));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.blob, b1);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorBlobTest, contant_blob) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBlob);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    auto tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);

    for (i64 i = 0; i < 1; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    for (i64 i = 0; i < 1; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);

    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kHeap);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), UnrecoverableException);
    tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);
    for (i64 i = 0; i < 1; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_F(ColumnVectorBlobTest, blob_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBlob);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
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
        i64 blob_len = 2 * i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }
}

TEST_F(ColumnVectorBlobTest, blob_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBlob);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
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
        i64 blob_len = src_idx + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }
}
#endif
