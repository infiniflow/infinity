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

module;

#include <cassert>

module infinity_core:column_vector.impl;

import :column_vector;
import :selection;
import :infinity_exception;
import :default_values;
import :roaring_bitmap;
import :vector_buffer;
import :logger;
import :value;
import :buffer_manager;
import :status;
import :base_expression;
import :value_expression;
import :expression_binder;
import :cast_function;
import :bound_cast_func;
import :cast_expression;
import :expression_evaluator;
import :expression_state;

import std;
import std.compat;
import third_party;

import sparse_info;
import constant_expr;
import data_type;
import global_resource_usage;
import logical_type;
import embedding_info;
import array_info;
import internal_types;
import logical_type;
import serialize;
import type_info;

namespace infinity {

std::shared_ptr<ColumnVector> ColumnVector::Make(std::shared_ptr<DataType> data_type) { return std::make_shared<ColumnVector>(std::move(data_type)); }

ColumnVector::ColumnVector() : vector_type_(ColumnVectorType::kInvalid) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
}

// Construct a column vector without initialization;
ColumnVector::ColumnVector(std::shared_ptr<DataType> data_type) : vector_type_(ColumnVectorType::kInvalid), data_type_(std::move(data_type)) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
}

// used in BatchInvertTask::BatchInvertTask, keep ObjectCount correct
ColumnVector::ColumnVector(const ColumnVector &right)
    : data_type_size_(right.data_type_size_), buffer_(right.buffer_), nulls_ptr_(right.nulls_ptr_), initialized(right.initialized),
      vector_type_(right.vector_type_), data_type_(right.data_type_), data_ptr_(right.data_ptr_), capacity_(right.capacity_),
      tail_index_(right.tail_index_.load()) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
}

// used in BlockColumnIter, keep ObjectCount correct
ColumnVector::ColumnVector(ColumnVector &&right) noexcept
    : data_type_size_(right.data_type_size_), buffer_(std::move(right.buffer_)), nulls_ptr_(std::move(right.nulls_ptr_)),
      initialized(right.initialized), vector_type_(right.vector_type_), data_type_(std::move(right.data_type_)), data_ptr_(right.data_ptr_),
      capacity_(right.capacity_), tail_index_(right.tail_index_.load()) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
}

ColumnVector &ColumnVector::operator=(ColumnVector &&right) noexcept {
    if (this != &right) {
        data_type_size_ = right.data_type_size_;
        buffer_ = std::move(right.buffer_);
        nulls_ptr_ = std::move(right.nulls_ptr_);
        initialized = right.initialized;
        vector_type_ = right.vector_type_;
        data_type_ = std::move(right.data_type_);
        data_ptr_ = std::exchange(right.data_ptr_, nullptr);
        capacity_ = right.capacity_;
        tail_index_.store(right.tail_index_.load());
    }
    return *this;
}

ColumnVector::~ColumnVector() {
    // Reset(); // TODO: overload copy constructor and move constructor TO PREVENT USING `Reset`
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("ColumnVector");
#endif
}

std::string ColumnVector::ToString() const {
    std::stringstream ss;
    for (size_t idx = 0; idx < tail_index_.load(); ++idx) {
        ss << ToString(idx) << std::endl;
    }
    return ss.str();
}

void ColumnVector::AppendWith(const ColumnVector &other) { return AppendWith(other, 0, other.Size()); }

void ColumnVector::AppendValue(const Value &value) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (tail_index_.load() >= 1) {
            UnrecoverableError("Constant column vector will only have 1 value.");
        }
    }

    size_t tail_index = tail_index_.fetch_add(1);
    if (tail_index >= capacity_) {
        tail_index_.fetch_sub(1);
        UnrecoverableError(fmt::format("Exceed the column vector capacity.({}/{})", tail_index, capacity_));
    }
    SetValueByIndex(tail_index, value);
    LOG_TRACE(fmt::format("ColumnVector::AppendValue: data_ptr_ {:p}, tail_index {}, value {}", data_ptr_, tail_index, value.ToString()));
}

void ColumnVector::SetVectorType(ColumnVectorType vector_type) {
    if (initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type == ColumnVectorType::kInvalid) {
        UnrecoverableError("Invalid column vector type.");
    }
    if (vector_type_ == vector_type) {
        return;
    }
    this->Reset();
    this->Initialize(vector_type, DEFAULT_VECTOR_SIZE);
}

VectorBufferType ColumnVector::GetVectorBufferType(const DataType &data_type) {
    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type.type()) {
            //        case LogicalType::kBlob:
            //        case LogicalType::kBitmap:
            //        case LogicalType::kPolygon:
            //        case LogicalType::kPath:
        case LogicalType::kBoolean: {
            vector_buffer_type = VectorBufferType::kCompactBit;
            break;
        }
        case LogicalType::kArray:
        case LogicalType::kSparse:
        case LogicalType::kVarchar:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            vector_buffer_type = VectorBufferType::kVarBuffer;
            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            UnrecoverableError("Unexpected data type for column vector.");
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }
    return vector_buffer_type;
}

VectorBufferType ColumnVector::InitializeHelper(ColumnVectorType vector_type, size_t capacity) {
    if (initialized) {
        UnrecoverableError("Column vector is already initialized.");
    }
    initialized = true;
    if (data_type_->type() == LogicalType::kInvalid) {
        UnrecoverableError("Attempt to initialize column vector to invalid type.");
    }
    if (vector_type == ColumnVectorType::kInvalid) {
        UnrecoverableError("Attempt to initialize column vector to invalid type.");
    }

    // require BooleanT vector to be initialized with ColumnVectorType::kConstant or ColumnVectorType::kCompactBit
    // if ColumnVectorType::kFlat is used, change it to ColumnVectorType::kCompactBit
    if (data_type_->type() == LogicalType::kBoolean && vector_type == ColumnVectorType::kFlat) {
        vector_type = ColumnVectorType::kCompactBit;
    }

    // TODO: No check on capacity value.

    vector_type_ = vector_type;
    capacity_ = capacity;

    tail_index_.store(0);
    data_type_size_ = data_type_->Size();
    return GetVectorBufferType(*data_type_);
}

void ColumnVector::Initialize(ColumnVectorType vector_type, size_t capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::MakeSharedAllTrue(1);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::MakeSharedAllTrue(capacity_);
        }
        data_ptr_ = buffer_->GetDataMut();
    } else {
        // Initialize after reset will come to this branch
        buffer_->ResetToInit(vector_buffer_type);
    }
}

void ColumnVector::Initialize(BufferObj *buffer_obj,
                              BufferObj *outline_buffer_obj,
                              size_t current_row_count,
                              ColumnVectorMode vector_tipe,
                              ColumnVectorType vector_type,
                              size_t capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() != nullptr) {
        UnrecoverableError("Column vector is already initialized.");
    }

    if (vector_type_ == ColumnVectorType::kConstant) {
        buffer_ = VectorBuffer::Make(buffer_obj, outline_buffer_obj, data_type_size_, 1, vector_buffer_type);
        nulls_ptr_ = Bitmask::MakeSharedAllTrue(1);
    } else {
        buffer_ = VectorBuffer::Make(buffer_obj, outline_buffer_obj, data_type_size_, capacity_, vector_buffer_type);
        nulls_ptr_ = Bitmask::MakeSharedAllTrue(capacity_);
    }
    switch (vector_tipe) {
        case ColumnVectorMode::kReadWrite: {
            data_ptr_ = buffer_->GetDataMut();
            break;
        }
        case ColumnVectorMode::kReadOnly: {
            data_ptr_ = const_cast<char *>(buffer_->GetData());
            break;
        }
    }
    tail_index_.store(current_row_count);
}

void ColumnVector::SetToCatalog(BufferObj *buffer_obj, BufferObj *outline_buffer_obj, ColumnVectorMode vector_tipe) {
    if (buffer_.get() == nullptr) {
        UnrecoverableError("Column vector is not initialized.");
    }

    if (vector_type_ == ColumnVectorType::kConstant) {
        UnrecoverableError("Constant column vector cannot be set to catalog.");
    } else {
        buffer_->SetToCatalog(buffer_obj, outline_buffer_obj);
    }
    switch (vector_tipe) {
        case ColumnVectorMode::kReadWrite: {
            data_ptr_ = buffer_->GetDataMut();
            break;
        }
        case ColumnVectorMode::kReadOnly: {
            data_ptr_ = const_cast<char *>(buffer_->GetData());
            break;
        }
    }
}

