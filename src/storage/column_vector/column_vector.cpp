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

#include "type/complex/varchar.h"
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

import buffer_manager;
import status;
import logical_type;
import embedding_info;

import block_column_entry;

namespace infinity {

VectorBufferType ColumnVector::InitializeHelper(ColumnVectorType vector_type, SizeT capacity) {
    if (initialized) {
        UnrecoverableError("Column vector is already initialized.");
    }
    initialized = true;
    if (data_type_->type() == LogicalType::kInvalid) {
        UnrecoverableError("Data type isn't assigned.");
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
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;
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

void ColumnVector::Initialize(ColumnVectorType vector_type, SizeT capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(8);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(capacity_);
        }
        data_ptr_ = buffer_->GetDataMut();
    } else {
        // Initialize after reset will come to this branch
        if (vector_buffer_type == VectorBufferType::kHeap) {
            if (buffer_->fix_heap_mgr_.get() != nullptr) {
                UnrecoverableError("Vector heap should be null.");
            }
            buffer_->fix_heap_mgr_ = MakeUnique<FixHeapManager>();
        }
    }
}

void ColumnVector::Initialize(BufferManager *buffer_mgr,
                              BlockColumnEntry *block_column_entry,
                              SizeT current_row_count,
                              ColumnVectorType vector_type,
                              SizeT capacity) {
    VectorBufferType vector_buffer_type = InitializeHelper(vector_type, capacity);

    if (buffer_.get() != nullptr) {
        UnrecoverableError("Column vector is already initialized.");
    }

    if (vector_type_ == ColumnVectorType::kConstant) {
        buffer_ = VectorBuffer::Make(buffer_mgr, block_column_entry, data_type_size_, 1, vector_buffer_type);
        nulls_ptr_ = Bitmask::Make(8);
    } else {
        buffer_ = VectorBuffer::Make(buffer_mgr, block_column_entry, data_type_size_, capacity_, vector_buffer_type);
        nulls_ptr_ = Bitmask::Make(capacity_); // TODO: version file is not managed by buffer_manager now.
    }
    data_ptr_ = buffer_->GetDataMut();
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
            case kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
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
            case kNull: {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case kMissing: {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case kInvalid: {
                UnrecoverableError("Invalid data type");
            }
        }
    }
}

void ColumnVector::Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx) {
    if (end_idx <= start_idx) {
        UnrecoverableError("End index should larger than start index.");
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
            case kDecimal: {
                CopyFrom<DecimalT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
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
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case kNull: {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case kMissing: {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            case kInvalid: {
                UnrecoverableError("Invalid data type");
            }
        }
    }
}

void ColumnVector::CopyRow(const ColumnVector &other, SizeT dst_idx, SizeT src_idx) {
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
        tail_index_ = 1;
    } else {
        if (dst_idx >= tail_index_) {
            UnrecoverableError("Attempting to access invalid position of target column vector");
        }
    }
    if (other.vector_type_ == ColumnVectorType::kConstant) {
        // Copy from constant vector, only first row have value.
        src_idx = 0;
    }

    if (src_idx >= other.tail_index_) {
        UnrecoverableError("Attempting to access invalid position of source column vector");
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
        case kDecimal: {
            CopyRowFrom<DecimalT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
        case kVarchar: {
            CopyRowFrom<VarcharT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
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
        case kNull: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kMissing: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kInvalid: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
    }
}

String ColumnVector::ToString(SizeT row_index) const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
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
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kFloat: {
            return std::to_string(((FloatT *)data_ptr_)[row_index]);
        }
        case kDouble: {
            return std::to_string(((DoubleT *)data_ptr_)[row_index]);
        }
        case kDecimal: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kVarchar: {
            VarcharT &varchar_ref = ((VarcharT *)data_ptr_)[row_index];
            if (varchar_ref.IsInlined()) {
                return {varchar_ref.short_.data_, varchar_ref.length_};
            } else {
                // Must be vector type
                if (varchar_ref.IsValue()) {
                    UnrecoverableError("Must be vector type of varchar, here");
                }
                String result_str;
                result_str.resize(varchar_ref.length_);
                buffer_->fix_heap_mgr_->ReadFromHeap(result_str.data(),
                                                     varchar_ref.vector_.chunk_id_,
                                                     varchar_ref.vector_.chunk_offset_,
                                                     varchar_ref.length_);
                return result_str;
            }
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
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kArray: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kTuple: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kPoint: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kLine: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kLineSeg: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        case kBox: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
            //        case kPath: {
            //        }
            //        case kPolygon: {
            //        }
        case kCircle: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
            //        case kBitmap: {
            //        }
        case kUuid: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
            //        case kBlob: {
            //        }
        case kEmbedding: {
            //            RecoverableError(Status::NotSupport("Not implemented"));
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                RecoverableError(Status::NotSupport("Not implemented"));
            }
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            EmbeddingT embedding_element(nullptr, false);
            embedding_element.ptr = (data_ptr_ + row_index * data_type_->type_info()->Size());
            String embedding_str = EmbeddingT::Embedding2String(embedding_element, embedding_info->Type(), embedding_info->Dimension());
            embedding_element.SetNull();
            return embedding_str;
        }
        case kRowID: {
            return (((RowID *)data_ptr_)[row_index]).ToString();
        }
        case kMixed: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
        default: {
            UnrecoverableError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    return String();
}

Value ColumnVector::GetValue(SizeT index) const {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (index >= tail_index_) {
        UnrecoverableError(fmt::format("Attempt to access an invalid index of column vector: {}", std::to_string(index)));
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
        case kDecimal: {
            return Value::MakeDecimal(((DecimalT *)data_ptr_)[index], data_type_->type_info());
        }
        case kVarchar: {
            VarcharT &varchar = ((VarcharT *)data_ptr_)[index];
            if (varchar.IsInlined()) {
                return Value::MakeVarchar(((VarcharT *)data_ptr_)[index]);
            } else {
                char *varchar_ptr = new char[varchar.length_ + 1]{0};
                this->buffer_->fix_heap_mgr_->ReadFromHeap(varchar_ptr, varchar.vector_.chunk_id_, varchar.vector_.chunk_offset_, varchar.length_);
                Value value = Value::MakeVarchar(varchar_ptr, varchar.length_);
                delete[] varchar_ptr;
                return value;
            }
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
            UnrecoverableError("Shouldn't access tuple directly, a tuple is flatten as many columns");
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
        case kRowID: {
            return Value::MakeRow(((RowID *)data_ptr_)[index]);
        }
        default: {
            UnrecoverableError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    return Value::MakeInvalid();
}

void ColumnVector::SetValue(SizeT index, const Value &value) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (index > tail_index_) {
        UnrecoverableError(
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index_),
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
            UnrecoverableError("Shouldn't store tuple directly, a tuple is flatten as many columns");
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
            // Copy string
            const String &src_str = value.GetVarchar();
            u64 varchar_len = src_str.size();
            VarcharT &target_ref = ((VarcharT *)data_ptr_)[index];
            target_ref.is_value_ = false;
            target_ref.length_ = varchar_len;
            if (varchar_len <= VARCHAR_INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                std::memcpy(target_ref.short_.data_, src_str.c_str(), varchar_len);
            } else {
                std::memcpy(target_ref.vector_.prefix_, src_str.c_str(), VARCHAR_PREFIX_LEN);
                auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_str.c_str(), varchar_len);
                target_ref.vector_.chunk_id_ = chunk_id;
                target_ref.vector_.chunk_offset_ = chunk_offset;
            }
            break;
        }
        case kEmbedding: {
            const_ptr_t src_ptr;
            SizeT src_size;
            std::tie(src_ptr, src_size) = value.GetEmbedding();
            if (src_size != data_type_->Size()) {
                UnrecoverableError(fmt::format("Attempt to store a value with different size than column vector type, want {}, got {}",
                                               data_type_->Size(),
                                               src_size));
            }
            ptr_t dst_ptr = data_ptr_ + index * data_type_->Size();
            std::memcpy(dst_ptr, src_ptr, src_size);
            break;
        }
        default: {
            UnrecoverableError("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::Finalize(SizeT index) {
    if (index > capacity_) {
        UnrecoverableError(fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));
    }
    tail_index_ = index;
}

void ColumnVector::SetByRawPtr(SizeT index, const_ptr_t raw_ptr) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (index > capacity_) {
        UnrecoverableError(fmt::format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));
    }
    if (index > tail_index_) {
        UnrecoverableError(
            fmt::format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                        std::to_string(index),
                        std::to_string(tail_index_),
                        std::to_string(capacity_)));
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
        case kDouble: {
            ((DoubleT *)data_ptr_)[index] = *(DoubleT *)(raw_ptr);
            break;
        }
        case kDecimal: {
            ((DecimalT *)data_ptr_)[index] = *(DecimalT *)(raw_ptr);
            break;
        }
        case kVarchar: {
            // Copy string
            const VarcharT &src_ref = *(VarcharT *)(raw_ptr);
            if (!src_ref.IsValue()) {
                UnrecoverableError("Only can set value not column vector here.");
            }

            u64 varchar_len = src_ref.length_;
            VarcharT &target_ref = ((VarcharT *)data_ptr_)[index];
            target_ref.is_value_ = false;
            target_ref.length_ = varchar_len;
            if (src_ref.IsInlined()) {
                // Only prefix is enough to contain all string data.
                std::memcpy(target_ref.short_.data_, src_ref.short_.data_, varchar_len);
            } else {
                std::memcpy(target_ref.vector_.prefix_, src_ref.value_.prefix_, VARCHAR_PREFIX_LEN);
                auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_ref.value_.ptr_, varchar_len);
                target_ref.vector_.chunk_id_ = chunk_id;
                target_ref.vector_.chunk_offset_ = chunk_offset;
            }
            break;
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
            UnrecoverableError("Shouldn't store tuple directly, a tuple is flatten as many columns");
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
            UnrecoverableError("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::SetByPtr(SizeT index, const_ptr_t value_ptr) {
    // We assume the value_ptr point to the same type data.
    if (data_type_->type() == LogicalType::kEmbedding) {
        auto *embedding_ptr = (EmbeddingT *)(value_ptr);
        SetByRawPtr(index, embedding_ptr->ptr);
    } else {
        SetByRawPtr(index, value_ptr);
    }
}

void ColumnVector::AppendByPtr(const_ptr_t value_ptr) {
    if (!initialized) {
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ == ColumnVectorType::kConstant) {
        if (tail_index_ >= 1) {
            UnrecoverableError("Constant column vector will only have 1 value.");
        }
    }
    if (tail_index_ >= capacity_) {
        UnrecoverableError(fmt::format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
    }
    if (data_type_->type() == LogicalType::kEmbedding) {
        SetByRawPtr(tail_index_++, value_ptr);
    } else {
        SetByPtr(tail_index_++, value_ptr);
    }
}

namespace {
Vector<std::string_view> SplitArrayElement(std::string_view data, char delimiter) {
    SizeT data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        RecoverableError(Status::ImportFileFormatError("Embedding data must be surrounded by [ and ]"));
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
        i++;
    }
    return ret;
}

} // namespace

void ColumnVector::AppendByStringView(std::string_view sv, char delimiter) {
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
            Vector<std::string_view> ele_str_views = SplitArrayElement(sv, delimiter);
            if (embedding_info->Dimension() < ele_str_views.size()) {
                RecoverableError(Status::ImportFileFormatError("Embedding data size exceeds dimension."));
            }
            SizeT dst_off = index * data_type_->Size();
            switch (embedding_info->Type()) {
                case kElemBit: {
                    RecoverableError(Status::NotSupport("Not implemented"));
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
                default: {
                    UnrecoverableError("Invalid embedding type");
                }
            }
            break;
        }
        case kVarchar: {
            auto &varchar = (reinterpret_cast<VarcharT *>(data_ptr_))[index];
            varchar.is_value_ = false;
            varchar.length_ = sv.size();
            if (sv.size() <= VARCHAR_INLINE_LENGTH) {
                std::memcpy(varchar.short_.data_, sv.data(), sv.size());
            } else {
                std::memcpy(varchar.vector_.prefix_, sv.data(), VARCHAR_PREFIX_LEN);
                auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(sv.data(), sv.size());
                varchar.vector_.chunk_id_ = chunk_id;
                varchar.vector_.chunk_offset_ = chunk_offset;
            }
            break;
        }
        default: {
            RecoverableError(Status::NotSupport("Not implemented"));
        }
    }
}

void ColumnVector::AppendWith(const ColumnVector &other, SizeT from, SizeT count) {
    if (count == 0) {
        return;
    }

    if (*this->data_type_ != *other.data_type_) {
        UnrecoverableError(fmt::format("Attempt to append column vector{} to column vector{}", other.data_type_->ToString(), data_type_->ToString()));
    }

    if (this->tail_index_ + count > this->capacity_) {
        UnrecoverableError(
            fmt::format("Attempt to append {} rows data to {} rows data, which exceeds {} limit.", count, this->tail_index_, this->capacity_));
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
                dst_ref.is_value_ = src_ref.is_value_;
                dst_ref.length_ = src_ref.length_;
                dst_ref.is_value_ = src_ref.is_value_;
                if (src_ref.IsInlined()) {
                    std::memcpy(&dst_ref.short_, &src_ref.short_, src_ref.length_);
                } else {
                    // Assume the source must be column vector type.
                    std::memcpy(dst_ref.vector_.prefix_, src_ref.vector_.prefix_, VARCHAR_PREFIX_LEN);
                    auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(other.buffer_->fix_heap_mgr_.get(),
                                                                                               src_ref.vector_.chunk_id_,
                                                                                               src_ref.vector_.chunk_offset_,
                                                                                               src_ref.length_);
                    dst_ref.vector_.chunk_id_ = chunk_id;
                    dst_ref.vector_.chunk_offset_ = chunk_offset;
                }
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
            UnrecoverableError("Array copy");
            break;
        }
        case kTuple: {
            UnrecoverableError("Shouldn't store tuple directly, a tuple is flatten as many columns");
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
            UnrecoverableError("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    this->tail_index_ += count;
}

SizeT ColumnVector::AppendWith(RowID from, SizeT row_count) {
    if (data_type_->type() != LogicalType::kRowID) {
        UnrecoverableError(fmt::format("Only RowID column vector supports this method, current data type: {}", data_type_->ToString()));
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
        LOG_ERROR(fmt::format("{} isn't supported", data_type_->ToString()));
        UnrecoverableError(
            fmt::format("Attempt to shallow copy: {} column vector to: {}", other.data_type_->ToString(), this->data_type_->ToString()));
        ;
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
        buffer_->fix_heap_mgr_ = nullptr;
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
            const Varchar *lhs = reinterpret_cast<const Varchar *>(this->data_ptr_ + i * this->data_type_size_);
            const Varchar *rhs = reinterpret_cast<const Varchar *>(other.data_ptr_ + i * this->data_type_size_);
            if (lhs->length_ != rhs->length_) {
                return false;
            }
            if (lhs->IsInlined()) {
                if (0 != memcmp(lhs->short_.data_, rhs->short_.data_, lhs->length_))
                    return false;
            } else {
                Vector<char> lhs_str(lhs->length_);
                Vector<char> rhs_str(rhs->length_);
                this->buffer_->fix_heap_mgr_->ReadFromHeap(lhs_str.data(), lhs->vector_.chunk_id_, lhs->vector_.chunk_offset_, lhs->length_);
                other.buffer_->fix_heap_mgr_->ReadFromHeap(rhs_str.data(), rhs->vector_.chunk_id_, rhs->vector_.chunk_offset_, rhs->length_);
                if (0 != memcmp(lhs_str.data(), rhs_str.data(), lhs->length_))
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
        UnrecoverableError("Column vector isn't initialized.");
    }
    if (vector_type_ != ColumnVectorType::kFlat && vector_type_ != ColumnVectorType::kConstant && vector_type_ != ColumnVectorType::kCompactBit) {
        UnrecoverableError(fmt::format("Not supported vector_type {}", int(vector_type_)));
    }
    i32 size = this->data_type_->GetSizeInBytes() + sizeof(ColumnVectorType);
    size += sizeof(i32);
    if (vector_type_ == ColumnVectorType::kCompactBit) {
        size += (this->tail_index_ + 7) / 8;
    } else {
        size += this->tail_index_ * this->data_type_size_;
    }
    if (data_type_->type() == kVarchar) {
        size += sizeof(i32) + buffer_->fix_heap_mgr_->total_size();
    }
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
    if (data_type_->type() == kVarchar) {
        i32 heap_len = buffer_->fix_heap_mgr_->total_size();
        WriteBufAdv<i32>(ptr, heap_len);
        buffer_->fix_heap_mgr_->ReadFromHeap(ptr, 0, 0, heap_len);
        ptr += heap_len;
    }
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
    if (data_type->type() == kVarchar) {
        i32 heap_len = ReadBufAdv<i32>(ptr);
        if (heap_len > 0) {
            column_vector->buffer_->fix_heap_mgr_->AppendToHeap(ptr, heap_len);
            ptr += heap_len;
        }
    }

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

} // namespace infinity
