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

#include <cstring>
#include <sstream>

module column_vector;

import stl;
import selection;

import type_info;
import infinity_exception;
import default_values;
import bitmask;
import vector_buffer;
import fix_heap;
import serialize;
import third_party;
import logger;
import value;
import internal_types;
import logical_type;
import buffer_manager;
import status;
import logical_type;
import embedding_info;
import base_expression;
import value_expression;
import expression_binder;
import cast_function;
import bound_cast_func;
import cast_expression;
import expression_evaluator;
import expression_state;
import sparse_info;

import block_column_entry;

namespace infinity {

VectorBufferType ColumnVector::InitializeHelper(ColumnVectorType vector_type, SizeT capacity) {
    if (initialized) {
        String error_message = "Column vector is already initialized.";
        UnrecoverableError(error_message);
    }
    initialized = true;
    if (data_type_->type() == LogicalType::kInvalid) {
        String error_message = "Attempt to initialize column vector to invalid type.";
        UnrecoverableError(error_message);
    }
    if (vector_type == ColumnVectorType::kInvalid) {
        String error_message = "Attempt to initialize column vector to invalid type.";
        UnrecoverableError(error_message);
    }

    // require BooleanT vector to be initialized with ColumnVectorType::kConstant or ColumnVectorType::kCompactBit
    // if ColumnVectorType::kFlat is used, change it to ColumnVectorType::kCompactBit
    if (data_type_->type() == LogicalType::kBoolean && vector_type == ColumnVectorType::kFlat) {
        vector_type = ColumnVectorType::kCompactBit;
    }

    // TODO: No check on capacity value.

    vector_type_ = vector_type;
    capacity_ = capacity;

    tail_index_ = 0;
    data_type_size_ = data_type_->Size();
    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type_->type()) {
            //        case LogicalType::kBlob:
            //        case LogicalType::kBitmap:
            //        case LogicalType::kPolygon:
            //        case LogicalType::kPath:
        case LogicalType::kBoolean: {
            vector_buffer_type = VectorBufferType::kCompactBit;
            break;
        }
        case LogicalType::kSparse:
        case LogicalType::kVarchar:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            vector_buffer_type = VectorBufferType::kVarBuffer;
            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            String error_message = "Unexpected data type for column vector.";
            UnrecoverableError(error_message);
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }
    return vector_buffer_type;
}

void ColumnVector::Initialize(ColumnVectorType vector_type, SizeT capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(8);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(std::bit_ceil(capacity_));
        }
        data_ptr_ = buffer_->GetDataMut();
    } else {
        // Initialize after reset will come to this branch
        buffer_->ResetToInit(vector_buffer_type);
    }
}

void ColumnVector::Initialize(BufferManager *buffer_mgr,
                              BlockColumnEntry *block_column_entry,
                              SizeT current_row_count,
                              ColumnVectorTipe vector_tipe,
                              ColumnVectorType vector_type,
                              SizeT capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() != nullptr) {
        String error_message = "Column vector is already initialized.";
        UnrecoverableError(error_message);
    }

    if (vector_type_ == ColumnVectorType::kConstant) {
        buffer_ = VectorBuffer::Make(buffer_mgr, block_column_entry, data_type_size_, 1, vector_buffer_type);
        nulls_ptr_ = Bitmask::Make(8);
    } else {
        buffer_ = VectorBuffer::Make(buffer_mgr, block_column_entry, data_type_size_, capacity_, vector_buffer_type);
        nulls_ptr_ = Bitmask::Make(capacity_);
    }
    switch (vector_tipe) {
        case ColumnVectorTipe::kReadWrite: {
            data_ptr_ = buffer_->GetDataMut();
            break;
        }
        case ColumnVectorTipe::kReadOnly: {
            data_ptr_ = const_cast<ptr_t>(buffer_->GetData());
            break;
        }
    }
    tail_index_ = current_row_count;
}