void ColumnVector::Initialize(const ColumnVector &other, const Selection &input_select) {
    ColumnVectorType vector_type = other.vector_type_;
    Initialize(vector_type, vector_type == ColumnVectorType::kConstant ? other.capacity() : DEFAULT_VECTOR_SIZE);

    if (vector_type_ == ColumnVectorType::kConstant) {
        size_t tail_index = other.tail_index_.load();
        tail_index_.store(tail_index);
        if (tail_index == 0)
            return;
        CopyRow(other, 0, 0);
    } else {
        size_t tail_index = input_select.Size();
        tail_index_.store(tail_index);

        // Copy data from other column vector to here according to the select
        switch (data_type_->type()) {
            case LogicalType::kBoolean: {
                CopyFrom<BooleanT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kTinyInt: {
                CopyFrom<TinyIntT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kSmallInt: {
                CopyFrom<SmallIntT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kInteger: {
                CopyFrom<IntegerT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kBigInt: {
                CopyFrom<BigIntT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kHugeInt: {
                CopyFrom<HugeIntT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kFloat: {
                CopyFrom<FloatT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kDouble: {
                CopyFrom<DoubleT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kFloat16: {
                CopyFrom<Float16T>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kBFloat16: {
                CopyFrom<BFloat16T>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kMultiVector: {
                CopyFrom<MultiVectorT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kTensor: {
                CopyFrom<TensorT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kTensorArray: {
                CopyFrom<TensorArrayT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
            }
            case LogicalType::kSparse: {
                CopyFrom<SparseT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kDate: {
                CopyFrom<DateT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kTime: {
                CopyFrom<TimeT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kDateTime: {
                CopyFrom<DateTimeT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kTimestamp: {
                CopyFrom<TimestampT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kInterval: {
                CopyFrom<IntervalT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kArray: {
                CopyFrom<ArrayT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            // case LogicalType::kTuple: {
            //    CopyFrom<TupleT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
            //    break;
            // }
            case LogicalType::kPoint: {
                CopyFrom<PointT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kLine: {
                CopyFrom<LineT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kLineSeg: {
                CopyFrom<LineSegT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kBox: {
                CopyFrom<BoxT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
                //            case kPath: {
                //            }
                //            case kPolygon: {
                //            }
            case LogicalType::kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
                //            case kBitmap: {
                //            }
            case LogicalType::kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
                //            case kBlob: {
                //            }
            case LogicalType::kEmbedding: {
                CopyFrom<EmbeddingT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kRowID: {
                CopyFrom<RowID>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kMixed: {
                CopyFrom<MixedT>(other.buffer_.get(), this->buffer_.get(), tail_index, input_select);
                break;
            }
            case LogicalType::kEmptyArray:
            case LogicalType::kNull: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case LogicalType::kMissing: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case LogicalType::kInvalid: {
                UnrecoverableError("Invalid data type");
            }
            default: {
                UnrecoverableError("std::tuple is not implement.");
            }
        }
    }
}

void ColumnVector::Initialize(ColumnVectorType vector_type, const ColumnVector &other, size_t start_idx, size_t end_idx) {
    if (end_idx <= start_idx) {
        UnrecoverableError("End index should larger than start index.");
    }
    Initialize(vector_type, end_idx - start_idx);

    if (vector_type_ == ColumnVectorType::kConstant) {
        tail_index_.store(1);
        CopyRow(other, 0, 0);
    } else {
        tail_index_.store(capacity_);
        // Copy data from other column vector to here according to the range
        switch (data_type_->type()) {
            case LogicalType::kBoolean: {
                CopyFrom<BooleanT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kTinyInt: {
                CopyFrom<TinyIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kSmallInt: {
                CopyFrom<SmallIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kInteger: {
                CopyFrom<IntegerT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kBigInt: {
                CopyFrom<BigIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kHugeInt: {
                CopyFrom<HugeIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kFloat: {
                CopyFrom<FloatT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kDouble: {
                CopyFrom<DoubleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kFloat16: {
                CopyFrom<Float16T>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kBFloat16: {
                CopyFrom<BFloat16T>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kMultiVector: {
                CopyFrom<MultiVectorT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kTensor: {
                CopyFrom<TensorT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kTensorArray: {
                CopyFrom<TensorArrayT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kSparse: {
                CopyFrom<SparseT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kDate: {
                CopyFrom<DateT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kTime: {
                CopyFrom<TimeT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kDateTime: {
                CopyFrom<DateTimeT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kTimestamp: {
                CopyFrom<TimestampT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kInterval: {
                CopyFrom<IntervalT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kArray: {
                CopyFrom<ArrayT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            // case LogicalType::kTuple: {
            //     CopyFrom<TupleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
            //     break;
            // }
            case LogicalType::kPoint: {
                CopyFrom<PointT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kLine: {
                CopyFrom<LineT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kLineSeg: {
                CopyFrom<LineSegT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kBox: {
                CopyFrom<BoxT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kPath: {
                //            }
                //            case kPolygon: {
                //            }
            case LogicalType::kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBitmap: {
                //            }
            case LogicalType::kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBlob: {
                //            }
            case LogicalType::kEmbedding: {
                CopyFrom<EmbeddingT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kRowID: {
                CopyFrom<RowID>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case LogicalType::kMixed: {
                // BUG1213_1: with no initialize of `this->buffer`, this will cause a segfault
                // `VectorBuffer` should be `RAII`
#if 0
                CopyFrom<MixedT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
#endif
                [[fallthrough]];
            }
            case LogicalType::kEmptyArray:
                [[fallthrough]];
            case LogicalType::kNull:
                [[fallthrough]];
            case LogicalType::kMissing: {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case LogicalType::kInvalid: {
                UnrecoverableError("Invalid data type");
            }
            default: {
                UnrecoverableError("std::tuple is not implement.");
            }
        }
    }
}

void ColumnVector::CopyRow(const ColumnVector &other, size_t dst_idx, size_t src_idx) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (data_type_->type() == LogicalType::kInvalid) {
        UnrecoverableError("Data type isn't assigned.");
    }
    if (*data_type_ != *other.data_type_) {
        UnrecoverableError("Data type isn't assigned.");
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (dst_idx != 0) {
            UnrecoverableError("Attempting to access non-zero position of constant vector");
        }
        tail_index_.store(1);
    } else {
        if (dst_idx >= tail_index_.load()) {
            UnrecoverableError("Attempting to access invalid position of target column vector");
        }
    }
    if (other.vector_type_ == ColumnVectorType::kConstant) {
        // Copy from constant vector, only first row have value.
        src_idx = 0;
    }

    if (src_idx >= other.tail_index_.load()) {
        UnrecoverableError("Attempting to access invalid position of target column vector");
    }
    switch (data_type_->type()) {
        case LogicalType::kBoolean: {
            CopyRowFrom<BooleanT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kTinyInt: {
            CopyRowFrom<TinyIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kSmallInt: {
            CopyRowFrom<SmallIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kInteger: {
            CopyRowFrom<IntegerT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kBigInt: {
            CopyRowFrom<BigIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kHugeInt: {
            CopyRowFrom<HugeIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kFloat: {
            CopyRowFrom<FloatT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kDouble: {
            CopyRowFrom<DoubleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kFloat16: {
            CopyRowFrom<Float16T>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kBFloat16: {
            CopyRowFrom<BFloat16T>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kDecimal: {
            CopyRowFrom<DecimalT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kVarchar: {
            CopyRowFrom<VarcharT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kMultiVector: {
            CopyRowFrom<MultiVectorT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kTensor: {
            CopyRowFrom<TensorT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kTensorArray: {
            CopyRowFrom<TensorArrayT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kSparse: {
            CopyRowFrom<SparseT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kDate: {
            CopyRowFrom<DateT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kTime: {
            CopyRowFrom<TimeT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kDateTime: {
            CopyRowFrom<DateTimeT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kTimestamp: {
            CopyRowFrom<TimestampT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kInterval: {
            CopyRowFrom<IntervalT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kArray: {
            CopyRowFrom<ArrayT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        // case LogicalType::kTuple: {
        //     CopyRowFrom<TupleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
        //     break;
        // }
        case LogicalType::kPoint: {
            CopyRowFrom<PointT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kLine: {
            CopyRowFrom<LineT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kLineSeg: {
            CopyRowFrom<LineSegT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kBox: {
            CopyRowFrom<BoxT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case LogicalType::kCircle: {
            CopyRowFrom<CircleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBitmap: {
            //        }
        case LogicalType::kUuid: {
            CopyRowFrom<UuidT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBlob: {
            //        }
        case LogicalType::kEmbedding: {
            CopyRowFrom<EmbeddingT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kRowID: {
            CopyRowFrom<RowID>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kMixed: {
            CopyRowFrom<MixedT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        default: {
            UnrecoverableError("std::tuple is not implement.");
        }
    }
}

std::string ColumnVector::ToString(size_t row_index) const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(row_index))) {
        return "null";
    }

    if (vector_type_ == ColumnVectorType::kConstant) {
        row_index = 0;
    }

    switch (data_type_->type()) {
        case LogicalType::kBoolean: {
            return buffer_->GetCompactBit(row_index) ? "true" : "false";
        }
        case LogicalType::kTinyInt: {
            return std::to_string(((TinyIntT *)data_ptr_)[row_index]);
        }
        case LogicalType::kSmallInt: {
            return std::to_string(((SmallIntT *)data_ptr_)[row_index]);
        }
        case LogicalType::kInteger: {
            return std::to_string(((IntegerT *)data_ptr_)[row_index]);
        }
        case LogicalType::kBigInt: {
            return std::to_string(((BigIntT *)data_ptr_)[row_index]);
        }
        case LogicalType::kFloat: {
            return std::to_string(((FloatT *)data_ptr_)[row_index]);
        }
        case LogicalType::kDouble: {
            return std::to_string(((DoubleT *)data_ptr_)[row_index]);
        }
        case LogicalType::kFloat16: {
            return std::to_string(static_cast<float>(((Float16T *)data_ptr_)[row_index]));
        }
        case LogicalType::kBFloat16: {
            return std::to_string(static_cast<float>(((BFloat16T *)data_ptr_)[row_index]));
        }
        case LogicalType::kVarchar: {
            std::span<const char> data = this->GetVarchar(row_index);
            return {data.data(), data.size()};
        }
        case LogicalType::kDate: {
            return ((DateT *)data_ptr_)[row_index].ToString();
        }
        case LogicalType::kTime: {
            return ((TimeT *)data_ptr_)[row_index].ToString();
        }
        case LogicalType::kDateTime: {
            return ((DateTimeT *)data_ptr_)[row_index].ToString();
        }
        case LogicalType::kTimestamp: {
            return ((TimestampT *)data_ptr_)[row_index].ToString();
        }
        case LogicalType::kInterval:
            [[fallthrough]];
        case LogicalType::kTuple:
            [[fallthrough]];
        case LogicalType::kPoint:
            [[fallthrough]];
        case LogicalType::kLine:
            [[fallthrough]];
        case LogicalType::kLineSeg:
            [[fallthrough]];
        case LogicalType::kBox:
            [[fallthrough]];
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case LogicalType::kCircle:
            [[fallthrough]];
            //        case kBitmap: {
            //        }
        case LogicalType::kUuid:
            [[fallthrough]];
        case LogicalType::kDecimal:
            [[fallthrough]];
        case LogicalType::kMixed:
            [[fallthrough]];
        case LogicalType::kHugeInt: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
            //        case kBlob: {
            //        }
        case LogicalType::kEmbedding: {
            //            RecoverableError(Status::NotSupport("Not implemented"));
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            auto *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            EmbeddingT embedding_element(nullptr, false);
            embedding_element.ptr = (data_ptr_ + row_index * data_type_->type_info()->Size());
            std::string embedding_str = EmbeddingT::Embedding2String(embedding_element, embedding_info->Type(), embedding_info->Dimension());
            embedding_element.SetNull();
            return embedding_str;
        }
        case LogicalType::kMultiVector: {
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                UnrecoverableError("MultiVector type mismatch with unexpected type_info");
            }
            const auto embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            const auto [raw_data, embedding_num] = GetMultiVectorRaw(row_index);
            return MultiVectorT::MultiVector2String(raw_data.data(), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
        }
        case LogicalType::kTensor: {
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                UnrecoverableError("Tensor type mismatch with unexpected type_info");
            }
            const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            auto [raw_data, embedding_num] = GetTensorRaw(row_index);
            return TensorT::Tensor2String(raw_data.data(), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
        }
        case LogicalType::kTensorArray: {
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                UnrecoverableError("TensorArray type mismatch with unexpected type_info");
            }
            const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            std::vector<std::pair<std::span<const char>, size_t>> raw_data = GetTensorArrayRaw(row_index);
            return TensorArrayT::TensorArray2String(raw_data, embedding_info->Type(), embedding_info->Dimension());
        }
        case LogicalType::kSparse: {
            if (data_type_->type_info()->type() != TypeInfoType::kSparse) {
                UnrecoverableError("Sparse type mismatch with unexpected sparse_info");
            }
            const auto *sparse_info = static_cast<SparseInfo *>(data_type_->type_info().get());
            const auto [data_span, index_span, nnz] = this->GetSparseRaw(row_index);
            auto res = SparseT::Sparse2String(data_span.data(), index_span.data(), sparse_info->DataType(), sparse_info->IndexType(), nnz);
            return res;
        }
        case LogicalType::kArray: {
            const auto array_value = GetValueByIndex(row_index);
            return array_value.ToString();
        }
        case LogicalType::kRowID: {
            return (((RowID *)data_ptr_)[row_index]).ToString();
        }
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    return std::string{};
}

Value ColumnVector::GetValueByIndex(size_t index) const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    size_t tail_index = tail_index_.load();
    if (index >= tail_index) {
        UnrecoverableError(
            fmt::format("Attempt to access an invalid index of column vector: {}, current tail index: {}", std::to_string(index), tail_index));
    }

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(index))) {
        return Value::MakeValue(*this->data_type_);
    }

    if (data_type_->type() == LogicalType::kBoolean) {
        // special case for boolean
        return Value::MakeBool(buffer_->GetCompactBit(index));
    }
    return GetArrayValueRecursively(*data_type_, data_ptr_ + index * data_type_->Size());
}

Value ColumnVector::GetArrayValueRecursively(const DataType &data_type, const char *data_ptr) const {
    switch (data_type.type()) {
        case LogicalType::kBoolean: {
            return Value::MakeBool(*reinterpret_cast<const BooleanT *>(data_ptr));
        }
        case LogicalType::kTinyInt: {
            return Value::MakeTinyInt(*reinterpret_cast<const TinyIntT *>(data_ptr));
        }
        case LogicalType::kSmallInt: {
            return Value::MakeSmallInt(*reinterpret_cast<const SmallIntT *>(data_ptr));
        }
        case LogicalType::kInteger: {
            return Value::MakeInt(*reinterpret_cast<const IntegerT *>(data_ptr));
        }
        case LogicalType::kBigInt: {
            return Value::MakeBigInt(*reinterpret_cast<const BigIntT *>(data_ptr));
        }
        case LogicalType::kHugeInt: {
            return Value::MakeHugeInt(*reinterpret_cast<const HugeIntT *>(data_ptr));
        }
        case LogicalType::kFloat: {
            return Value::MakeFloat(*reinterpret_cast<const FloatT *>(data_ptr));
        }
        case LogicalType::kDouble: {
            return Value::MakeDouble(*reinterpret_cast<const DoubleT *>(data_ptr));
        }
        case LogicalType::kFloat16: {
            return Value::MakeFloat16(*reinterpret_cast<const Float16T *>(data_ptr));
        }
        case LogicalType::kBFloat16: {
            return Value::MakeBFloat16(*reinterpret_cast<const BFloat16T *>(data_ptr));
        }
        case LogicalType::kDecimal: {
            return Value::MakeDecimal(*reinterpret_cast<const DecimalT *>(data_ptr), data_type.type_info());
        }
        case LogicalType::kVarchar: {
            const auto varchar = *reinterpret_cast<const VarcharT *>(data_ptr);
            const auto data = GetVarcharInner(varchar);
            return Value::MakeVarchar(data.data(), data.size());
        }
        case LogicalType::kDate: {
            return Value::MakeDate(*reinterpret_cast<const DateT *>(data_ptr));
        }
        case LogicalType::kTime: {
            return Value::MakeTime(*reinterpret_cast<const TimeT *>(data_ptr));
        }
        case LogicalType::kDateTime: {
            return Value::MakeDateTime(*reinterpret_cast<const DateTimeT *>(data_ptr));
        }
        case LogicalType::kTimestamp: {
            return Value::MakeTimestamp(*reinterpret_cast<const TimestampT *>(data_ptr));
        }
        case LogicalType::kInterval: {
            return Value::MakeInterval(*reinterpret_cast<const IntervalT *>(data_ptr));
        }
        case LogicalType::kTuple: {
            UnrecoverableError(fmt::format("{}: std::tuple is not supported", __func__));
        }
        case LogicalType::kPoint: {
            return Value::MakePoint(*reinterpret_cast<const PointT *>(data_ptr));
        }
        case LogicalType::kLine: {
            return Value::MakeLine(*reinterpret_cast<const LineT *>(data_ptr));
        }
        case LogicalType::kLineSeg: {
            return Value::MakeLineSegment(*reinterpret_cast<const LineSegT *>(data_ptr));
        }
        case LogicalType::kBox: {
            return Value::MakeBox(*reinterpret_cast<const BoxT *>(data_ptr));
        }
        case LogicalType::kCircle: {
            return Value::MakeCircle(*reinterpret_cast<const CircleT *>(data_ptr));
        }
        case LogicalType::kUuid: {
            return Value::MakeUuid(*reinterpret_cast<const UuidT *>(data_ptr));
        }
        case LogicalType::kEmbedding: {
            return Value::MakeEmbedding(data_ptr, data_type.type_info());
        }
        case LogicalType::kMultiVector: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            const auto src_multi_vec = *reinterpret_cast<const MultiVectorT *>(data_ptr);
            const auto [raw_data, embedding_num] = GetMultiVector(src_multi_vec, buffer_.get(), embedding_info);
            return Value::MakeMultiVector(raw_data.data(), raw_data.size(), data_type.type_info());
        }
        case LogicalType::kTensor: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            const auto src_tensor = *reinterpret_cast<const TensorT *>(data_ptr);
            const auto [raw_data, embedding_num] = GetTensor(src_tensor, buffer_.get(), embedding_info);
            return Value::MakeTensor(raw_data.data(), raw_data.size(), data_type.type_info());
        }
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            const auto src_tensor_array = *reinterpret_cast<const TensorArrayT *>(data_ptr);
            const auto raw_datas = GetTensorArray(src_tensor_array, buffer_.get(), embedding_info);
            Value value = Value::MakeTensorArray(data_type.type_info());
            for (const auto &[raw_data, embedding_num] : raw_datas) {
                value.AppendToTensorArray(raw_data.data(), raw_data.size());
            }
            return value;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<const SparseInfo *>(data_type.type_info().get());
            const auto sparse = *reinterpret_cast<const SparseT *>(data_ptr);
            const auto [raw_data_ptr, raw_idx_ptr] = buffer_->GetSparseRaw(sparse.file_offset_, sparse.nnz_, sparse_info);
            return Value::MakeSparse(raw_data_ptr, raw_idx_ptr, sparse.nnz_, data_type.type_info());
        }
        case LogicalType::kArray: {
            const auto *array_info = static_cast<const ArrayInfo *>(data_type.type_info().get());
            const auto src_array = *reinterpret_cast<const ArrayT *>(data_ptr);
            const auto [raw_data, element_num] = GetArray(src_array, buffer_.get(), array_info);
            const auto &elem_type = array_info->ElemType();
            const auto elem_size = array_info->ElemSize();
            std::vector<Value> array_values;
            for (size_t i = 0; i < element_num; ++i) {
                auto v = GetArrayValueRecursively(elem_type, raw_data.data() + i * elem_size);
                array_values.emplace_back(std::move(v));
            }
            return Value::MakeArray(std::move(array_values), data_type.type_info());
        }
        case LogicalType::kRowID: {
            return Value::MakeRow(*reinterpret_cast<const RowID *>(data_ptr));
        }
        case LogicalType::kMixed:
            [[fallthrough]];
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError(fmt::format("{}: Attempt to access an unaccepted type", __func__));
        }
    }
    return Value::MakeInvalid();
}

void ColumnVector::SetValueByIndex(size_t index, const Value &value) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    size_t tail_index = tail_index_.load();
    if (index > tail_index || index >= capacity_) {
        UnrecoverableError(
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index),
                        std::to_string(capacity_)));
    }

    // TODO: Check if the value type is same as column vector type
    // TODO: if not, try to cast
    if (value.type() != *data_type_) {
        UnrecoverableError(fmt::format("Attempt to store a different type value into column vector: {}, column vector type: {}",
                                       value.type().ToString(),
                                       data_type_->ToString()));
    }

    // TODO: Check if the value is null, then set the column vector validity.
    if (data_type_->type() == LogicalType::kBoolean) {
        buffer_->SetCompactBit(index, value.GetValue<BooleanT>());
    } else {
        SetArrayValueRecursively(value, data_ptr_ + index * data_type_->Size());
    }
}

void ColumnVector::SetArrayValueRecursively(const Value &value, char *dst_ptr) {
    auto plain_assign_func = [&value, dst_ptr]<typename T>() {
        const auto val = value.GetValue<T>();
        std::memcpy(dst_ptr, &val, sizeof(T));
    };
    switch (const auto &data_type = value.type(); data_type.type()) {
        case LogicalType::kBoolean: {
            plain_assign_func.operator()<BooleanT>();
            break;
        }
        case LogicalType::kTinyInt: {
            plain_assign_func.operator()<TinyIntT>();
            break;
        }
        case LogicalType::kSmallInt: {
            plain_assign_func.operator()<SmallIntT>();
            break;
        }
        case LogicalType::kInteger: {
            plain_assign_func.operator()<IntegerT>();
            break;
        }
        case LogicalType::kBigInt: {
            plain_assign_func.operator()<BigIntT>();
            break;
        }
        case LogicalType::kHugeInt: {
            plain_assign_func.operator()<HugeIntT>();
            break;
        }
        case LogicalType::kFloat: {
            plain_assign_func.operator()<FloatT>();
            break;
        }
        case LogicalType::kDouble: {
            plain_assign_func.operator()<DoubleT>();
            break;
        }
        case LogicalType::kFloat16: {
            plain_assign_func.operator()<Float16T>();
            break;
        }
        case LogicalType::kBFloat16: {
            plain_assign_func.operator()<BFloat16T>();
            break;
        }
        case LogicalType::kDecimal: {
            plain_assign_func.operator()<DecimalT>();
            break;
        }
        case LogicalType::kDate: {
            plain_assign_func.operator()<DateT>();
            break;
        }
        case LogicalType::kTime: {
            plain_assign_func.operator()<TimeT>();
            break;
        }
        case LogicalType::kDateTime: {
            plain_assign_func.operator()<DateTimeT>();
            break;
        }
        case LogicalType::kTimestamp: {
            plain_assign_func.operator()<TimestampT>();
            break;
        }
        case LogicalType::kInterval: {
            plain_assign_func.operator()<IntervalT>();
            break;
        }
        case LogicalType::kTuple: {
            UnrecoverableError(fmt::format("{}: std::tuple is not supported", __func__));
        }
        case LogicalType::kPoint: {
            plain_assign_func.operator()<PointT>();
            break;
        }
        case LogicalType::kLine: {
            plain_assign_func.operator()<LineT>();
            break;
        }
        case LogicalType::kLineSeg: {
            plain_assign_func.operator()<LineSegT>();
            break;
        }
        case LogicalType::kBox: {
            plain_assign_func.operator()<BoxT>();
            break;
        }
        case LogicalType::kCircle: {
            plain_assign_func.operator()<CircleT>();
            break;
        }
        case LogicalType::kUuid: {
            plain_assign_func.operator()<UuidT>();
            break;
        }
        case LogicalType::kRowID: {
            plain_assign_func.operator()<RowID>();
            break;
        }
        case LogicalType::kVarchar: {
            const std::string &data = value.GetVarchar();
            VarcharT varchar{};
            AppendVarcharInner(data, varchar);
            std::memcpy(dst_ptr, &varchar, sizeof(VarcharT));
            break;
        }
        case LogicalType::kEmbedding: {
            const std::span<char> data_span = value.GetEmbedding();
            if (data_span.size() != data_type.Size()) {
                UnrecoverableError(fmt::format("{}: Attempt to store Embedding with different size than column vector type, want {}, got {}",
                                               __func__,
                                               data_type.Size(),
                                               data_span.size()));
            }
            std::memcpy(dst_ptr, data_span.data(), data_span.size());
            break;
        }
        case LogicalType::kMultiVector: {
            const std::span<char> data_span = value.GetEmbedding();
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            MultiVectorT target_multivec{};
            SetMultiVector(target_multivec, buffer_.get(), data_span, embedding_info);
            std::memcpy(dst_ptr, &target_multivec, sizeof(MultiVectorT));
            break;
        }
        case LogicalType::kTensor: {
            const std::span<char> data_span = value.GetEmbedding();
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            TensorT target_tensor{};
            SetTensor(target_tensor, buffer_.get(), data_span, embedding_info);
            std::memcpy(dst_ptr, &target_tensor, sizeof(TensorT));
            break;
        }
        case LogicalType::kTensorArray: {
            const auto &value_tensor_array = value.GetTensorArray();
            std::vector<std::span<const char>> data;
            for (auto &tensor_data : value_tensor_array) {
                const auto raw_data = tensor_data->GetData();
                data.push_back(raw_data);
            }
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            TensorArrayT target_tensor_array{};
            SetTensorArray(target_tensor_array, buffer_.get(), data, embedding_info);
            std::memcpy(dst_ptr, &target_tensor_array, sizeof(TensorArrayT));
            break;
        }
        case LogicalType::kSparse: {
            const auto [nnz, source_idx, source_data] = value.GetSparse();
            SparseT sparse{};
            sparse.nnz_ = nnz;
            if (nnz == 0) {
                sparse.file_offset_ = -1;
            } else {
                const auto *sparse_info = static_cast<const SparseInfo *>(data_type.type_info().get());
                sparse.file_offset_ = buffer_->AppendSparseRaw(source_data.data(), source_idx.data(), nnz, sparse_info);
            }
            std::memcpy(dst_ptr, &sparse, sizeof(SparseT));
            break;
        }
        case LogicalType::kArray: {
            const auto &array_elements = value.GetArray();
            const auto *array_info = static_cast<const ArrayInfo *>(data_type.type_info().get());
            const auto &elem_type = array_info->ElemType();
            const auto elem_size = array_info->ElemSize();
            const auto element_num = array_elements.size();
            const auto element_dst_ptr = std::make_unique_for_overwrite<char[]>(element_num * elem_size);
            for (size_t i = 0; i < element_num; ++i) {
                const auto &element_value = array_elements[i];
                if (element_value.type() != elem_type) {
                    UnrecoverableError(fmt::format("{}: Attempt to store an array with wrong element type, want {}, got {}",
                                                   __func__,
                                                   elem_type.ToString(),
                                                   element_value.type().ToString()));
                }
                SetArrayValueRecursively(element_value, element_dst_ptr.get() + i * elem_size);
            }
            ArrayT array_val{};
            array_val.element_num_ = element_num;
            array_val.file_offset_ = buffer_->AppendArrayRaw(element_dst_ptr.get(), element_num * elem_size);
            std::memcpy(dst_ptr, &array_val, sizeof(ArrayT));
            break;
        }
        case LogicalType::kEmptyArray: {
            break;
        }
        case LogicalType::kMixed:
            [[fallthrough]];
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError(fmt::format("{}: Attempt to access an unaccepted type", __func__));
        }
    }
}

void ColumnVector::Finalize(size_t index) {
    if (index > capacity_) {
        UnrecoverableError(fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));
    }
    tail_index_.store(index);
}

char *ColumnVector::GetRawPtr(size_t index) { return data_ptr_ + index * data_type_->Size(); }

void ColumnVector::SetByRawPtr(size_t index, const char *raw_ptr) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (index >= capacity_) {
        UnrecoverableError(fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));
    }
    size_t tail_index = tail_index_.load();
    if (index > tail_index) {
        UnrecoverableError(
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index),
                        std::to_string(capacity_)));
    }
    // We assume the value_ptr point to the same type data.

    switch (data_type_->type()) {

        case LogicalType::kBoolean: {
            buffer_->SetCompactBit(index, *(BooleanT *)(raw_ptr));
            break;
        }
        case LogicalType::kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = *(TinyIntT *)(raw_ptr);
            break;
        }
        case LogicalType::kSmallInt: {
            ((SmallIntT *)data_ptr_)[index] = *(SmallIntT *)(raw_ptr);
            break;
        }
        case LogicalType::kInteger: {
            ((IntegerT *)data_ptr_)[index] = *(IntegerT *)(raw_ptr);
            break;
        }
        case LogicalType::kBigInt: {
            ((BigIntT *)data_ptr_)[index] = *(BigIntT *)(raw_ptr);
            break;
        }
        case LogicalType::kHugeInt: {
            ((HugeIntT *)data_ptr_)[index] = *(HugeIntT *)(raw_ptr);
            break;
        }
        case LogicalType::kFloat: {
            ((FloatT *)data_ptr_)[index] = *(FloatT *)(raw_ptr);
            break;
        }
        case LogicalType::kFloat16: {
            ((Float16T *)data_ptr_)[index] = *(Float16T *)(raw_ptr);
            break;
        }
        case LogicalType::kBFloat16: {
            ((BFloat16T *)data_ptr_)[index] = *(BFloat16T *)(raw_ptr);
            break;
        }
        case LogicalType::kDouble: {
            ((DoubleT *)data_ptr_)[index] = *(DoubleT *)(raw_ptr);
            break;
        }
        case LogicalType::kDecimal: {
            ((DecimalT *)data_ptr_)[index] = *(DecimalT *)(raw_ptr);
            break;
        }
        case LogicalType::kVarchar: {
            UnrecoverableError("Cannot SetByRawPtr to Varchar.");
        }
        case LogicalType::kDate: {
            ((DateT *)data_ptr_)[index] = *(DateT *)(raw_ptr);
            break;
        }
        case LogicalType::kTime: {
            ((TimeT *)data_ptr_)[index] = *(TimeT *)(raw_ptr);
            break;
        }
        case LogicalType::kDateTime: {
            ((DateTimeT *)data_ptr_)[index] = *(DateTimeT *)(raw_ptr);
            break;
        }
        case LogicalType::kTimestamp: {
            ((TimestampT *)data_ptr_)[index] = *(TimestampT *)(raw_ptr);
            break;
        }
        case LogicalType::kInterval: {
            ((IntervalT *)data_ptr_)[index] = *(IntervalT *)(raw_ptr);
            break;
        }
        case LogicalType::kArray: {
            UnrecoverableError("Cannot SetByRawPtr to Array.");
            break;
        }
        case LogicalType::kTuple: {
            UnrecoverableError("Shouldn't store tuple directly, a tuple is flatten as many columns");
        }
        case LogicalType::kPoint: {
            ((PointT *)data_ptr_)[index] = *(PointT *)(raw_ptr);
            break;
        }
        case LogicalType::kLine: {
            ((LineT *)data_ptr_)[index] = *(LineT *)(raw_ptr);
            break;
        }
        case LogicalType::kLineSeg: {
            ((LineSegT *)data_ptr_)[index] = *(LineSegT *)(raw_ptr);
            break;
        }
        case LogicalType::kBox: {
            ((BoxT *)data_ptr_)[index] = *(BoxT *)(raw_ptr);
            break;
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case LogicalType::kCircle: {
            ((CircleT *)data_ptr_)[index] = *(CircleT *)(raw_ptr);
            break;
        }
            //        case kBitmap: {
            //        }
        case LogicalType::kUuid: {
            ((UuidT *)data_ptr_)[index] = *(UuidT *)(raw_ptr);
            break;
        }
            //        case kBlob: {
            //        }
        case LogicalType::kMultiVector: {
            UnrecoverableError("Cannot SetByRawPtr to MultiVector.");
        }
        case LogicalType::kTensor: {
            UnrecoverableError("Cannot SetByRawPtr to Tensor.");
        }
        case LogicalType::kTensorArray: {
            UnrecoverableError("Cannot SetByRawPtr to TensorArray.");
        }
        case LogicalType::kSparse: {
            UnrecoverableError("Cannot SetByRawPtr to sparse");
        }
        case LogicalType::kEmbedding: {
            //            auto *embedding_ptr = (EmbeddingT *)(value_ptr);
            char *ptr = data_ptr_ + index * data_type_->Size();
            std::memcpy(ptr, raw_ptr, data_type_->Size());
            break;
        }
        case LogicalType::kRowID: {
            ((RowID *)data_ptr_)[index] = *(RowID *)(raw_ptr);
            break;
        }
        case LogicalType::kMixed: {
            ((MixedT *)data_ptr_)[index] = *(MixedT *)(raw_ptr);
            break;
        }
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError("Attempt to access an unaccepted type");

            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::AppendByPtr(const char *value_ptr) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (tail_index_.load() >= 1) {
            UnrecoverableError("Constant column vector will only have 1 value.");
        }
    }
    size_t tail_index = tail_index_.fetch_add(1);
    if (tail_index >= capacity_) {
        tail_index_.fetch_sub(1);
        UnrecoverableError(fmt::format("Exceed the column vector capacity.({}/{})", tail_index, capacity_));
    }
    SetByRawPtr(tail_index, value_ptr);
}

namespace {
std::vector<std::string_view> SplitArrayElement(std::string_view data, char delimiter) {
    size_t data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("Embedding data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    std::vector<std::string_view> ret;
    size_t i = 1, j = 1;
    while (true) {
        if (data[i] == delimiter || data[i] == ' ' || i == data_size - 1) {
            if (i > j) {
                ret.emplace_back(data.begin() + j, data.begin() + i);
            }
            j = i + 1;
        }
        if (i == data_size - 1) {
            break;
        }
        if (data[i] == '[' || data[i] == ']') {
            Status status = Status::ImportFileFormatError("Invalid Embedding data format: should not contain '[' or ']' in embedding!");
            RecoverableError(status);
        }
        i++;
    }
    return ret;
}

std::vector<std::string_view> SplitTensorElement(std::string_view data, const u32 unit_embedding_dim) {
    size_t data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("Tensor data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    bool have_child_embedding = false;
    for (size_t i = 1; i < data_size - 1; ++i) {
        if (data[i] == '[') {
            have_child_embedding = true;
            break;
        }
    }
    if (!have_child_embedding) {
        return SplitArrayElement(data, ',');
    }
    std::string_view child_data = data.substr(1, data_size - 2);
    std::vector<std::string_view> ret;
    size_t bg_id = 0;
    while (true) {
        const auto next_bg_id = child_data.find('[', bg_id);
        if (next_bg_id == std::string_view::npos) {
            break;
        }
        const auto ed_id = child_data.find(']', next_bg_id);
        if (ed_id == std::string_view::npos) {
            RecoverableError(Status::ImportFileFormatError("Tensor data member embedding must be surrounded by [ and ]"));
        }
        if (const auto check_inner_valid = child_data.find('[', next_bg_id + 1); check_inner_valid < ed_id) {
            RecoverableError(Status::ImportFileFormatError("Tensor data format invalid: mismatch of inner '[', ']'."));
        }
        std::vector<std::string_view> sub_result = SplitArrayElement(child_data.substr(next_bg_id, ed_id - next_bg_id + 1), ',');
        if (sub_result.size() != unit_embedding_dim) {
            RecoverableError(Status::ImportFileFormatError("Tensor data member embedding size must be equal to unit embedding dimension."));
        }
        ret.insert(ret.end(), sub_result.begin(), sub_result.end());
        bg_id = ed_id + 1;
    }
    return ret;
}

std::vector<std::vector<std::string_view>> SplitTensorArrayElement(std::string_view data, const u32 unit_embedding_dim) {
    size_t data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("TensorArray data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    std::string_view child_data = data.substr(1, data_size - 2);
    std::vector<std::vector<std::string_view>> ret;
    size_t bg_id = 0;
    while (true) {
        const auto next_bg_id = child_data.find('[', bg_id);
        if (next_bg_id == std::string_view::npos) {
            break;
        }
        i32 depth = -1;
        u32 ed_id = next_bg_id + 1;
        while (ed_id < child_data.size()) {
            if (child_data[ed_id] == '[') {
                --depth;
            } else if (child_data[ed_id] == ']') {
                if (++depth == 0) {
                    break;
                }
            }
            ++ed_id;
        }
        if (ed_id == child_data.size() or depth != 0) {
            Status status = Status::ImportFileFormatError("TensorArray format error");
            RecoverableError(status);
        }
        auto sub_result = SplitTensorElement(child_data.substr(next_bg_id, ed_id - next_bg_id + 1), unit_embedding_dim);
        ret.emplace_back(std::move(sub_result));
        bg_id = ed_id + 1;
    }
    return ret;
}

} // namespace

void ColumnVector::AppendByStringView(std::string_view sv) {
    size_t index = tail_index_.fetch_add(1);
    switch (data_type_->type()) {
        case LogicalType::kBoolean: {
            buffer_->SetCompactBit(index, DataType::StringToValue<BooleanT>(sv));
            break;
        }
        case LogicalType::kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = DataType::StringToValue<TinyIntT>(sv);
            break;
        }
        case LogicalType::kSmallInt: {
            ((SmallIntT *)data_ptr_)[index] = DataType::StringToValue<SmallIntT>(sv);
            break;
        }
        case LogicalType::kInteger: {
            ((IntegerT *)data_ptr_)[index] = DataType::StringToValue<IntegerT>(sv);
            break;
        }
        case LogicalType::kBigInt: {
            ((BigIntT *)data_ptr_)[index] = DataType::StringToValue<BigIntT>(sv);
            break;
        }
        case LogicalType::kFloat: {
            ((FloatT *)data_ptr_)[index] = DataType::StringToValue<FloatT>(sv);
            break;
        }
        case LogicalType::kDouble: {
            ((DoubleT *)data_ptr_)[index] = DataType::StringToValue<DoubleT>(sv);
            break;
        }
        case LogicalType::kFloat16: {
            ((Float16T *)data_ptr_)[index] = DataType::StringToValue<Float16T>(sv);
            break;
        }
        case LogicalType::kBFloat16: {
            ((BFloat16T *)data_ptr_)[index] = DataType::StringToValue<BFloat16T>(sv);
            break;
        }
        case LogicalType::kDate: {
            ((DateT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case LogicalType::kTime: {
            ((TimeT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case LogicalType::kDateTime: {
            ((DateTimeT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case LogicalType::kTimestamp: {
            ((TimestampT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case LogicalType::kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            std::vector<std::string_view> ele_str_views = SplitArrayElement(sv, ',');
            std::string default_val = "0";
            if (embedding_info->Dimension() < ele_str_views.size()) {
                Status status = Status::ImportFileFormatError("Embedding data size exceeds dimension.");
                RecoverableError(status);
            }
            if (ele_str_views.size() < embedding_info->Dimension()) {
                for (size_t i = ele_str_views.size(); i < embedding_info->Dimension(); ++i) {
                    ele_str_views.emplace_back(default_val);
                }
            }
            size_t dst_off = index * data_type_->Size();
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    AppendEmbedding<BooleanT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    AppendEmbedding<u8>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    AppendEmbedding<TinyIntT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    AppendEmbedding<SmallIntT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    AppendEmbedding<IntegerT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    AppendEmbedding<BigIntT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    AppendEmbedding<FloatT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    AppendEmbedding<DoubleT>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    AppendEmbedding<Float16T>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    AppendEmbedding<BFloat16T>(ele_str_views, dst_off);
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid embedding type");
                }
            }
            break;
        }
        case LogicalType::kMultiVector: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            const auto unit_embedding_dim = embedding_info->Dimension();
            std::vector<std::string_view> ele_str_views = SplitTensorElement(sv, unit_embedding_dim);
            if (ele_str_views.size() == 0 or ele_str_views.size() % unit_embedding_dim != 0) {
                Status status = Status::ImportFileFormatError("Embedding data size is not multiple of tensor unit dimension.");
                RecoverableError(status);
            }
            size_t dst_off = index;
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    AppendMultiVector<BooleanT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    AppendMultiVector<u8>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    AppendMultiVector<TinyIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    AppendMultiVector<SmallIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    AppendMultiVector<IntegerT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    AppendMultiVector<BigIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    AppendMultiVector<FloatT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    AppendMultiVector<DoubleT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    AppendMultiVector<Float16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    AppendMultiVector<BFloat16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid embedding type");
                }
            }
            break;
        }
        case LogicalType::kTensor: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            const auto unit_embedding_dim = embedding_info->Dimension();
            std::vector<std::string_view> ele_str_views = SplitTensorElement(sv, unit_embedding_dim);
            if (ele_str_views.size() == 0 or ele_str_views.size() % unit_embedding_dim != 0) {
                Status status = Status::ImportFileFormatError("Embedding data size is not multiple of tensor unit dimension.");
                RecoverableError(status);
            }
            size_t dst_off = index;
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    AppendTensor<BooleanT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    AppendTensor<u8>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    AppendTensor<TinyIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    AppendTensor<SmallIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    AppendTensor<IntegerT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    AppendTensor<BigIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    AppendTensor<FloatT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    AppendTensor<DoubleT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    AppendTensor<Float16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    AppendTensor<BFloat16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid embedding type");
                }
            }
            break;
        }
        case LogicalType::kTensorArray: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            const auto unit_embedding_dim = embedding_info->Dimension();
            std::vector<std::vector<std::string_view>> ele_str_views = SplitTensorArrayElement(sv, unit_embedding_dim);
            if (ele_str_views.size() == 0) {
                Status status = Status::ImportFileFormatError("TensorArray data size is 0.");
                RecoverableError(status);
            }
            for (const auto &ele_str_view : ele_str_views) {
                if (ele_str_view.size() == 0 or ele_str_view.size() % unit_embedding_dim != 0) {
                    Status status = Status::ImportFileFormatError("Tensor dimension is not multiple of embedding unit dimension.");
                    RecoverableError(status);
                }
            }
            size_t dst_off = index;
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    AppendTensorArray<BooleanT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    AppendTensorArray<u8>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    AppendTensorArray<TinyIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    AppendTensorArray<SmallIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    AppendTensorArray<IntegerT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    AppendTensorArray<BigIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    AppendTensorArray<FloatT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    AppendTensorArray<DoubleT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    AppendTensorArray<Float16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    AppendTensorArray<BFloat16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid embedding type");
                }
            }
            break;
        }
        case LogicalType::kVarchar: {
            this->AppendVarcharInner(sv, index);
            break;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<SparseInfo *>(data_type_->type_info().get());
            std::vector<std::string_view> ele_str_views = SplitArrayElement(sv, ',');
            switch (sparse_info->DataType()) {
                case EmbeddingDataType::kElemBit: {
                    AppendSparse<BooleanT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    AppendSparse<u8>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    AppendSparse<TinyIntT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    AppendSparse<SmallIntT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    AppendSparse<IntegerT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    AppendSparse<BigIntT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    AppendSparse<FloatT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    AppendSparse<DoubleT>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    AppendSparse<Float16T>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    AppendSparse<BFloat16T>(ele_str_views, index);
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid sparse type");
                }
            }
            break;
        }
        case LogicalType::kArray: {
            // TODO: '{', '}' in varchar? varchar with or without quotes?
            RecoverableError(Status::NotSupport("Cannot AppendByStringView to Array now."));
            break;
        }
        case LogicalType::kHugeInt:
            [[fallthrough]];
        case LogicalType::kDecimal:
            [[fallthrough]];
        case LogicalType::kInterval:
            [[fallthrough]];
        case LogicalType::kTuple:
            [[fallthrough]];
        case LogicalType::kPoint:
            [[fallthrough]];
        case LogicalType::kLine:
            [[fallthrough]];
        case LogicalType::kLineSeg:
            [[fallthrough]];
        case LogicalType::kBox:
            [[fallthrough]];
        case LogicalType::kCircle:
            [[fallthrough]];
        case LogicalType::kUuid:
            [[fallthrough]];
        case LogicalType::kRowID:
            [[fallthrough]];
        case LogicalType::kMixed:
            [[fallthrough]];
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
    }
}

void ColumnVector::AppendByConstantExpr(const ConstantExpr *const_expr) {
    ExpressionBinder tmp_binder(nullptr);
    auto expr = tmp_binder.BuildValueExpr(*const_expr, nullptr, 0, false);
    auto value_expr = std::dynamic_pointer_cast<ValueExpression>(expr);
    if (value_expr->Type() == *data_type()) {
        auto value_to_insert = value_expr->GetValue();
        AppendValue(value_to_insert);
    } else {
        // try cast
        BoundCastFunc cast = CastFunction::GetBoundFunc(value_expr->Type(), *data_type());
        std::shared_ptr<BaseExpression> cast_expr = std::make_shared<CastExpression>(cast, expr, *data_type());
        std::shared_ptr<ExpressionState> expr_state = ExpressionState::CreateState(cast_expr);
        std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(data_type());
        output_column_vector->Initialize(ColumnVectorType::kConstant, 1);
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);
        evaluator.Execute(cast_expr, expr_state, output_column_vector);
        AppendWith(*output_column_vector, 0, 1);
    }
}

void ColumnVector::AppendWith(const ColumnVector &other, size_t from, size_t count) {
    if (count == 0) {
        return;
    }

    if (*this->data_type_ != *other.data_type_) {
        UnrecoverableError(
            fmt::format("Attempt to append column vector {} to column vector {}", other.data_type_->ToString(), data_type_->ToString()));
    }

    size_t tail_index = tail_index_.load();
    if (tail_index + count > this->capacity_) {
        UnrecoverableError(
            fmt::format("Attempt to append {} rows data to {} rows data, which exceeds {} limit.", count, tail_index, this->capacity_));
    }

    switch (data_type_->type()) {
        case LogicalType::kBoolean: {
            CopyValue<BooleanT>(*this, other, from, count);
            break;
        }
        case LogicalType::kTinyInt: {
            CopyValue<TinyIntT>(*this, other, from, count);
            break;
        }
        case LogicalType::kSmallInt: {
            CopyValue<SmallIntT>(*this, other, from, count);
            break;
        }
        case LogicalType::kInteger: {
            CopyValue<IntegerT>(*this, other, from, count);
            break;
        }
        case LogicalType::kBigInt: {
            CopyValue<BigIntT>(*this, other, from, count);
            break;
        }
        case LogicalType::kHugeInt: {
            CopyValue<HugeIntT>(*this, other, from, count);
            break;
        }
        case LogicalType::kFloat: {
            CopyValue<FloatT>(*this, other, from, count);
            break;
        }
        case LogicalType::kDouble: {
            CopyValue<DoubleT>(*this, other, from, count);
            break;
        }
        case LogicalType::kFloat16: {
            CopyValue<Float16T>(*this, other, from, count);
            break;
        }
        case LogicalType::kBFloat16: {
            CopyValue<BFloat16T>(*this, other, from, count);
            break;
        }
        case LogicalType::kDecimal: {
            CopyValue<DecimalT>(*this, other, from, count);
            break;
        }
        case LogicalType::kVarchar: {
            // Copy string
            auto *base_src_ptr = (VarcharT *)(other.data_ptr_);
            VarcharT *base_dst_ptr = &((VarcharT *)(data_ptr_))[tail_index_.load()];
            for (size_t idx = 0; idx < count; ++idx) {
                VarcharT &src_ref = base_src_ptr[from + idx];
                VarcharT &dst_ref = base_dst_ptr[idx];
                CopyVarchar(dst_ref, buffer_.get(), src_ref, other.buffer_.get());
            }
            break;
        }
        case LogicalType::kMultiVector: {
            auto *base_src_ptr = (MultiVectorT *)(other.data_ptr_);
            MultiVectorT *base_dst_ptr = ((MultiVectorT *)(data_ptr_)) + tail_index_.load();
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            for (size_t idx = 0; idx < count; ++idx) {
                const MultiVectorT &src_ref = base_src_ptr[from + idx];
                MultiVectorT &dst_ref = base_dst_ptr[idx];
                CopyMultiVector(dst_ref, buffer_.get(), src_ref, other.buffer_.get(), embedding_info);
            }
            break;
        }
        case LogicalType::kTensor: {
            auto *base_src_ptr = (TensorT *)(other.data_ptr_);
            TensorT *base_dst_ptr = ((TensorT *)(data_ptr_)) + tail_index_.load();
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            for (size_t idx = 0; idx < count; ++idx) {
                const TensorT &src_ref = base_src_ptr[from + idx];
                TensorT &dst_ref = base_dst_ptr[idx];
                CopyTensor(dst_ref, buffer_.get(), src_ref, other.buffer_.get(), embedding_info);
            }
            break;
        }
        case LogicalType::kTensorArray: {
            auto *base_src_ptr = (TensorArrayT *)(other.data_ptr_);
            TensorArrayT *base_dst_ptr = ((TensorArrayT *)(data_ptr_)) + tail_index_.load();
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            for (size_t idx = 0; idx < count; ++idx) {
                const TensorArrayT &src_ref = base_src_ptr[from + idx];
                TensorArrayT &dst_ref = base_dst_ptr[idx];
                CopyTensorArray(dst_ref, buffer_.get(), src_ref, other.buffer_.get(), embedding_info);
            }
            break;
        }
        case LogicalType::kSparse: {
            const auto *base_src_ptr = reinterpret_cast<const SparseT *>(other.data_ptr_);
            auto *base_dst_ptr = reinterpret_cast<SparseT *>(data_ptr_) + tail_index_.load();
            const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
            for (size_t idx = 0; idx < count; ++idx) {
                const SparseT &src_sparse = base_src_ptr[from + idx];
                SparseT &dst_sparse = base_dst_ptr[idx];
                CopySparse(dst_sparse, buffer_.get(), src_sparse, other.buffer_.get(), sparse_info);
            }
            break;
        }
        case LogicalType::kArray: {
            const auto *base_src_ptr = reinterpret_cast<const ArrayT *>(other.data_ptr_);
            auto *base_dst_ptr = reinterpret_cast<ArrayT *>(data_ptr_) + tail_index_.load();
            const auto *array_info = static_cast<const ArrayInfo *>(data_type_->type_info().get());
            for (size_t idx = 0; idx < count; ++idx) {
                const ArrayT &src_array = base_src_ptr[from + idx];
                ArrayT &dst_array = base_dst_ptr[idx];
                CopyArray(dst_array, buffer_.get(), src_array, other.buffer_.get(), array_info);
            }
            break;
        }
        case LogicalType::kDate: {
            CopyValue<DateT>(*this, other, from, count);
            break;
        }
        case LogicalType::kTime: {
            CopyValue<TimeT>(*this, other, from, count);
            break;
        }
        case LogicalType::kDateTime: {
            CopyValue<DateTimeT>(*this, other, from, count);
            break;
        }
        case LogicalType::kTimestamp: {
            CopyValue<TimestampT>(*this, other, from, count);
            break;
        }
        case LogicalType::kInterval: {
            CopyValue<IntervalT>(*this, other, from, count);
            break;
        }
        case LogicalType::kTuple: {
            UnrecoverableError("Shouldn't store tuple directly, a tuple is flatten as many columns");
            break;
        }
        case LogicalType::kPoint: {
            CopyValue<PointT>(*this, other, from, count);
            break;
        }
        case LogicalType::kLine: {
            CopyValue<LineT>(*this, other, from, count);
            break;
        }
        case LogicalType::kLineSeg: {
            CopyValue<LineSegT>(*this, other, from, count);
            break;
        }
        case LogicalType::kBox: {
            CopyValue<BoxT>(*this, other, from, count);
            break;
        }
            //        case kPath: {
            //        }
        case LogicalType::kCircle: {
            CopyValue<CircleT>(*this, other, from, count);
            break;
        }
            //        case kBitmap: {
            //        }
        case LogicalType::kUuid: {
            CopyValue<UuidT>(*this, other, from, count);
            break;
        }
            //        case kBlob: {
            //        }
        case LogicalType::kEmbedding: {
            //            auto *base_src_ptr = (EmbeddingT *)(other.data_ptr_);
            auto *base_src_ptr = other.data_ptr_;
            char *base_dst_ptr = data_ptr_ + tail_index_.load() * data_type_->Size();
            for (size_t idx = 0; idx < count; ++idx) {
                char *src_ptr = base_src_ptr + (from + idx) * data_type_->Size();
                char *dst_ptr = base_dst_ptr + idx * data_type_->Size();
                std::memcpy(dst_ptr, src_ptr, data_type_->Size());
            }
            break;
        }
        case LogicalType::kRowID: {
            CopyValue<RowID>(*this, other, from, count);
            break;
        }
        case LogicalType::kMixed: {
            CopyValue<MixedT>(*this, other, from, count);
            break;
        }
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    tail_index_.fetch_add(count);
    LOG_TRACE(fmt::format("ColumnVector::AppendWith: data_ptr_ {:p}, tail_index {}, from {}, count {}", data_ptr_, tail_index, from, count));
}

size_t ColumnVector::AppendWith(RowID from, size_t row_count) {
    if (data_type_->type() != LogicalType::kRowID) {
        UnrecoverableError(fmt::format("Only RowID column vector supports this method, current data type: {}", data_type_->ToString()));
    }
    if (row_count == 0) {
        return 0;
    }

    size_t appended_rows = row_count;
    size_t tail_index = tail_index_.load();
    if (tail_index + row_count > capacity_) {
        // attempt to append data rows more than the capacity;
        appended_rows = capacity_ - tail_index;
    }

    char *dst_ptr = data_ptr_ + tail_index * data_type_size_;
    for (size_t i = 0; i < row_count; i++) {
        *(RowID *)dst_ptr = RowID(from.segment_id_, from.segment_offset_ + i);
        dst_ptr += data_type_size_;
    }
    tail_index_.fetch_add(appended_rows);
    return appended_rows;
}

void ColumnVector::ShallowCopy(const ColumnVector &other) {
    if (*this->data_type_ != *other.data_type_) {
        UnrecoverableError(
            fmt::format("Attempt to shallow copy: {} column vector to: {}", other.data_type_->ToString(), this->data_type_->ToString()));
    }
    if (this->buffer_.get() != other.buffer_.get()) {
        this->buffer_ = other.buffer_;
    }
    if (this->nulls_ptr_.get() != other.nulls_ptr_.get()) {
        this->nulls_ptr_ = other.nulls_ptr_;
    }
    this->vector_type_ = other.vector_type_;
    this->data_ptr_ = other.data_ptr_;
    this->data_type_size_ = other.data_type_size_;
    this->initialized = other.initialized;
    this->capacity_ = other.capacity_;
    this->tail_index_.store(other.tail_index_.load());
}

void ColumnVector::Reset() {
    // 1. std::vector type is reset to invalid.
    vector_type_ = ColumnVectorType::kInvalid;

    // 2. Date type won't be changed.

    // 3. Since data type isn't change, data_type_size_ won't be changed, either.

    // 4. For trivial data type, the VectorBuffer will not be reset.
    // But for non-trivial data type, the heap memory manage need to be reset
    if (data_type_->type() == LogicalType::kMixed) {
        // Current solution:
        // std::tuple/Array/Long std::string will use heap memory which isn't managed by ColumnVector.
        // This part of memory should managed by ColumnVector, but it isn't now.
        // So, when ColumnVector is destructed, this part need to free here.
        // TODO: we are going to manage the nested object in ColumnVector.
        for (size_t idx = 0; idx < tail_index_.load(); ++idx) {
            ((MixedT *)data_ptr_)[idx].Reset();
        }
    }

    //    buffer_.reset();
    if (buffer_.get() != nullptr) {
        buffer_->Reset();
    }
    //    data_ptr_ = nullptr;

    // 5. null indicator need to reset
    //    nulls_ptr_.reset();

    // 6. Capacity is set to zero
    capacity_ = 0;

    // 7. Tail index is set to zero
    tail_index_.store(0);

    // 8. Reset initialized flag
    initialized = false;
}

bool ColumnVector::operator==(const ColumnVector &other) const {
    // initialized, data_type_, vector_type_, data_ptr_[0..tail_index_ * data_type_size_]
    if (!this->initialized && !other.initialized)
        return true;
    if (!this->initialized || !other.initialized || this->data_type_.get() == nullptr || other.data_type_.get() == nullptr ||
        (*this->data_type_).operator!=(*other.data_type_) || this->data_type_size_ != other.data_type_size_ ||
        this->vector_type_ != other.vector_type_ || this->tail_index_.load() != other.tail_index_.load() || *this->nulls_ptr_ != *other.nulls_ptr_)
        return false;
    if (data_type_->type() == LogicalType::kVarchar) {
        for (size_t i = 0; i < this->tail_index_.load(); i++) {
            std::span<const char> data1 = this->GetVarchar(i);
            std::span<const char> data2 = other.GetVarchar(i);
            if (data1.size() != data2.size() || std::strncmp(data1.data(), data2.data(), data1.size())) {
                return false;
            }
        }
    } else if (data_type_->type() == LogicalType::kBoolean) {
        return other.data_type_->type() == LogicalType::kBoolean &&
               VectorBuffer::CompactBitIsSame(this->buffer_, this->tail_index_.load(), other.buffer_, other.tail_index_.load());
    } else {
        return 0 == std::memcmp(this->data_ptr_, other.data_ptr_, this->tail_index_.load() * this->data_type_size_);
    }
    return true;
}

i32 ColumnVector::GetSizeInBytes() const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ != ColumnVectorType::kFlat && vector_type_ != ColumnVectorType::kConstant && vector_type_ != ColumnVectorType::kCompactBit) {
        UnrecoverableError(fmt::format("Not supported vector_type {}", int(vector_type_)));
    }
    i32 size = this->data_type_->GetSizeInBytes() + sizeof(ColumnVectorType);
    size += sizeof(i32);
    if (vector_type_ == ColumnVectorType::kCompactBit) {
        size += (this->tail_index_.load() + 7) / 8;
    } else {
        size += this->tail_index_.load() * this->data_type_size_;
    }
    size += buffer_->TotalSize(data_type_.get());
    size += this->nulls_ptr_->GetSizeInBytes();
    return size;
}

void ColumnVector::WriteAdv(char *&ptr) const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ != ColumnVectorType::kFlat && vector_type_ != ColumnVectorType::kConstant && vector_type_ != ColumnVectorType::kCompactBit) {
        UnrecoverableError(fmt::format("Not supported vector_type {}", int(vector_type_)));
    }

    if (data_type_->type() == LogicalType::kHugeInt) {
        UnrecoverableError("Attempt to serialize huge integer type");
    }
    this->data_type_->WriteAdv(ptr);
    WriteBufAdv<ColumnVectorType>(ptr, this->vector_type_);
    // write fixed part
    WriteBufAdv<i32>(ptr, tail_index_.load());
    if (vector_type_ == ColumnVectorType::kCompactBit) {
        size_t byte_size = (this->tail_index_.load() + 7) / 8;
        std::memcpy(ptr, this->data_ptr_, byte_size);
        ptr += byte_size;
    } else {
        std::memcpy(ptr, this->data_ptr_, this->tail_index_.load() * this->data_type_size_);
        ptr += this->tail_index_.load() * this->data_type_size_;
    }
    // write variable part
    buffer_->WriteAdv(ptr, data_type_.get());
    this->nulls_ptr_->WriteAdv(ptr);
    return;
}

std::shared_ptr<ColumnVector> ColumnVector::ReadAdv(const char *&ptr, i32 maxbytes) {
    const char *ptr_end = ptr + maxbytes;
    std::shared_ptr<DataType> data_type = DataType::ReadAdv(ptr, maxbytes);
    ColumnVectorType vector_type = ReadBufAdv<ColumnVectorType>(ptr);
    std::shared_ptr<ColumnVector> column_vector = ColumnVector::Make(data_type);
    column_vector->Initialize(vector_type, DEFAULT_VECTOR_SIZE);
    // read fixed part
    i32 tail_index = ReadBufAdv<i32>(ptr);
    column_vector->tail_index_.store(tail_index);
    if (vector_type == ColumnVectorType::kCompactBit) {
        size_t byte_size = (tail_index + 7) / 8;
        std::memcpy((void *)column_vector->data_ptr_, ptr, byte_size);
        ptr += byte_size;
    } else {
        i32 data_type_size = data_type->Size();
        std::memcpy((void *)column_vector->data_ptr_, ptr, tail_index * data_type_size);
        ptr += tail_index * data_type_size;
    }
    // read variable part
    column_vector->buffer_->ReadAdv(ptr, data_type.get());

    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        UnrecoverableError("ptr goes out of range when reading ColumnVector");
    }
    column_vector->nulls_ptr_ = Bitmask::ReadAdv(ptr, maxbytes);
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        UnrecoverableError("ptr goes out of range when reading ColumnVector");
    }
    return column_vector;
}

//////////////////////////////tensor////////////////////////////////////

void ColumnVector::SetMultiVector(MultiVectorT &dest_multi_vec,
                                  VectorBuffer *dest_buffer,
                                  std::span<const char> data,
                                  const EmbeddingInfo *embedding_info) {
    size_t unit_embedding_bytes = embedding_info->Size();
    if (data.size() % unit_embedding_bytes != 0) {
        UnrecoverableError(fmt::format("Data size {} is not a multiple of embedding size {}", data.size(), unit_embedding_bytes));
    }
    const auto embedding_num = data.size() / unit_embedding_bytes;
    dest_multi_vec.embedding_num_ = embedding_num;
    dest_multi_vec.file_offset_ = dest_buffer->AppendTensorRaw(data.data(), data.size());
}

std::pair<std::span<const char>, size_t>
ColumnVector::GetMultiVector(const MultiVectorT &multi_vec, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info) {
    const auto multi_vec_bytes = multi_vec.embedding_num_ * embedding_info->Size();
    const char *raw_data = src_buffer->GetMultiVectorRaw(multi_vec.file_offset_, multi_vec_bytes);
    return {std::span<const char>(raw_data, multi_vec_bytes), multi_vec.embedding_num_};
}

void ColumnVector::SetTensor(TensorT &dest_tensor, VectorBuffer *dest_buffer, std::span<const char> data, const EmbeddingInfo *embedding_info) {
    size_t unit_embedding_bytes = embedding_info->Size();
    if (data.size() % unit_embedding_bytes != 0) {
        UnrecoverableError(fmt::format("Data size {} is not a multiple of embedding size {}", data.size(), unit_embedding_bytes));
    }
    const auto embedding_num = data.size() / unit_embedding_bytes;
    dest_tensor.embedding_num_ = embedding_num;
    dest_tensor.file_offset_ = dest_buffer->AppendTensorRaw(data.data(), data.size());
}

std::pair<std::span<const char>, size_t>
ColumnVector::GetTensor(const TensorT &src_tensor, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info) {
    const auto tensor_bytes = src_tensor.embedding_num_ * embedding_info->Size();
    const char *raw_data = src_buffer->GetTensorRaw(src_tensor.file_offset_, tensor_bytes);
    return {std::span<const char>(raw_data, tensor_bytes), src_tensor.embedding_num_};
}

void ColumnVector::SetTensorArrayMeta(TensorArrayT &dest_tensor_array, VectorBuffer *dest_buffer, std::span<const TensorT> tensors) {
    dest_tensor_array.tensor_num_ = tensors.size();
    dest_tensor_array.file_offset_ = dest_buffer->AppendTensorArrayMeta(tensors);
}

std::vector<TensorT> ColumnVector::GetTensorArrayMeta(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer) {
    const size_t tensor_num = src_tensor_array.tensor_num_;
    std::vector<TensorT> res(tensor_num);
    const char *data = src_buffer->GetTensorArrayMeta(src_tensor_array.file_offset_, tensor_num);
    std::memcpy(res.data(), data, tensor_num * sizeof(TensorT));
    return res;
}

void ColumnVector::SetTensorArray(TensorArrayT &dest_tensor_array,
                                  VectorBuffer *dest_buffer,
                                  const std::vector<std::span<const char>> &data,
                                  const EmbeddingInfo *embedding_info) {
    size_t tensor_num = data.size();
    std::vector<TensorT> tensor_array_meta(tensor_num);
    for (size_t i = 0; i < tensor_num; ++i) {
        const auto &raw_data = data[i];
        TensorT &target_tensor = tensor_array_meta[i];
        ColumnVector::SetTensor(target_tensor, dest_buffer, raw_data, embedding_info);
    }
    dest_tensor_array.tensor_num_ = tensor_num;
    dest_tensor_array.file_offset_ = dest_buffer->AppendTensorArrayMeta(tensor_array_meta);
}

std::vector<std::pair<std::span<const char>, size_t>>
ColumnVector::GetTensorArray(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info) {
    const std::vector<TensorT> &array_meta = GetTensorArrayMeta(src_tensor_array, src_buffer);
    std::vector<std::pair<std::span<const char>, size_t>> res;
    res.reserve(array_meta.size());
    for (const TensorT &tensor : array_meta) {
        res.push_back(GetTensor(tensor, src_buffer, embedding_info));
    }
    return res;
}

std::pair<std::span<const char>, size_t>
ColumnVector::GetArray(const ArrayT &src_array, const VectorBuffer *src_buffer, const ArrayInfo *array_info) {
    const auto elements_bytes = src_array.element_num_ * array_info->ElemSize();
    const char *raw_data = src_buffer->GetArrayRaw(src_array.file_offset_, elements_bytes);
    return {std::span<const char>(raw_data, elements_bytes), src_array.element_num_};
}

std::pair<std::span<const char>, size_t> ColumnVector::GetMultiVectorRaw(size_t idx) const {
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    const MultiVectorT &src_multi_vec = reinterpret_cast<const MultiVectorT *>(data_ptr_)[idx];
    return ColumnVector::GetMultiVector(src_multi_vec, buffer_.get(), embedding_info);
}

std::pair<std::span<const char>, size_t> ColumnVector::GetTensorRaw(size_t idx) const {
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    const TensorT &src_tensor = reinterpret_cast<const TensorT *>(data_ptr_)[idx];
    return ColumnVector::GetTensor(src_tensor, buffer_.get(), embedding_info);
}

std::vector<std::pair<std::span<const char>, size_t>> ColumnVector::GetTensorArrayRaw(size_t idx) const {
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    const TensorArrayT &src_tensor_array = reinterpret_cast<const TensorArrayT *>(data_ptr_)[idx];
    return ColumnVector::GetTensorArray(src_tensor_array, buffer_.get(), embedding_info);
}

Value ColumnVector::GetArrayValue(const ArrayT &source) const {
    if (data_type_->type() != LogicalType::kArray) {
        UnrecoverableError("Attempt to get array value from non-array column vector");
    }
    return GetArrayValueRecursively(*data_type_, reinterpret_cast<const char *>(&source));
}

void ColumnVector::SetArrayValue(ArrayT &target, const Value &value) {
    if (value.type() != *data_type_) {
        UnrecoverableError("Attempt to set array value with different type");
    }
    SetArrayValueRecursively(value, reinterpret_cast<char *>(&target));
}

bool ColumnVector::AppendUnnestArray(const ColumnVector &other, size_t offset, size_t &array_offset) {
    if (other.data_type_->type() != LogicalType::kArray) {
        UnrecoverableError("Attempt to unnest non-array column vector");
    }
    auto *array_info = static_cast<ArrayInfo *>(other.data_type_->type_info().get());
    if (array_info->ElemType() != *data_type_) {
        UnrecoverableError("Attempt to unnest array with different element type");
    }

    ArrayT &array_val = reinterpret_cast<ArrayT *>(other.data_ptr_)[offset];
    const auto &[span_data, array_size] = other.GetArray(array_val, other.buffer_.get(), array_info);
    const auto *raw_data = span_data.data();

    size_t array_off = array_offset;
    size_t available_space = capacity_ - tail_index_.load();
    array_offset = std::min(array_size, available_space);
    bool complete = false;
    if (array_offset == array_val.element_num_) {
        complete = true;
    }
    switch (array_info->ElemType().type()) {
        case LogicalType::kTinyInt: {
            auto *array_data = reinterpret_cast<const TinyIntT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kSmallInt: {
            auto *array_data = reinterpret_cast<const SmallIntT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kInteger: {
            auto *array_data = reinterpret_cast<const IntegerT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kBigInt: {
            auto *array_data = reinterpret_cast<const BigIntT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kFloat: {
            auto *array_data = reinterpret_cast<const FloatT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kDouble: {
            auto *array_data = reinterpret_cast<const DoubleT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                AppendByPtr(reinterpret_cast<const char *>(&array_data[i]));
            }
            break;
        }
        case LogicalType::kVarchar: {
            auto *array_data = reinterpret_cast<const VarcharT *>(raw_data);
            for (size_t i = array_off; i < array_offset; ++i) {
                const auto &data = GetVarcharInner(array_data[i]);
                AppendVarchar(data);
            }
            break;
        }
        default: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
    }
    return complete;
}

//////////////////////////////tensor end////////////////////////////////////

void ColumnVector::AppendSparseRaw(const char *raw_data_ptr, const char *raw_index_ptr, size_t nnz, size_t dst_off) {
    auto &sparse = reinterpret_cast<SparseT *>(data_ptr_)[dst_off];
    sparse.nnz_ = nnz;
    if (nnz == 0) {
        sparse.file_offset_ = -1;
        return;
    }
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
    sparse.file_offset_ = buffer_->AppendSparseRaw(raw_data_ptr, raw_index_ptr, nnz, sparse_info);
}

std::tuple<std::span<const char>, std::span<const char>, size_t> ColumnVector::GetSparseRaw(size_t index) const {
    const auto &sparse = reinterpret_cast<const SparseT *>(data_ptr_)[index];
    size_t nnz = sparse.nnz_;
    if (nnz == 0) {
        return {std::span<const char>(), std::span<const char>(), 0};
    }
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
    size_t data_size = sparse_info->DataSize(nnz);
    size_t idx_size = sparse_info->IndiceSize(nnz);
    auto [raw_data_ptr, raw_idx_ptr] = buffer_->GetSparseRaw(sparse.file_offset_, nnz, sparse_info);
    return {std::span<const char>(raw_data_ptr, data_size), std::span<const char>(raw_idx_ptr, idx_size), nnz};
}

void ColumnVector::AppendVarcharInner(std::span<const char> data, VarcharT &varchar) {
    varchar.length_ = data.size();
    if (varchar.IsInlined()) {
        std::memcpy(varchar.short_.data_, data.data(), data.size());
    } else {
        varchar.vector_.file_offset_ = buffer_->AppendVarchar(data.data(), data.size());
    }
}

void ColumnVector::AppendVarcharInner(std::span<const char> data, size_t dst_off) {
    auto &varchar = reinterpret_cast<VarcharT *>(data_ptr_)[dst_off];
    AppendVarcharInner(data, varchar);
}

void ColumnVector::AppendVarchar(std::span<const char> data) {
    size_t dst_off = tail_index_.fetch_add(1);
    AppendVarcharInner(data, dst_off);
}

std::span<const char> ColumnVector::GetVarcharInner(const VarcharT &varchar) const {
    i32 length = varchar.length_;
    const char *data = nullptr;
    if (varchar.IsInlined()) {
        data = varchar.short_.data_;
    } else {
        data = buffer_->GetVarchar(varchar.vector_.file_offset_, length);
    }
    return {data, static_cast<size_t>(length)};
}

std::span<const char> ColumnVector::GetVarchar(size_t index) const {
    const auto &varchar = reinterpret_cast<const VarcharT *>(data_ptr_)[index];
    return GetVarcharInner(varchar);
}

template <typename T>
void CopyVarBufferType(T &dst_ref, VectorBuffer *dst_vec_buffer, const T &src_ref, const VectorBuffer *src_vec_buffer, const TypeInfo *type_info) {
    if constexpr (std::is_same_v<T, VarcharT>) {
        CopyVarchar(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer);
    } else if constexpr (std::is_same_v<T, MultiVectorT>) {
        CopyMultiVector(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer, dynamic_cast<const EmbeddingInfo *>(type_info));
    } else if constexpr (std::is_same_v<T, TensorT>) {
        CopyTensor(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer, dynamic_cast<const EmbeddingInfo *>(type_info));
    } else if constexpr (std::is_same_v<T, TensorArrayT>) {
        CopyTensorArray(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer, dynamic_cast<const EmbeddingInfo *>(type_info));
    } else if constexpr (std::is_same_v<T, SparseT>) {
        CopySparse(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer, dynamic_cast<const SparseInfo *>(type_info));
    } else if constexpr (std::is_same_v<T, ArrayT>) {
        CopyArray(dst_ref, dst_vec_buffer, src_ref, src_vec_buffer, dynamic_cast<const ArrayInfo *>(type_info));
    } else {
        static_assert(false, "Unsupported type!");
    }
}

void CopyVarchar(VarcharT &dst_ref, VectorBuffer *dst_vec_buffer, const VarcharT &src_ref, const VectorBuffer *src_vec_buffer) {
    const u32 varchar_len = src_ref.length_;
    dst_ref.length_ = varchar_len;
    if (src_ref.IsInlined()) {
        // Only prefix is enough to contain all string data.
        std::memcpy(dst_ref.short_.data_, src_ref.short_.data_, varchar_len);
    } else {
        std::memcpy(dst_ref.vector_.prefix_, src_ref.vector_.prefix_, VARCHAR_PREFIX_LEN);
        dst_ref.vector_.file_offset_ =
            dst_vec_buffer->AppendVarchar(src_vec_buffer->GetVarchar(src_ref.vector_.file_offset_, varchar_len), varchar_len);
    }
}

void CopyMultiVector(MultiVectorT &dst_ref,
                     VectorBuffer *dst_vec_buffer,
                     const MultiVectorT &src_ref,
                     const VectorBuffer *src_vec_buffer,
                     const EmbeddingInfo *embedding_info) {
    auto [raw_data, embedding_num] = ColumnVector::GetMultiVector(src_ref, src_vec_buffer, embedding_info);
    ColumnVector::SetMultiVector(dst_ref, dst_vec_buffer, raw_data, embedding_info);
}

void CopyTensor(TensorT &dst_ref,
                VectorBuffer *dst_vec_buffer,
                const TensorT &src_ref,
                const VectorBuffer *src_vec_buffer,
                const EmbeddingInfo *embedding_info) {
    auto [raw_data, embedding_num] = ColumnVector::GetTensor(src_ref, src_vec_buffer, embedding_info);
    ColumnVector::SetTensor(dst_ref, dst_vec_buffer, raw_data, embedding_info);
}

void CopyTensorArray(TensorArrayT &dst_ref,
                     VectorBuffer *dst_buffer,
                     const TensorArrayT &src_ref,
                     const VectorBuffer *src_buffer,
                     const EmbeddingInfo *embedding_info) {
    auto data = ColumnVector::GetTensorArray(src_ref, src_buffer, embedding_info);
    std::vector<std::span<const char>> data_vec;
    for (const auto &[raw_data, embedding_num] : data) {
        data_vec.push_back(raw_data);
    }
    ColumnVector::SetTensorArray(dst_ref, dst_buffer, data_vec, embedding_info);
}

void CopySparse(SparseT &dst_sparse,
                VectorBuffer *dst_vec_buffer,
                const SparseT &src_sparse,
                const VectorBuffer *src_vec_buffer,
                const SparseInfo *sparse_info) {
    dst_sparse.nnz_ = src_sparse.nnz_;
    if (src_sparse.nnz_ == 0) {
        dst_sparse.file_offset_ = -1;
        return;
    }
    const auto [raw_data_ptr, raw_idx_ptr] = src_vec_buffer->GetSparseRaw(src_sparse.file_offset_, src_sparse.nnz_, sparse_info);
    dst_sparse.file_offset_ = dst_vec_buffer->AppendSparseRaw(raw_data_ptr, raw_idx_ptr, src_sparse.nnz_, sparse_info);
}

void CopyArray(ArrayT &dst_array,
               VectorBuffer *dst_vec_buffer,
               const ArrayT &src_array,
               const VectorBuffer *src_vec_buffer,
               const ArrayInfo *array_info) {
    const auto [raw_data, element_num] = ColumnVector::GetArray(src_array, src_vec_buffer, array_info);
    dst_array.element_num_ = element_num;
    const auto &elem_type = array_info->ElemType();
    auto loop_copy = [&]<typename T>() {
        const auto elem_size = array_info->ElemSize();
        assert(raw_data.size() == element_num * elem_size);
        const auto dst_array_data = std::make_unique_for_overwrite<char[]>(raw_data.size());
        const auto *elem_type_info = elem_type.type_info().get();
        for (size_t i = 0; i < element_num; ++i) {
            T src_v{}, dst_v{};
            std::memcpy(&src_v, raw_data.data() + i * elem_size, elem_size);
            CopyVarBufferType(dst_v, dst_vec_buffer, src_v, src_vec_buffer, elem_type_info);
            std::memcpy(dst_array_data.get() + i * elem_size, &dst_v, elem_size);
        }
        dst_array.file_offset_ = dst_vec_buffer->AppendArrayRaw(dst_array_data.get(), raw_data.size());
    };
    switch (elem_type.type()) {
        case LogicalType::kBoolean:
            [[fallthrough]];
        case LogicalType::kTinyInt:
            [[fallthrough]];
        case LogicalType::kSmallInt:
            [[fallthrough]];
        case LogicalType::kInteger:
            [[fallthrough]];
        case LogicalType::kBigInt:
            [[fallthrough]];
        case LogicalType::kHugeInt:
            [[fallthrough]];
        case LogicalType::kDecimal:
            [[fallthrough]];
        case LogicalType::kFloat:
            [[fallthrough]];
        case LogicalType::kDouble:
            [[fallthrough]];
        case LogicalType::kFloat16:
            [[fallthrough]];
        case LogicalType::kBFloat16:
            [[fallthrough]];
        case LogicalType::kDate:
            [[fallthrough]];
        case LogicalType::kTime:
            [[fallthrough]];
        case LogicalType::kDateTime:
            [[fallthrough]];
        case LogicalType::kTimestamp:
            [[fallthrough]];
        case LogicalType::kInterval:
            [[fallthrough]];
        case LogicalType::kPoint:
            [[fallthrough]];
        case LogicalType::kLine:
            [[fallthrough]];
        case LogicalType::kLineSeg:
            [[fallthrough]];
        case LogicalType::kBox:
            [[fallthrough]];
        case LogicalType::kCircle:
            [[fallthrough]];
        case LogicalType::kUuid:
            [[fallthrough]];
        case LogicalType::kEmbedding:
            [[fallthrough]];
        case LogicalType::kRowID: {
            dst_array.file_offset_ = dst_vec_buffer->AppendArrayRaw(raw_data.data(), raw_data.size());
            return;
        }
        case LogicalType::kVarchar: {
            loop_copy.operator()<VarcharT>();
            return;
        }
        case LogicalType::kSparse: {
            loop_copy.operator()<SparseT>();
            return;
        }
        case LogicalType::kTensor: {
            loop_copy.operator()<TensorT>();
            return;
        }
        case LogicalType::kTensorArray: {
            loop_copy.operator()<TensorArrayT>();
            return;
        }
        case LogicalType::kMultiVector: {
            loop_copy.operator()<MultiVectorT>();
            return;
        }
        case LogicalType::kArray: {
            loop_copy.operator()<ArrayT>();
            return;
        }
        case LogicalType::kMixed:
            [[fallthrough]];
        case LogicalType::kTuple:
            [[fallthrough]];
        case LogicalType::kNull:
            [[fallthrough]];
        case LogicalType::kEmptyArray:
            [[fallthrough]];
        case LogicalType::kMissing:
            [[fallthrough]];
        case LogicalType::kInvalid: {
            UnrecoverableError(fmt::format("{}: Unhandled element type: {}", __func__, elem_type.ToString()));
            break;
        }
    }
    UnrecoverableError(fmt::format("{}: Unhandled element type: {}", __func__, elem_type.ToString()));
}

} // namespace infinity