void ColumnVector::Initialize(const ColumnVector &other, const Selection &input_select) {
    ColumnVectorType vector_type = other.vector_type_;
    Initialize(vector_type, vector_type == ColumnVectorType::kConstant ? other.capacity() : DEFAULT_VECTOR_SIZE);

    if (vector_type_ == ColumnVectorType::kConstant) {
        tail_index_ = other.tail_index_;
        if (tail_index_ == 0)
            return;
        CopyRow(other, 0, 0);
    } else {
        tail_index_ = input_select.Size();

        // Copy data from other column vector to here according to the select
        switch (data_type_->type()) {
            case kBoolean: {
                CopyFrom<BooleanT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTinyInt: {
                CopyFrom<TinyIntT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kSmallInt: {
                CopyFrom<SmallIntT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kInteger: {
                CopyFrom<IntegerT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kBigInt: {
                CopyFrom<BigIntT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kHugeInt: {
                CopyFrom<HugeIntT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kFloat: {
                CopyFrom<FloatT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kDouble: {
                CopyFrom<DoubleT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kFloat16: {
                CopyFrom<Float16T>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kBFloat16: {
                CopyFrom<BFloat16T>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTensor: {
                CopyFrom<TensorT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTensorArray: {
                CopyFrom<TensorArrayT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
            }
            case kSparse: {
                CopyFrom<SparseT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kDate: {
                CopyFrom<DateT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTime: {
                CopyFrom<TimeT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kDateTime: {
                CopyFrom<DateTimeT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTimestamp: {
                CopyFrom<TimestampT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kInterval: {
                CopyFrom<IntervalT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kArray: {
                CopyFrom<ArrayT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kTuple: {
                CopyFrom<TupleT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kPoint: {
                CopyFrom<PointT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kLine: {
                CopyFrom<LineT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kLineSeg: {
                CopyFrom<LineSegT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kBox: {
                CopyFrom<BoxT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
                //            case kPath: {
                //            }
                //            case kPolygon: {
                //            }
            case kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
                //            case kBitmap: {
                //            }
            case kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
                //            case kBlob: {
                //            }
            case kEmbedding: {
                CopyFrom<EmbeddingT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kRowID: {
                CopyFrom<RowID>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kMixed: {
                CopyFrom<MixedT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kEmptyArray:
            case kNull: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case kMissing: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case kInvalid: {
                String error_message = "Invalid data type";
                UnrecoverableError(error_message);
            }
        }
    }
}

void ColumnVector::Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx) {
    if (end_idx <= start_idx) {
        String error_message = "End index should larger than start index.";
        UnrecoverableError(error_message);
    }
    Initialize(vector_type, end_idx - start_idx);

    if (vector_type_ == ColumnVectorType::kConstant) {
        tail_index_ = 1;
        CopyRow(other, 0, 0);
    } else {
        tail_index_ = capacity_;
        // Copy data from other column vector to here according to the range
        switch (data_type_->type()) {
            case kBoolean: {
                CopyFrom<BooleanT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTinyInt: {
                CopyFrom<TinyIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kSmallInt: {
                CopyFrom<SmallIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kInteger: {
                CopyFrom<IntegerT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBigInt: {
                CopyFrom<BigIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kHugeInt: {
                CopyFrom<HugeIntT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kFloat: {
                CopyFrom<FloatT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDouble: {
                CopyFrom<DoubleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kFloat16: {
                CopyFrom<Float16T>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBFloat16: {
                CopyFrom<BFloat16T>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTensor: {
                CopyFrom<TensorT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTensorArray: {
                CopyFrom<TensorArrayT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kSparse: {
                CopyFrom<SparseT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDate: {
                CopyFrom<DateT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTime: {
                CopyFrom<TimeT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDateTime: {
                CopyFrom<DateTimeT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTimestamp: {
                CopyFrom<TimestampT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kInterval: {
                CopyFrom<IntervalT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kArray: {
                CopyFrom<ArrayT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTuple: {
                CopyFrom<TupleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kPoint: {
                CopyFrom<PointT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kLine: {
                CopyFrom<LineT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kLineSeg: {
                CopyFrom<LineSegT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBox: {
                CopyFrom<BoxT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kPath: {
                //            }
                //            case kPolygon: {
                //            }
            case kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBitmap: {
                //            }
            case kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBlob: {
                //            }
            case kEmbedding: {
                CopyFrom<EmbeddingT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kRowID: {
                CopyFrom<RowID>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kMixed: {
                // BUG1213_1: with no initialize of `this->buffer`, this will cause a segfault
                // `VectorBuffer` should be `RAII`
#if 0
                CopyFrom<MixedT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
#endif
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case kEmptyArray:
            case kNull: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case kMissing: {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            case kInvalid: {
                String error_message = "Invalid data type";
                UnrecoverableError(error_message);
            }
        }
    }
}

void ColumnVector::CopyRow(const ColumnVector &other, SizeT dst_idx, SizeT src_idx) {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (data_type_->type() == LogicalType::kInvalid) {
        String error_message = "Data type isn't assigned.";
        UnrecoverableError(error_message);
    }
    if (*data_type_ != *other.data_type_) {
        String error_message = "Data type isn't assigned.";
        UnrecoverableError(error_message);
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (dst_idx != 0) {
            String error_message = "Attempting to access non-zero position of constant vector";
            UnrecoverableError(error_message);
        }
        tail_index_ = 1;
    } else {
        if (dst_idx >= tail_index_) {
            String error_message = "Attempting to access invalid position of target column vector";
            UnrecoverableError(error_message);
        }
    }
    if (other.vector_type_ == ColumnVectorType::kConstant) {
        // Copy from constant vector, only first row have value.
        src_idx = 0;
    }

    if (src_idx >= other.tail_index_) {
        String error_message = "Attempting to access invalid position of target column vector";
        UnrecoverableError(error_message);
    }
    switch (data_type_->type()) {
        case kBoolean: {
            CopyRowFrom<BooleanT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTinyInt: {
            CopyRowFrom<TinyIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kSmallInt: {
            CopyRowFrom<SmallIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kInteger: {
            CopyRowFrom<IntegerT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kBigInt: {
            CopyRowFrom<BigIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kHugeInt: {
            CopyRowFrom<HugeIntT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kFloat: {
            CopyRowFrom<FloatT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kDouble: {
            CopyRowFrom<DoubleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kFloat16: {
            CopyRowFrom<Float16T>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kBFloat16: {
            CopyRowFrom<BFloat16T>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kDecimal: {
            CopyRowFrom<DecimalT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kVarchar: {
            CopyRowFrom<VarcharT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTensor: {
            CopyRowFrom<TensorT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTensorArray: {
            CopyRowFrom<TensorArrayT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kSparse: {
            CopyRowFrom<SparseT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kDate: {
            CopyRowFrom<DateT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTime: {
            CopyRowFrom<TimeT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kDateTime: {
            CopyRowFrom<DateTimeT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTimestamp: {
            CopyRowFrom<TimestampT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kInterval: {
            CopyRowFrom<IntervalT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kArray: {
            CopyRowFrom<ArrayT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kTuple: {
            CopyRowFrom<TupleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kPoint: {
            CopyRowFrom<PointT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kLine: {
            CopyRowFrom<LineT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kLineSeg: {
            CopyRowFrom<LineSegT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kBox: {
            CopyRowFrom<BoxT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            CopyRowFrom<CircleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            CopyRowFrom<UuidT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBlob: {
            //        }
        case kEmbedding: {
            CopyRowFrom<EmbeddingT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kRowID: {
            CopyRowFrom<RowID>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kMixed: {
            CopyRowFrom<MixedT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kEmptyArray:
        case kNull: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kMissing: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kInvalid: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
    }
}

String ColumnVector::ToString(SizeT row_index) const {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(row_index))) {
        return "null";
    }

    switch (data_type_->type()) {
        case kBoolean: {
            return buffer_->GetCompactBit(row_index) ? "true" : "false";
        }
        case kTinyInt: {
            return std::to_string(((TinyIntT *)data_ptr_)[row_index]);
        }
        case kSmallInt: {
            return std::to_string(((SmallIntT *)data_ptr_)[row_index]);
        }
        case kInteger: {
            return std::to_string(((IntegerT *)data_ptr_)[row_index]);
        }
        case kBigInt: {
            return std::to_string(((BigIntT *)data_ptr_)[row_index]);
        }
        case kHugeInt: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kFloat: {
            return std::to_string(((FloatT *)data_ptr_)[row_index]);
        }
        case kDouble: {
            return std::to_string(((DoubleT *)data_ptr_)[row_index]);
        }
        case kFloat16: {
            return std::to_string(static_cast<float>(((Float16T *)data_ptr_)[row_index]));
        }
        case kBFloat16: {
            return std::to_string(static_cast<float>(((BFloat16T *)data_ptr_)[row_index]));
        }
        case kDecimal: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kVarchar: {
            Span<const char> data = this->GetVarchar(row_index);
            return {data.data(), data.size()};
        }
        case kDate: {
            return ((DateT *)data_ptr_)[row_index].ToString();
        }
        case kTime: {
            return ((TimeT *)data_ptr_)[row_index].ToString();
        }
        case kDateTime: {
            return ((DateTimeT *)data_ptr_)[row_index].ToString();
        }
        case kTimestamp: {
            return ((TimestampT *)data_ptr_)[row_index].ToString();
        }
        case kInterval: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kArray: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kTuple: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kPoint: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kLine: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kLineSeg: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        case kBox: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
            //        case kBlob: {
            //        }
        case kEmbedding: {
            //            RecoverableError(Status::NotSupport("Not implemented"));
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                Status status = Status::NotSupport("Not implemented");
                RecoverableError(status);
            }
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            EmbeddingT embedding_element(nullptr, false);
            embedding_element.ptr = (data_ptr_ + row_index * data_type_->type_info()->Size());
            String embedding_str = EmbeddingT::Embedding2String(embedding_element, embedding_info->Type(), embedding_info->Dimension());
            embedding_element.SetNull();
            return embedding_str;
        }
        case kTensor: {
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                String error_message = "Tensor type mismatch with unexpected type_info";
                UnrecoverableError(error_message);
            }
            const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            auto [raw_data, embedding_num] = GetTensorRaw(row_index);
            return TensorT::Tensor2String(raw_data.data(), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
        }
        case kTensorArray: {
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                String error_message = "TensorArray type mismatch with unexpected type_info";
                UnrecoverableError(error_message);
            }
            const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            Vector<Pair<Span<const char>, SizeT>> raw_data = GetTensorArrayRaw(row_index);
            return TensorArrayT::TensorArray2String(raw_data, embedding_info->Type(), embedding_info->Dimension());
        }
        case kSparse: {
            if (data_type_->type_info()->type() != TypeInfoType::kSparse) {
                String error_message = "Sparse type mismatch with unexpected sparse_info";
                UnrecoverableError(error_message);
            }
            const auto *sparse_info = static_cast<SparseInfo *>(data_type_->type_info().get());
            const auto [data_span, index_span, nnz] = this->GetSparseRaw(row_index);
            auto res = SparseT::Sparse2String(data_span.data(), index_span.data(), sparse_info->DataType(), sparse_info->IndexType(), nnz);
            return res;
        }
        case kRowID: {
            return (((RowID *)data_ptr_)[row_index]).ToString();
        }
        case kMixed: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        default: {
            String error_message = "Attempt to access an unaccepted type";
            UnrecoverableError(error_message);
            // Null/Missing/Invalid
        }
    }
    return String();
}

Value ColumnVector::GetValue(SizeT index) const {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (index >= tail_index_) {
        String error_message = fmt::format("Attempt to access an invalid index of column vector: {}", std::to_string(index));
        UnrecoverableError(error_message);
    }

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(index))) {
        return Value::MakeValue(*this->data_type_);
    }

    switch (data_type_->type()) {

        case kBoolean: {
            return Value::MakeBool(buffer_->GetCompactBit(index));
        }
        case kTinyInt: {
            return Value::MakeTinyInt(((TinyIntT *)data_ptr_)[index]);
        }
        case kSmallInt: {
            return Value::MakeSmallInt(((SmallIntT *)data_ptr_)[index]);
        }
        case kInteger: {
            return Value::MakeInt(((IntegerT *)data_ptr_)[index]);
        }
        case kBigInt: {
            return Value::MakeBigInt(((BigIntT *)data_ptr_)[index]);
        }
        case kHugeInt: {
            return Value::MakeHugeInt(((HugeIntT *)data_ptr_)[index]);
        }
        case kFloat: {
            return Value::MakeFloat(((FloatT *)data_ptr_)[index]);
        }
        case kDouble: {
            return Value::MakeDouble(((DoubleT *)data_ptr_)[index]);
        }
        case kFloat16: {
            return Value::MakeFloat16(((Float16T *)data_ptr_)[index]);
        }
        case kBFloat16: {
            return Value::MakeBFloat16(((BFloat16T *)data_ptr_)[index]);
        }
        case kDecimal: {
            return Value::MakeDecimal(((DecimalT *)data_ptr_)[index], data_type_->type_info());
        }
        case kVarchar: {
            Span<const char> data = this->GetVarchar(index);
            return Value::MakeVarchar(data.data(), data.size());
        }
        case kDate: {
            return Value::MakeDate(((DateT *)data_ptr_)[index]);
        }
        case kTime: {
            return Value::MakeTime(((TimeT *)data_ptr_)[index]);
        }
        case kDateTime: {
            return Value::MakeDateTime(((DateTimeT *)data_ptr_)[index]);
        }
        case kTimestamp: {
            return Value::MakeTimestamp(((TimestampT *)data_ptr_)[index]);
        }
        case kInterval: {
            return Value::MakeInterval(((IntervalT *)data_ptr_)[index]);
        }
        case kTuple: {
            String error_message = "Shouldn't access tuple directly, a tuple is flatten as many columns";
            UnrecoverableError(error_message);
        }
        case kPoint: {
            return Value::MakePoint(((PointT *)data_ptr_)[index]);
        }
        case kLine: {
            return Value::MakeLine(((LineT *)data_ptr_)[index]);
        }
        case kLineSeg: {
            return Value::MakeLineSegment(((LineSegT *)data_ptr_)[index]);
        }
        case kBox: {
            return Value::MakeBox(((BoxT *)data_ptr_)[index]);
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            return Value::MakeCircle(((CircleT *)data_ptr_)[index]);
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            return Value::MakeUuid(((UuidT *)data_ptr_)[index]);
        }
            //        case kBlob: {
            //        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            return Value::MakeEmbedding(ptr, data_type_->type_info());
        }
        case kTensor: {
            auto [raw_data, embedding_num] = this->GetTensorRaw(index);
            return Value::MakeTensor(raw_data.data(), raw_data.size(), data_type_->type_info());
        }
        case kTensorArray: {
            auto raw_datas = this->GetTensorArrayRaw(index);
            Value value = Value::MakeTensorArray(data_type_->type_info());
            for (const auto &[raw_data, embedding_num] : raw_datas) {
                value.AppendToTensorArray(raw_data.data(), raw_data.size());
            }
            return value;
        }
        case kSparse: {
            auto [data_span, index_span, nnz] = this->GetSparseRaw(index);
            return Value::MakeSparse(data_span.data(), index_span.data(), nnz, data_type_->type_info());
        }
        case kRowID: {
            return Value::MakeRow(((RowID *)data_ptr_)[index]);
        }
        default: {
            String error_message = "Attempt to access an unaccepted type";
            UnrecoverableError(error_message);
            // Null/Missing/Invalid
        }
    }
    return Value::MakeInvalid();
}

void ColumnVector::SetValue(SizeT index, const Value &value) {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (index > tail_index_) {
        String error_message =
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index_),
                        std::to_string(capacity_));
        UnrecoverableError(error_message);
    }

    // TODO: Check if the value type is same as column vector type
    // TODO: if not, try to cast
    if (value.type() != *data_type_) {
        String error_message = fmt::format("Attempt to store a different type value into column vector: {}, column vector type: {}",
                                           value.type().ToString(),
                                           data_type_->ToString());
        UnrecoverableError(error_message);
    }

    // TODO: Check if the value is null, then set the column vector validity.
    switch (data_type_->type()) {

        case kBoolean: {
            buffer_->SetCompactBit(index, value.GetValue<BooleanT>());
            break;
        }
        case kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = value.GetValue<TinyIntT>();
            break;
        }
        case kSmallInt: {
            ((SmallIntT *)data_ptr_)[index] = value.GetValue<SmallIntT>();
            break;
        }
        case kInteger: {
            ((IntegerT *)data_ptr_)[index] = value.GetValue<IntegerT>();
            break;
        }
        case kBigInt: {
            ((BigIntT *)data_ptr_)[index] = value.GetValue<BigIntT>();
            break;
        }
        case kHugeInt: {
            ((HugeIntT *)data_ptr_)[index] = value.GetValue<HugeIntT>();
            break;
        }
        case kFloat: {
            ((FloatT *)data_ptr_)[index] = value.GetValue<FloatT>();
            break;
        }
        case kDouble: {
            ((DoubleT *)data_ptr_)[index] = value.GetValue<DoubleT>();
            break;
        }
        case kFloat16: {
            ((Float16T *)data_ptr_)[index] = value.GetValue<Float16T>();
            break;
        }
        case kBFloat16: {
            ((BFloat16T *)data_ptr_)[index] = value.GetValue<BFloat16T>();
            break;
        }
        case kDecimal: {
            ((DecimalT *)data_ptr_)[index] = value.GetValue<DecimalT>();
            break;
        }
        case kDate: {
            ((DateT *)data_ptr_)[index] = value.GetValue<DateT>();
            break;
        }
        case kTime: {
            ((TimeT *)data_ptr_)[index] = value.GetValue<TimeT>();
            break;
        }
        case kDateTime: {
            ((DateTimeT *)data_ptr_)[index] = value.GetValue<DateTimeT>();
            break;
        }
        case kTimestamp: {
            ((TimestampT *)data_ptr_)[index] = value.GetValue<TimestampT>();
            break;
        }
        case kInterval: {
            ((IntervalT *)data_ptr_)[index] = value.GetValue<IntervalT>();
            break;
        }
        case kArray: {
            ((ArrayT *)data_ptr_)[index] = value.GetValue<ArrayT>();
            break;
        }
        case kTuple: {
            String error_message = "Shouldn't store tuple directly, a tuple is flatten as many columns";
            UnrecoverableError(error_message);
        }
        case kPoint: {
            ((PointT *)data_ptr_)[index] = value.GetValue<PointT>();
            break;
        }
        case kLine: {
            ((LineT *)data_ptr_)[index] = value.GetValue<LineT>();
            break;
        }
        case kLineSeg: {
            ((LineSegT *)data_ptr_)[index] = value.GetValue<LineSegT>();
            break;
        }
        case kBox: {
            ((BoxT *)data_ptr_)[index] = value.GetValue<BoxT>();
            break;
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = value.GetValue<CircleT>();
            break;
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = value.GetValue<UuidT>();
            break;
        }
            //        case kBlob: {
            //        }
        case kRowID: {
            ((RowID *)data_ptr_)[index] = value.GetValue<RowID>();
            break;
        }
        case kVarchar: {
            const String &data = value.GetVarchar();
            this->AppendVarcharInner({data.data(), data.size()}, index);
            break;
        }
        case kTensor: {
            Span<char> data_span = value.GetEmbedding();
            TensorT &target_tensor = reinterpret_cast<TensorT *>(data_ptr_)[index];
            const auto *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            ColumnVector::SetTensor(target_tensor, buffer_.get(), data_span, embedding_info);
            break;
        }
        case kTensorArray: {
            TensorArrayT &target_tensor_array = reinterpret_cast<TensorArrayT *>(data_ptr_)[index];
            const auto &value_tensor_array = value.GetTensorArray();
            Vector<Span<const char>> data;
            for (auto &tensor_data : value_tensor_array) {
                Span<const char> raw_data = tensor_data->GetData();
                data.emplace_back(raw_data);
            }
            const auto *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            ColumnVector::SetTensorArray(target_tensor_array, buffer_.get(), data, embedding_info);
            break;
        }
        case kSparse: {
            auto [source_nnz, source_indice, source_data] = value.GetSparse();
            AppendSparseRaw(source_data.data(), source_indice.data(), source_nnz, index);
            break;
        }
        case kEmbedding: {
            Span<char> data_span = value.GetEmbedding();
            const_ptr_t src_ptr = data_span.data();
            SizeT src_size = data_span.size();
            if (src_size != data_type_->Size()) {
                String error_message = fmt::format("Attempt to store a value with different size than column vector type, want {}, got {}",
                                                   data_type_->Size(),
                                                   src_size);
                UnrecoverableError(error_message);
            }
            ptr_t dst_ptr = data_ptr_ + index * data_type_->Size();
            std::memcpy(dst_ptr, src_ptr, src_size);
            break;
        }
        case kEmptyArray: {
            break;
        }
        default: {
            String error_message = "Attempt to access an unaccepted type";
            UnrecoverableError(error_message);
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::Finalize(SizeT index) {
    if (index > capacity_) {
        String error_message = fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_);
        UnrecoverableError(error_message);
    }
    tail_index_ = index;
}

void ColumnVector::SetByRawPtr(SizeT index, const_ptr_t raw_ptr) {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (index > capacity_) {
        String error_message = fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_);
        UnrecoverableError(error_message);
    }
    if (index > tail_index_) {
        String error_message =
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index_),
                        std::to_string(capacity_));
        UnrecoverableError(error_message);
    }
    // We assume the value_ptr point to the same type data.

    switch (data_type_->type()) {

        case kBoolean: {
            buffer_->SetCompactBit(index, *(BooleanT *)(raw_ptr));
            break;
        }
        case kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = *(TinyIntT *)(raw_ptr);
            break;
        }
        case kSmallInt: {
            ((SmallIntT *)data_ptr_)[index] = *(SmallIntT *)(raw_ptr);
            break;
        }
        case kInteger: {
            ((IntegerT *)data_ptr_)[index] = *(IntegerT *)(raw_ptr);
            break;
        }
        case kBigInt: {
            ((BigIntT *)data_ptr_)[index] = *(BigIntT *)(raw_ptr);
            break;
        }
        case kHugeInt: {
            ((HugeIntT *)data_ptr_)[index] = *(HugeIntT *)(raw_ptr);
            break;
        }
        case kFloat: {
            ((FloatT *)data_ptr_)[index] = *(FloatT *)(raw_ptr);
            break;
        }
        case kFloat16: {
            ((Float16T *)data_ptr_)[index] = *(Float16T *)(raw_ptr);
            break;
        }
        case kBFloat16: {
            ((BFloat16T *)data_ptr_)[index] = *(BFloat16T *)(raw_ptr);
            break;
        }
        case kDouble: {
            ((DoubleT *)data_ptr_)[index] = *(DoubleT *)(raw_ptr);
            break;
        }
        case kDecimal: {
            ((DecimalT *)data_ptr_)[index] = *(DecimalT *)(raw_ptr);
            break;
        }
        case kVarchar: {
            String error_message = "Cannot SetByRawPtr to Varchar.";
            UnrecoverableError(error_message);
        }
        case kDate: {
            ((DateT *)data_ptr_)[index] = *(DateT *)(raw_ptr);
            break;
        }
        case kTime: {
            ((TimeT *)data_ptr_)[index] = *(TimeT *)(raw_ptr);
            break;
        }
        case kDateTime: {
            ((DateTimeT *)data_ptr_)[index] = *(DateTimeT *)(raw_ptr);
            break;
        }
        case kTimestamp: {
            ((TimestampT *)data_ptr_)[index] = *(TimestampT *)(raw_ptr);
            break;
        }
        case kInterval: {
            ((IntervalT *)data_ptr_)[index] = *(IntervalT *)(raw_ptr);
            break;
        }
        case kArray: {
            ((ArrayT *)data_ptr_)[index] = *(ArrayT *)(raw_ptr);
            break;
        }
        case kTuple: {
            String error_message = "Shouldn't store tuple directly, a tuple is flatten as many columns";
            UnrecoverableError(error_message);
        }
        case kPoint: {
            ((PointT *)data_ptr_)[index] = *(PointT *)(raw_ptr);
            break;
        }
        case kLine: {
            ((LineT *)data_ptr_)[index] = *(LineT *)(raw_ptr);
            break;
        }
        case kLineSeg: {
            ((LineSegT *)data_ptr_)[index] = *(LineSegT *)(raw_ptr);
            break;
        }
        case kBox: {
            ((BoxT *)data_ptr_)[index] = *(BoxT *)(raw_ptr);
            break;
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = *(CircleT *)(raw_ptr);
            break;
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = *(UuidT *)(raw_ptr);
            break;
        }
            //        case kBlob: {
            //        }
        case kTensor: {
            String error_message = "Cannot SetByRawPtr to Tensor.";
            UnrecoverableError(error_message);
        }
        case kTensorArray: {
            String error_message = "Cannot SetByRawPtr to TensorArray.";
            UnrecoverableError(error_message);
        }
        case kSparse: {
            String error_message = "Cannot SetByRawPtr to sparse";
            UnrecoverableError(error_message);
        }
        case kEmbedding: {
            //            auto *embedding_ptr = (EmbeddingT *)(value_ptr);
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            std::memcpy(ptr, raw_ptr, data_type_->Size());
            break;
        }
        case kRowID: {
            ((RowID *)data_ptr_)[index] = *(RowID *)(raw_ptr);
            break;
        }
        case kMixed: {
            ((MixedT *)data_ptr_)[index] = *(MixedT *)(raw_ptr);
            break;
        }
        default: {
            String error_message = "Attempt to access an unaccepted type";
            UnrecoverableError(error_message);

            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::AppendByPtr(const_ptr_t value_ptr) {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (tail_index_ >= 1) {
            String error_message = "Constant column vector will only have 1 value.";
            UnrecoverableError(error_message);
        }
    }
    if (tail_index_ >= capacity_) {
        String error_message = fmt::format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_);
        UnrecoverableError(error_message);
    }
    SetByRawPtr(tail_index_++, value_ptr);
}

namespace {
Vector<std::string_view> SplitArrayElement(std::string_view data, char delimiter) {
    SizeT data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("Embedding data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    Vector<std::string_view> ret;
    SizeT i = 1, j = 1;
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

Vector<Vector<std::string_view>> SplitTensorArrayElement(std::string_view data, const u32 unit_embedding_dim) {
    SizeT data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("TensorArray data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    std::string_view child_data = data.substr(1, data_size - 2);
    Vector<Vector<std::string_view>> ret;
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
        auto sub_result = SplitTensorElement(child_data.substr(next_bg_id, ed_id - next_bg_id + 1), ',', unit_embedding_dim);
        ret.emplace_back(std::move(sub_result));
        bg_id = ed_id + 1;
    }
    return ret;
}

} // namespace

void ColumnVector::AppendByStringView(std::string_view sv) {
    SizeT index = tail_index_++;
    switch (data_type_->type()) {
        case kBoolean: {
            buffer_->SetCompactBit(index, DataType::StringToValue<BooleanT>(sv));
            break;
        }
        case kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = DataType::StringToValue<TinyIntT>(sv);
            break;
        }
        case kSmallInt: {
            ((SmallIntT *)data_ptr_)[index] = DataType::StringToValue<SmallIntT>(sv);
            break;
        }
        case kInteger: {
            ((IntegerT *)data_ptr_)[index] = DataType::StringToValue<IntegerT>(sv);
            break;
        }
        case kBigInt: {
            ((BigIntT *)data_ptr_)[index] = DataType::StringToValue<BigIntT>(sv);
            break;
        }
        case kFloat: {
            ((FloatT *)data_ptr_)[index] = DataType::StringToValue<FloatT>(sv);
            break;
        }
        case kDouble: {
            ((DoubleT *)data_ptr_)[index] = DataType::StringToValue<DoubleT>(sv);
            break;
        }
        case kFloat16: {
            ((Float16T *)data_ptr_)[index] = DataType::StringToValue<Float16T>(sv);
            break;
        }
        case kBFloat16: {
            ((BFloat16T *)data_ptr_)[index] = DataType::StringToValue<BFloat16T>(sv);
            break;
        }
        case kDate: {
            ((DateT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case kTime: {
            ((TimeT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case kDateTime: {
            ((DateTimeT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case kTimestamp: {
            ((TimestampT *)data_ptr_)[index].FromString(sv);
            break;
        }
        case kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            Vector<std::string_view> ele_str_views = SplitArrayElement(sv, ',');
            if (embedding_info->Dimension() < ele_str_views.size()) {
                Status status = Status::ImportFileFormatError("Embedding data size exceeds dimension.");
                RecoverableError(status);
            }
            SizeT dst_off = index * data_type_->Size();
            switch (embedding_info->Type()) {
                case kElemBit: {
                    AppendEmbedding<BooleanT>(ele_str_views, dst_off);
                    break;
                }
                case kElemUInt8: {
                    AppendEmbedding<u8>(ele_str_views, dst_off);
                    break;
                }
                case kElemInt8: {
                    AppendEmbedding<TinyIntT>(ele_str_views, dst_off);
                    break;
                }
                case kElemInt16: {
                    AppendEmbedding<SmallIntT>(ele_str_views, dst_off);
                    break;
                }
                case kElemInt32: {
                    AppendEmbedding<IntegerT>(ele_str_views, dst_off);
                    break;
                }
                case kElemInt64: {
                    AppendEmbedding<BigIntT>(ele_str_views, dst_off);
                    break;
                }
                case kElemFloat: {
                    AppendEmbedding<FloatT>(ele_str_views, dst_off);
                    break;
                }
                case kElemDouble: {
                    AppendEmbedding<DoubleT>(ele_str_views, dst_off);
                    break;
                }
                case kElemFloat16: {
                    AppendEmbedding<Float16T>(ele_str_views, dst_off);
                    break;
                }
                case kElemBFloat16: {
                    AppendEmbedding<BFloat16T>(ele_str_views, dst_off);
                    break;
                }
                case kElemInvalid: {
                    String error_message = "Invalid embedding type";
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case kTensor: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            const auto unit_embedding_dim = embedding_info->Dimension();
            Vector<std::string_view> ele_str_views = SplitTensorElement(sv, ',', unit_embedding_dim);
            if (ele_str_views.size() == 0 or ele_str_views.size() % unit_embedding_dim != 0) {
                Status status = Status::ImportFileFormatError("Embedding data size is not multiple of tensor unit dimension.");
                RecoverableError(status);
            }
            SizeT dst_off = index;
            switch (embedding_info->Type()) {
                case kElemBit: {
                    AppendTensor<BooleanT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemUInt8: {
                    AppendTensor<u8>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt8: {
                    AppendTensor<TinyIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt16: {
                    AppendTensor<SmallIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt32: {
                    AppendTensor<IntegerT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt64: {
                    AppendTensor<BigIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemFloat: {
                    AppendTensor<FloatT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemDouble: {
                    AppendTensor<DoubleT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemFloat16: {
                    AppendTensor<Float16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemBFloat16: {
                    AppendTensor<BFloat16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInvalid: {
                    String error_message = "Invalid embedding type";
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case kTensorArray: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            const auto unit_embedding_dim = embedding_info->Dimension();
            Vector<Vector<std::string_view>> ele_str_views = SplitTensorArrayElement(sv, unit_embedding_dim);
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
            SizeT dst_off = index;
            switch (embedding_info->Type()) {
                case kElemBit: {
                    AppendTensorArray<BooleanT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemUInt8: {
                    AppendTensorArray<u8>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt8: {
                    AppendTensorArray<TinyIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt16: {
                    AppendTensorArray<SmallIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt32: {
                    AppendTensorArray<IntegerT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInt64: {
                    AppendTensorArray<BigIntT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemFloat: {
                    AppendTensorArray<FloatT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemDouble: {
                    AppendTensorArray<DoubleT>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemFloat16: {
                    AppendTensorArray<Float16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemBFloat16: {
                    AppendTensorArray<BFloat16T>(ele_str_views, dst_off, embedding_info);
                    break;
                }
                case kElemInvalid: {
                    String error_message = "Invalid embedding type";
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case kVarchar: {
            this->AppendVarcharInner(sv, index);
            break;
        }
        case kSparse: {
            const auto *sparse_info = static_cast<SparseInfo *>(data_type_->type_info().get());
            Vector<std::string_view> ele_str_views = SplitArrayElement(sv, ',');
            switch (sparse_info->DataType()) {
                case kElemBit: {
                    AppendSparse<BooleanT>(ele_str_views, index);
                    break;
                }
                case kElemUInt8: {
                    AppendSparse<u8>(ele_str_views, index);
                    break;
                }
                case kElemInt8: {
                    AppendSparse<TinyIntT>(ele_str_views, index);
                    break;
                }
                case kElemInt16: {
                    AppendSparse<SmallIntT>(ele_str_views, index);
                    break;
                }
                case kElemInt32: {
                    AppendSparse<IntegerT>(ele_str_views, index);
                    break;
                }
                case kElemInt64: {
                    AppendSparse<BigIntT>(ele_str_views, index);
                    break;
                }
                case kElemFloat: {
                    AppendSparse<FloatT>(ele_str_views, index);
                    break;
                }
                case kElemDouble: {
                    AppendSparse<DoubleT>(ele_str_views, index);
                    break;
                }
                case kElemFloat16: {
                    AppendSparse<Float16T>(ele_str_views, index);
                    break;
                }
                case kElemBFloat16: {
                    AppendSparse<BFloat16T>(ele_str_views, index);
                    break;
                }
                case kElemInvalid: {
                    String error_message = "Invalid sparse type";
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
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
        SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast, expr, *data_type());
        SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(cast_expr);
        SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(data_type());
        output_column_vector->Initialize(ColumnVectorType::kConstant, 1);
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);
        evaluator.Execute(cast_expr, expr_state, output_column_vector);
        AppendWith(*output_column_vector, 0, 1);
    }
}

void ColumnVector::AppendWith(const ColumnVector &other, SizeT from, SizeT count) {
    if (count == 0) {
        return;
    }

    if (*this->data_type_ != *other.data_type_) {
        String error_message =
            fmt::format("Attempt to append column vector{} to column vector{}", other.data_type_->ToString(), data_type_->ToString());
        UnrecoverableError(error_message);
    }

    if (this->tail_index_ + count > this->capacity_) {
        String error_message =
            fmt::format("Attempt to append {} rows data to {} rows data, which exceeds {} limit.", count, this->tail_index_, this->capacity_);
        UnrecoverableError(error_message);
    }

    switch (data_type_->type()) {
        case kBoolean: {
            CopyValue<BooleanT>(*this, other, from, count);
            break;
        }
        case kTinyInt: {
            CopyValue<TinyIntT>(*this, other, from, count);
            break;
        }
        case kSmallInt: {
            CopyValue<SmallIntT>(*this, other, from, count);
            break;
        }
        case kInteger: {
            CopyValue<IntegerT>(*this, other, from, count);
            break;
        }
        case kBigInt: {
            CopyValue<BigIntT>(*this, other, from, count);
            break;
        }
        case kHugeInt: {
            CopyValue<HugeIntT>(*this, other, from, count);
            break;
        }
        case kFloat: {
            CopyValue<FloatT>(*this, other, from, count);
            break;
        }
        case kDouble: {
            CopyValue<DoubleT>(*this, other, from, count);
            break;
        }
        case kFloat16: {
            CopyValue<Float16T>(*this, other, from, count);
            break;
        }
        case kBFloat16: {
            CopyValue<BFloat16T>(*this, other, from, count);
            break;
        }
        case kDecimal: {
            CopyValue<DecimalT>(*this, other, from, count);
            break;
        }
        case kVarchar: {
            // Copy string
            auto *base_src_ptr = (VarcharT *)(other.data_ptr_);
            VarcharT *base_dst_ptr = &((VarcharT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                VarcharT &src_ref = base_src_ptr[from + idx];
                VarcharT &dst_ref = base_dst_ptr[idx];
                CopyVarchar(dst_ref, buffer_.get(), src_ref, other.buffer_.get());
            }
            break;
        }
        case kTensor: {
            auto *base_src_ptr = (TensorT *)(other.data_ptr_);
            TensorT *base_dst_ptr = ((TensorT *)(data_ptr_)) + this->tail_index_;
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            for (SizeT idx = 0; idx < count; ++idx) {
                const TensorT &src_ref = base_src_ptr[from + idx];
                TensorT &dst_ref = base_dst_ptr[idx];
                CopyTensor(dst_ref, buffer_.get(), src_ref, other.buffer_.get(), embedding_info);
            }
            break;
        }
        case kTensorArray: {
            auto *base_src_ptr = (TensorArrayT *)(other.data_ptr_);
            TensorArrayT *base_dst_ptr = ((TensorArrayT *)(data_ptr_)) + this->tail_index_;
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
            for (SizeT idx = 0; idx < count; ++idx) {
                const TensorArrayT &src_ref = base_src_ptr[from + idx];
                TensorArrayT &dst_ref = base_dst_ptr[idx];
                CopyTensorArray(dst_ref, buffer_.get(), src_ref, other.buffer_.get(), embedding_info);
            }
            break;
        }
        case kSparse: {
            const auto *base_src_ptr = reinterpret_cast<const SparseT *>(other.data_ptr_);
            auto *base_dst_ptr = reinterpret_cast<SparseT *>(data_ptr_) + this->tail_index_;
            const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
            for (SizeT idx = 0; idx < count; ++idx) {
                const SparseT &src_sparse = base_src_ptr[from + idx];
                SparseT &dst_sparse = base_dst_ptr[idx];
                CopySparse(dst_sparse, buffer_.get(), src_sparse, other.buffer_.get(), sparse_info);
            }
            break;
        }
        case kDate: {
            CopyValue<DateT>(*this, other, from, count);
            break;
        }
        case kTime: {
            CopyValue<TimeT>(*this, other, from, count);
            break;
        }
        case kDateTime: {
            CopyValue<DateTimeT>(*this, other, from, count);
            break;
        }
        case kTimestamp: {
            CopyValue<TimestampT>(*this, other, from, count);
            break;
        }
        case kInterval: {
            CopyValue<IntervalT>(*this, other, from, count);
            break;
        }
        case kArray: {
            String error_message = "Array copy";
            UnrecoverableError(error_message);
            break;
        }
        case kTuple: {
            String error_message = "Shouldn't store tuple directly, a tuple is flatten as many columns";
            UnrecoverableError(error_message);
            break;
        }
        case kPoint: {
            CopyValue<PointT>(*this, other, from, count);
            break;
        }
        case kLine: {
            CopyValue<LineT>(*this, other, from, count);
            break;
        }
        case kLineSeg: {
            CopyValue<LineSegT>(*this, other, from, count);
            break;
        }
        case kBox: {
            CopyValue<BoxT>(*this, other, from, count);
            break;
        }
            //        case kPath: {
            //        }
        case kCircle: {
            CopyValue<CircleT>(*this, other, from, count);
            break;
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            CopyValue<UuidT>(*this, other, from, count);
            break;
        }
            //        case kBlob: {
            //        }
        case kEmbedding: {
            //            auto *base_src_ptr = (EmbeddingT *)(other.data_ptr_);
            auto *base_src_ptr = other.data_ptr_;
            ptr_t base_dst_ptr = data_ptr_ + this->tail_index_ * data_type_->Size();
            for (SizeT idx = 0; idx < count; ++idx) {
                ptr_t src_ptr = base_src_ptr + (from + idx) * data_type_->Size();
                ptr_t dst_ptr = base_dst_ptr + idx * data_type_->Size();
                std::memcpy(dst_ptr, src_ptr, data_type_->Size());
            }
            break;
        }
        case kRowID: {
            CopyValue<RowID>(*this, other, from, count);
            break;
        }
        case kMixed: {
            CopyValue<MixedT>(*this, other, from, count);
            break;
        }
        default: {
            String error_message = "Attempt to access an unaccepted type";
            UnrecoverableError(error_message);
            // Null/Missing/Invalid
        }
    }
    this->tail_index_ += count;
}

SizeT ColumnVector::AppendWith(RowID from, SizeT row_count) {
    if (data_type_->type() != LogicalType::kRowID) {
        String error_message = fmt::format("Only RowID column vector supports this method, current data type: {}", data_type_->ToString());
        UnrecoverableError(error_message);
    }
    if (row_count == 0) {
        return 0;
    }

    SizeT appended_rows = row_count;
    if (tail_index_ + row_count > capacity_) {
        // attempt to append data rows more than the capacity;
        appended_rows = capacity_ - tail_index_;
    }

    ptr_t dst_ptr = data_ptr_ + tail_index_ * data_type_size_;
    for (SizeT i = 0; i < row_count; i++) {
        *(RowID *)dst_ptr = RowID(from.segment_id_, from.segment_offset_ + i);
        dst_ptr += data_type_size_;
    }
    this->tail_index_ += appended_rows;
    return appended_rows;
}

void ColumnVector::ShallowCopy(const ColumnVector &other) {
    if (*this->data_type_ != *other.data_type_) {
        String error_message =
            fmt::format("Attempt to shallow copy: {} column vector to: {}", other.data_type_->ToString(), this->data_type_->ToString());
        UnrecoverableError(error_message);
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
    this->tail_index_ = other.tail_index_;
}

void ColumnVector::Reset() {
    // 1. Vector type is reset to invalid.
    vector_type_ = ColumnVectorType::kInvalid;

    // 2. Date type won't be changed.

    // 3. Since data type isn't change, data_type_size_ won't be changed, either.

    // 4. For trivial data type, the VectorBuffer will not be reset.
    // But for non-trivial data type, the heap memory manage need to be reset
    if (data_type_->type() == LogicalType::kMixed) {
        // Current solution:
        // Tuple/Array/Long String will use heap memory which isn't managed by ColumnVector.
        // This part of memory should managed by ColumnVector, but it isn't now.
        // So, when ColumnVector is destructed, this part need to free here.
        // TODO: we are going to manage the nested object in ColumnVector.
        for (SizeT idx = 0; idx < tail_index_; ++idx) {
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
    tail_index_ = 0;

    // 8. Reset initialized flag
    initialized = false;
}

bool ColumnVector::operator==(const ColumnVector &other) const {
    // initialized, data_type_, vector_type_, data_ptr_[0..tail_index_ * data_type_size_]
    if (!this->initialized && !other.initialized)
        return true;
    if (!this->initialized || !other.initialized || this->data_type_.get() == nullptr || other.data_type_.get() == nullptr ||
        (*this->data_type_).operator!=(*other.data_type_) || this->data_type_size_ != other.data_type_size_ ||
        this->vector_type_ != other.vector_type_ || this->tail_index_ != other.tail_index_ || *this->nulls_ptr_ != *other.nulls_ptr_)
        return false;
    if (data_type_->type() == kVarchar) {
        for (SizeT i = 0; i < this->tail_index_; i++) {
            Span<const char> data1 = this->GetVarchar(i);
            Span<const char> data2 = other.GetVarchar(i);
            if (data1.size() != data2.size() || std::strncmp(data1.data(), data2.data(), data1.size())) {
                return false;
            }
        }
    } else if (data_type_->type() == LogicalType::kBoolean) {
        return other.data_type_->type() == LogicalType::kBoolean &&
               VectorBuffer::CompactBitIsSame(this->buffer_, this->tail_index_, other.buffer_, other.tail_index_);
    } else {
        return 0 == std::memcmp(this->data_ptr_, other.data_ptr_, this->tail_index_ * this->data_type_size_);
    }
    return true;
}

i32 ColumnVector::GetSizeInBytes() const {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (vector_type_ != ColumnVectorType::kFlat && vector_type_ != ColumnVectorType::kConstant && vector_type_ != ColumnVectorType::kCompactBit) {
        String error_message = fmt::format("Not supported vector_type {}", int(vector_type_));
        UnrecoverableError(error_message);
    }
    i32 size = this->data_type_->GetSizeInBytes() + sizeof(ColumnVectorType);
    size += sizeof(i32);
    if (vector_type_ == ColumnVectorType::kCompactBit) {
        size += (this->tail_index_ + 7) / 8;
    } else {
        size += this->tail_index_ * this->data_type_size_;
    }
    size += buffer_->TotalSize(data_type_.get());
    size += this->nulls_ptr_->GetSizeInBytes();
    return size;
}

void ColumnVector::WriteAdv(char *&ptr) const {
    if (!initialized) {
        String error_message = "Column vector isn't initialized.";
        UnrecoverableError(error_message);
    }
    if (vector_type_ != ColumnVectorType::kFlat && vector_type_ != ColumnVectorType::kConstant && vector_type_ != ColumnVectorType::kCompactBit) {
        String error_message = fmt::format("Not supported vector_type {}", int(vector_type_));
        UnrecoverableError(error_message);
    }

    if (data_type_->type() == LogicalType::kHugeInt) {
        String error_message = "Attempt to serialize huge integer type";
        UnrecoverableError(error_message);
    }
    this->data_type_->WriteAdv(ptr);
    WriteBufAdv<ColumnVectorType>(ptr, this->vector_type_);
    // write fixed part
    WriteBufAdv<i32>(ptr, tail_index_);
    if (vector_type_ == ColumnVectorType::kCompactBit) {
        SizeT byte_size = (this->tail_index_ + 7) / 8;
        std::memcpy(ptr, this->data_ptr_, byte_size);
        ptr += byte_size;
    } else {
        std::memcpy(ptr, this->data_ptr_, this->tail_index_ * this->data_type_size_);
        ptr += this->tail_index_ * this->data_type_size_;
    }
    // write variable part
    buffer_->WriteAdv(ptr, data_type_.get());
    this->nulls_ptr_->WriteAdv(ptr);
    return;
}

SharedPtr<ColumnVector> ColumnVector::ReadAdv(char *&ptr, i32 maxbytes) {
    const char *ptr_end = ptr + maxbytes;
    SharedPtr<DataType> data_type = DataType::ReadAdv(ptr, maxbytes);
    ColumnVectorType vector_type = ReadBufAdv<ColumnVectorType>(ptr);
    SharedPtr<ColumnVector> column_vector = ColumnVector::Make(data_type);
    column_vector->Initialize(vector_type, DEFAULT_VECTOR_SIZE);
    // read fixed part
    i32 tail_index = ReadBufAdv<i32>(ptr);
    column_vector->tail_index_ = tail_index;
    if (vector_type == ColumnVectorType::kCompactBit) {
        SizeT byte_size = (tail_index + 7) / 8;
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
        String error_message = "ptr goes out of range when reading ColumnVector";
        UnrecoverableError(error_message);
    }
    column_vector->nulls_ptr_ = Bitmask::ReadAdv(ptr, maxbytes);
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        String error_message = "ptr goes out of range when reading ColumnVector";
        UnrecoverableError(error_message);
    }
    return column_vector;
}

//////////////////////////////tensor////////////////////////////////////

void ColumnVector::SetTensor(TensorT &dest_tensor, VectorBuffer *dest_buffer, Span<const char> data, const EmbeddingInfo *embedding_info) {
    SizeT unit_embedding_bytes = embedding_info->Size();
    if (data.size() % unit_embedding_bytes != 0) {
        String error_message = fmt::format("Data size {} is not a multiple of embedding size {}", data.size(), unit_embedding_bytes);
        UnrecoverableError(error_message);
    }
    SizeT embedding_num = data.size() / unit_embedding_bytes;
    dest_tensor.embedding_num_ = embedding_num;
    dest_tensor.chunk_offset_ = dest_buffer->AppendTensorRaw(data.data(), data.size());
}

Pair<Span<const char>, SizeT> ColumnVector::GetTensor(const TensorT &src_tensor, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info) {
    SizeT tensor_bytes = src_tensor.embedding_num_ * embedding_info->Size();
    const char *raw_data = src_buffer->GetTensorRaw(src_tensor.chunk_offset_, tensor_bytes);
    return {Span<const char>(raw_data, tensor_bytes), src_tensor.embedding_num_};
}

void ColumnVector::SetTensorArrayMeta(TensorArrayT &dest_tensor_array, VectorBuffer *dest_buffer, Span<const TensorT> tensors) {
    dest_tensor_array.tensor_num_ = tensors.size();
    dest_tensor_array.chunk_offset_ = dest_buffer->AppendTensorArrayMeta(tensors);
}

Span<const TensorT> ColumnVector::GetTensorArrayMeta(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer) {
    SizeT array_bytes = src_tensor_array.tensor_num_ * sizeof(TensorT);
    const TensorT *data = src_buffer->GetTensorArrayMeta(src_tensor_array.chunk_offset_, array_bytes);
    return {data, src_tensor_array.tensor_num_};
}

void ColumnVector::SetTensorArray(TensorArrayT &dest_tensor_array,
                                 VectorBuffer *dest_buffer,
                                 const Vector<Span<const char>> &data,
                                 const EmbeddingInfo *embedding_info) {
    SizeT tensor_num = data.size();
    Vector<TensorT> tensor_array_meta(tensor_num);
    for (SizeT i = 0; i < tensor_num; ++i) {
        const auto &raw_data = data[i];
        TensorT &target_tensor = tensor_array_meta[i];
        ColumnVector::SetTensor(target_tensor, dest_buffer, raw_data, embedding_info);
    }
    dest_tensor_array.tensor_num_ = tensor_num;
    dest_tensor_array.chunk_offset_ = dest_buffer->AppendTensorArrayMeta(tensor_array_meta);
}

Vector<Pair<Span<const char>, SizeT>>
ColumnVector::GetTensorArray(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info) {
    SizeT array_bytes = src_tensor_array.tensor_num_ * sizeof(TensorT);
    const TensorT *array_meta = src_buffer->GetTensorArrayMeta(src_tensor_array.chunk_offset_, array_bytes);
    Vector<Pair<Span<const char>, SizeT>> res;
    res.reserve(src_tensor_array.tensor_num_);
    for (SizeT i = 0; i < src_tensor_array.tensor_num_; ++i) {
        const TensorT &tensor = array_meta[i];
        auto [raw_data, embedding_num] = ColumnVector::GetTensor(tensor, src_buffer, embedding_info);
        res.emplace_back(raw_data, tensor.embedding_num_);
    }
    return res;
}

Pair<Span<const char>, SizeT> ColumnVector::GetTensorRaw(SizeT idx) const {
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    const TensorT &src_tensor = reinterpret_cast<const TensorT *>(data_ptr_)[idx];
    return ColumnVector::GetTensor(src_tensor, buffer_.get(), embedding_info);
}

Vector<Pair<Span<const char>, SizeT>> ColumnVector::GetTensorArrayRaw(SizeT idx) const {
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    const TensorArrayT &src_tensor_array = reinterpret_cast<const TensorArrayT *>(data_ptr_)[idx];
    return ColumnVector::GetTensorArray(src_tensor_array, buffer_.get(), embedding_info);
}

//////////////////////////////tensor end////////////////////////////////////

void ColumnVector::AppendSparseRaw(const char *raw_data_ptr, const char *raw_index_ptr, SizeT nnz, SizeT dst_off) {
    auto &sparse = reinterpret_cast<SparseT *>(data_ptr_)[dst_off];
    sparse.nnz_ = nnz;
    if (nnz == 0) {
        sparse.file_offset_ = -1;
        return;
    }
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
    sparse.file_offset_ = buffer_->AppendSparseRaw(raw_data_ptr, raw_index_ptr, nnz, sparse_info);
}

Tuple<Span<const char>, Span<const char>, SizeT> ColumnVector::GetSparseRaw(SizeT index) const {
    const auto &sparse = reinterpret_cast<const SparseT *>(data_ptr_)[index];
    SizeT nnz = sparse.nnz_;
    if (nnz == 0) {
        return {Span<const char>(), Span<const char>(), 0};
    }
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
    SizeT data_size = sparse_info->DataSize(nnz);
    SizeT idx_size = sparse_info->IndiceSize(nnz);
    auto [raw_data_ptr, raw_idx_ptr] = buffer_->GetSparseRaw(sparse.file_offset_, nnz, sparse_info);
    return {Span<const char>(raw_data_ptr, data_size), Span<const char>(raw_idx_ptr, idx_size), nnz};
}

void ColumnVector::AppendVarcharInner(Span<const char> data, VarcharT &varchar) {
    varchar.length_ = data.size();
    if (varchar.IsInlined()) {
        std::memcpy(varchar.short_.data_, data.data(), data.size());
    } else {
        varchar.vector_.file_offset_ = buffer_->AppendVarchar(data.data(), data.size());
    }
}

void ColumnVector::AppendVarcharInner(Span<const char> data, SizeT dst_off) {
    auto &varchar = reinterpret_cast<VarcharT *>(data_ptr_)[dst_off];
    AppendVarcharInner(data, varchar);
}

void ColumnVector::AppendVarchar(Span<const char> data) {
    SizeT dst_off = tail_index_++;
    AppendVarcharInner(data, dst_off);
}

Span<const char> ColumnVector::GetVarcharInner(const VarcharT &varchar) const {
    i32 length = varchar.length_;
    const char *data = nullptr;
    if (varchar.IsInlined()) {
        data = varchar.short_.data_;
    } else {
        data = buffer_->GetVarchar(varchar.vector_.file_offset_, length);
    }
    return {data, static_cast<SizeT>(length)};
}

Span<const char> ColumnVector::GetVarchar(SizeT index) const {
    const auto &varchar = reinterpret_cast<const VarcharT *>(data_ptr_)[index];
    return GetVarcharInner(varchar);
}

void CopyVarchar(VarcharT &dst_ref, VectorBuffer *dst_vec_buffer, const VarcharT &src_ref, const VectorBuffer *src_vec_buffer) {
    const u32 varchar_len = src_ref.length_;
    dst_ref.is_value_ = 0;
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
    Vector<Span<const char>> data_vec;
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

Vector<std::string_view> SplitTensorElement(std::string_view data, char delimiter, const u32 unit_embedding_dim) {
    SizeT data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Status status = Status::ImportFileFormatError("Tensor data must be surrounded by [ and ]");
        RecoverableError(status);
    }
    bool have_child_embedding = false;
    for (SizeT i = 1; i < data_size - 1; ++i) {
        if (data[i] == '[') {
            have_child_embedding = true;
            break;
        }
    }
    if (!have_child_embedding) {
        return SplitArrayElement(data, ',');
    }
    std::string_view child_data = data.substr(1, data_size - 2);
    Vector<std::string_view> ret;
    size_t bg_id = 0;
    while (true) {
        const auto next_bg_id = child_data.find('[', bg_id);
        if (next_bg_id == std::string_view::npos) {
            break;
        }
        const auto ed_id = child_data.find(']', next_bg_id);
        if (ed_id == std::string_view::npos) {
            Status status = Status::ImportFileFormatError("Tensor data member embedding must be surrounded by [ and ]");
            RecoverableError(status);
        }
        if (const auto check_inner_valid = child_data.find('[', next_bg_id + 1); check_inner_valid < ed_id) {
            Status status = Status::ImportFileFormatError("Tensor data format invalid: mismatch of inner '[', ']'.");
            RecoverableError(status);
        }
        Vector<std::string_view> sub_result = SplitArrayElement(child_data.substr(next_bg_id, ed_id - next_bg_id + 1), ',');
        if (sub_result.size() != unit_embedding_dim) {
            Status status = Status::ImportFileFormatError("Tensor data member embedding size must be equal to unit embedding dimension.");
            RecoverableError(status);
        }
        ret.insert(ret.end(), sub_result.begin(), sub_result.end());
        bg_id = ed_id + 1;
    }
    return ret;
}

} // namespace infinity
