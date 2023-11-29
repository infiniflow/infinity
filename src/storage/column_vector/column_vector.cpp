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

#include <iostream>
#include <sstream>
import stl;
import selection;
import parser;

import infinity_exception;
import default_values;
import bitmask;
import vector_buffer;
import fix_heap;
import column_buffer;
import serialize;
import third_party;
import logger;
import value;
#include "type/complex/varchar.h"

module column_vector;

namespace infinity {

void ColumnVector::Initialize(const ColumnVector &other, const Selection &input_select) {
    Assert<StorageException>(!initialized, "Column vector is already initialized.");
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.");

    vector_type_ = other.vector_type_;
    data_type_size_ = data_type_->Size();

    if (vector_type_ == ColumnVectorType::kConstant) {
        // If the vector is constant, all elements are the same, so the select is meaningless.
        capacity_ = other.capacity();
    } else {
        //        capacity_ = input_select.Size();
        // Select size might not be the power of 2. Then, we use DEFAULT_VECTOR_SIZE;
        capacity_ = DEFAULT_VECTOR_SIZE;
    }

    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type_->type()) {
            //        case LogicalType::kBlob:
            //        case LogicalType::kBitmap:
            //        case LogicalType::kPolygon:
            //        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.");
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }

    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(8);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(capacity_);
        }
        data_ptr_ = buffer_->GetData();
    } else {
        // Initialize after reset will come to this branch
        if (vector_buffer_type == VectorBufferType::kHeap) {
            Assert<StorageException>(buffer_->fix_heap_mgr_.get() == nullptr, "Vector heap should be null.");
            buffer_->fix_heap_mgr_ = MakeUnique<FixHeapManager>();
        }
    }

    initialized = true;

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
                //                CopyFrom<PathT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                //                break;
                //            }
                //            case kPolygon: {
                //                CopyFrom<PolygonT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                //                break;
                //            }
            case kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
                //            case kBitmap: {
                //                CopyFrom<BitmapT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                //                break;
                //            }
            case kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                break;
            }
                //            case kBlob: {
                //                CopyFrom<BlobT>(other.buffer_.get(), this->buffer_.get(), tail_index_, input_select);
                //                break;
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
                Error<ExecutorException>("Not implemented");
            }
            case kMissing: {
                Error<ExecutorException>("Not implemented");
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type");
            }
        }
    }
}

void ColumnVector::Initialize(const ColumnVector &other, SizeT start_idx, SizeT end_idx) {
    Initialize(other.vector_type_, other, start_idx, end_idx);
}

void ColumnVector::Initialize(ColumnVectorType vector_type, SizeT capacity) {
    Assert<StorageException>(!initialized, "Column vector is already initialized.");
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.");
    Assert<StorageException>(vector_type != ColumnVectorType::kInvalid, "Attempt to initialize column vector to invalid type.");
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
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.");
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }
    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(8);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(capacity_);
        }
        data_ptr_ = buffer_->GetData();
    } else {
        // Initialize after reset will come to this branch
        if (vector_buffer_type == VectorBufferType::kHeap) {
            Assert<StorageException>(buffer_->fix_heap_mgr_.get() == nullptr, "Vector heap should be null.");
            buffer_->fix_heap_mgr_ = MakeUnique<FixHeapManager>();
        }
    }

    initialized = true;
}

void ColumnVector::Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx) {

    Assert<StorageException>(!initialized, "Column vector is already initialized.");
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.");
    Assert<StorageException>(end_idx > start_idx, "End index should larger than start index.");

    vector_type_ = vector_type;
    capacity_ = end_idx - start_idx;

    tail_index_ = 0;
    data_type_size_ = data_type_->Size();

    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type_->type()) {
            //        case LogicalType::kBlob:
            //        case LogicalType::kBitmap:
            //        case LogicalType::kPolygon:
            //        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.");
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }

    if (buffer_.get() == nullptr) {
        if (vector_type_ == ColumnVectorType::kConstant) {
            buffer_ = VectorBuffer::Make(data_type_size_, 1, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(8);
        } else {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
            nulls_ptr_ = Bitmask::Make(capacity_);
        }
        data_ptr_ = buffer_->GetData();
    } else {
        // Initialize after reset will come to this branch
        if (vector_buffer_type == VectorBufferType::kHeap) {
            Assert<StorageException>(buffer_->fix_heap_mgr_.get() == nullptr, "Vector heap should be null.");
            buffer_->fix_heap_mgr_ = MakeUnique<FixHeapManager>();
        }
    }

    initialized = true;

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
                //                CopyFrom<PathT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                //                break;
                //            }
                //            case kPolygon: {
                //                CopyFrom<PolygonT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                //                break;
                //            }
            case kCircle: {
                CopyFrom<CircleT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBitmap: {
                //                CopyFrom<BitmapT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                //                break;
                //            }
            case kUuid: {
                CopyFrom<UuidT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
                //            case kBlob: {
                //                CopyFrom<BlobT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                //                break;
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
                CopyFrom<MixedT>(other.buffer_.get(), this->buffer_.get(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kNull: {
                Error<ExecutorException>("Not implemented");
            }
            case kMissing: {
                Error<ExecutorException>("Not implemented");
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type");
            }
        }
    }
}

void ColumnVector::CopyRow(const ColumnVector &other, SizeT dst_idx, SizeT src_idx) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.");
    Assert<StorageException>(*data_type_ == *other.data_type_, "Data type isn't assigned.");
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(dst_idx == 0, "Attempting to access non-zero position of constant vector");
        tail_index_ = 1;
    } else {
        Assert<StorageException>(dst_idx < tail_index_, "Attempting to access invalid position of target column vector");
    }
    if (other.vector_type_ == ColumnVectorType::kConstant) {
        // Copy from constant vector, only first row have value.
        src_idx = 0;
    }

    Assert<StorageException>(src_idx < other.tail_index_, "Attempting to access invalid position of source column vector");
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
            //            CopyRowFrom<PathT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            //            break;
            //        }
            //        case kPolygon: {
            //            CopyRowFrom<PolygonT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            //            break;
            //        }
        case kCircle: {
            CopyRowFrom<CircleT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBitmap: {
            //            CopyRowFrom<BitmapT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            //            break;
            //        }
        case kUuid: {
            CopyRowFrom<UuidT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            break;
        }
            //        case kBlob: {
            //            CopyRowFrom<BlobT>(other.buffer_.get(), src_idx, this->buffer_.get(), dst_idx);
            //            break;
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
            Error<ExecutorException>("Not implemented");
        }
        case kMissing: {
            Error<ExecutorException>("Not implemented");
        }
        case kInvalid: {
            Error<ExecutorException>("Not implemented");
        }
    }
}

String ColumnVector::ToString() const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    std::stringstream ss;
    switch (data_type_->type()) {
        case kBoolean: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((BooleanT *)data_ptr_)[row_index] ? "true" : "false") << std::endl;
                ;
            }
            break;
        }
        case kTinyInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((TinyIntT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kSmallInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((SmallIntT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kInteger: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((IntegerT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kBigInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((BigIntT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kHugeInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((HugeIntT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kFloat: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((FloatT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kDouble: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((DoubleT *)data_ptr_)[row_index]) << std::endl;
                ;
            }
            break;
        }
        case kDecimal: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DecimalT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kVarchar: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((VarcharT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kDate: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DateT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kTime: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((TimeT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kDateTime: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DateT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kTimestamp: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((TimestampT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kInterval: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((IntervalT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kArray: {
            Error<NotImplementException>("Not implemented.");
        }
        case kTuple: {
            Error<NotImplementException>("Not implemented.");
        }
        case kPoint: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((PointT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kLine: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((LineT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kLineSeg: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((LineSegT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kBox: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((BoxT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
            //        case kPath: {
            //            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
            //                ss << ((PathT *)data_ptr_)[row_index].ToString().c_str() <<std::endl;;
            //            }
            //            break;
            //        }
            //        case kPolygon: {
            //            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
            //                ss << ((PolygonT *)data_ptr_)[row_index].ToString().c_str() <<std::endl;;
            //            }
            //            break;
            //        }
        case kCircle: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((CircleT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
            //        case kBitmap: {
            //            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
            //                ss << ((BitmapT *)data_ptr_)[row_index].ToString().c_str() <<std::endl;;
            //            }
            //            break;
            //        }
        case kUuid: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((UuidT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
            //        case kBlob: {
            //            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
            //                ss << ((BlobT *)data_ptr_)[row_index].ToString().c_str() <<std::endl;;
            //            }
            //            break;
            //        }
        case kEmbedding: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((EmbeddingT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kRowID: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((RowID *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        case kMixed: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((MixedT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
                ;
            }
            break;
        }
        default: {
            Error<TypeException>("Unexpected type");
        }
    }
    return ss.str();
}

String ColumnVector::ToString(SizeT row_index) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(row_index))) {
        return "null";
    }

    switch (data_type_->type()) {
        case kBoolean: {
            return ((BooleanT *)data_ptr_)[row_index] ? "true" : "false";
        }
        case kTinyInt: {
            return ToStr(((TinyIntT *)data_ptr_)[row_index]);
        }
        case kSmallInt: {
            return ToStr(((SmallIntT *)data_ptr_)[row_index]);
        }
        case kInteger: {
            return ToStr(((IntegerT *)data_ptr_)[row_index]);
        }
        case kBigInt: {
            return ToStr(((BigIntT *)data_ptr_)[row_index]);
        }
        case kHugeInt: {
            Error<TypeException>("Not implemented");
        }
        case kFloat: {
            return ToStr(((FloatT *)data_ptr_)[row_index]);
        }
        case kDouble: {
            return ToStr(((DoubleT *)data_ptr_)[row_index]);
        }
        case kDecimal: {
            Error<TypeException>("Not implemented");
        }
        case kVarchar: {
            VarcharT &varchar_ref = ((VarcharT *)data_ptr_)[row_index];
            if (varchar_ref.IsInlined()) {
                return {varchar_ref.short_.data_, varchar_ref.length_};
            } else {
                // Must be vector type
                return std::string{varchar_ref.vector_.prefix_, static_cast<size_t>(VARCHAR_PREFIX_LEN)};
            }
        }
        case kDate: {
            Error<TypeException>("Not implemented");
        }
        case kTime: {
            Error<TypeException>("Not implemented");
        }
        case kDateTime: {
            Error<TypeException>("Not implemented");
        }
        case kTimestamp: {
            Error<TypeException>("Not implemented");
        }
        case kInterval: {
            Error<TypeException>("Not implemented");
        }
        case kArray: {
            Error<TypeException>("Not implemented");
        }
        case kTuple: {
            Error<TypeException>("Not implemented");
        }
        case kPoint: {
            Error<TypeException>("Not implemented");
        }
        case kLine: {
            Error<TypeException>("Not implemented");
        }
        case kLineSeg: {
            Error<TypeException>("Not implemented");
        }
        case kBox: {
            Error<TypeException>("Not implemented");
        }
//        case kPath: {
//            Error<TypeException>("Not implemented");
//        }
//        case kPolygon: {
//            Error<TypeException>("Not implemented");
//        }
        case kCircle: {
            Error<TypeException>("Not implemented");
        }
//        case kBitmap: {
//            Error<TypeException>("Not implemented");
//        }
        case kUuid: {
            Error<TypeException>("Not implemented");
        }
//        case kBlob: {
//            Error<TypeException>("Not implemented");
//        }
        case kEmbedding: {
            //            Error<TypeException>("Not implemented");
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                Error<TypeException>("Not implemented");
            }
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            EmbeddingT embedding_element(nullptr);
            embedding_element.ptr = (data_ptr_ + row_index * data_type_->type_info()->Size());
            String embedding_str = EmbeddingT::Embedding2String(embedding_element, embedding_info->Type(), embedding_info->Dimension());
            embedding_element.SetNull();
            return embedding_str;
        }
        case kRowID: {
            return (((RowID *)data_ptr_)[row_index]).ToString();
        }
        case kMixed: {
            Error<TypeException>("Not implemented");
        }
        default: {
            Error<TypeException>("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    return String();
}

Value ColumnVector::GetValue(SizeT index) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    if (index >= tail_index_) {
        Error<TypeException>(Format("Attempt to access an invalid index of column vector: {}", ToStr(index)));
    }

    // Not valid, make a same data type with null indicator
    if (!(this->nulls_ptr_->IsTrue(index))) {
        return Value::MakeValue(*this->data_type_);
    }

    switch (data_type_->type()) {

        case kBoolean: {
            return Value::MakeBool(((BooleanT *)data_ptr_)[index]);
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
                return Value::MakeVarchar(varchar_ptr, true);
            }
            SizeT varchar_len = varchar.length_;
            return Value::MakeVarchar(((VarcharT *)data_ptr_)[index]);
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
        case kArray: {
            return Value::MakeArray(((ArrayT *)data_ptr_)[index]);
        }
        case kTuple: {
            Error<TypeException>("Shouldn't access tuple directly, a tuple is flatten as many columns");
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
            //            return Value::MakePath(((PathT *)data_ptr_)[index]);
            //        }
            //        case kPolygon: {
            //            return Value::MakePolygon(((PolygonT *)data_ptr_)[index]);
            //        }
        case kCircle: {
            return Value::MakeCircle(((CircleT *)data_ptr_)[index]);
        }
            //        case kBitmap: {
            //            return Value::MakeBitmap(((BitmapT *)data_ptr_)[index]);
            //        }
        case kUuid: {
            return Value::MakeUuid(((UuidT *)data_ptr_)[index]);
        }
            //        case kBlob: {
            //            return Value::MakeBlob(((BlobT *)data_ptr_)[index]);
            //        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            return Value::MakeEmbedding(ptr, data_type_->type_info());
        }
        case kRowID: {
            return Value::MakeRow(((RowID *)data_ptr_)[index]);
        }
        case kMixed: {
            return Value::MakeMixedData(((MixedT *)data_ptr_)[index]);
        }
        default: {
            Error<TypeException>("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    return Value::MakeInvalid();
}

void ColumnVector::SetValue(SizeT index, const Value &value) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    Assert<StorageException>(index <= tail_index_,
                             Format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                                    ToStr(index),
                                    ToStr(tail_index_),
                                    ToStr(capacity_)));

    // TODO: Check if the value type is same as column vector type
    // TODO: if not, try to cast
    Assert<StorageException>(value.type() == *data_type_, "Attempt to store a different type value into column vector.");

    // TODO: Check if the value is null, then set the column vector validity.
    switch (data_type_->type()) {

        case kBoolean: {
            ((BooleanT *)data_ptr_)[index] = value.GetValue<BooleanT>();
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
        case kVarchar: {
            // Copy string
            const VarcharT &src_ref = value.value_.varchar;
            if (!src_ref.IsValue()) {
                Error<StorageException>("Only can set value not column vector here.");
            }

            u64 varchar_len = src_ref.length_;
            VarcharT &target_ref = ((VarcharT *)data_ptr_)[index];
            target_ref.is_value_ = false;
            target_ref.length_ = varchar_len;
            if (src_ref.IsInlined()) {
                // Only prefix is enough to contain all string data.
                Memcpy(target_ref.short_.data_, src_ref.short_.data_, varchar_len);
            } else {
                Memcpy(target_ref.vector_.prefix_, src_ref.value_.prefix_, VARCHAR_PREFIX_LEN);
                auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_ref.value_.ptr_, varchar_len);
                target_ref.vector_.chunk_id_ = chunk_id;
                target_ref.vector_.chunk_offset_ = chunk_offset;
            }
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
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns");
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
            //            u32 point_count = value.value_.path.point_count;
            //
            //            SizeT point_area_size = point_count * sizeof(PointT);
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //            Memcpy(ptr, value.value_.path.ptr, point_area_size);
            //
            //            // Why not use value.GetValue<PathT>(); ?
            //            // It will call PathT operator= to allocate new memory for point area.
            //            // In this case, we need the point area in memory vector buffer.
            //            ((PathT *)data_ptr_)[index].ptr = ptr;
            //            ((PathT *)data_ptr_)[index].point_count = point_count;
            //            ((PathT *)data_ptr_)[index].closed = value.value_.path.closed;
            //            break;
            //        }
            //        case kPolygon: {
            //
            //            u64 point_count = value.value_.polygon.point_count;
            //
            //            SizeT point_area_size = point_count * sizeof(PointT);
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //            Memcpy(ptr, value.value_.polygon.ptr, point_area_size);
            //
            //            // Why not use value.GetValue<PolygonT>(); ?
            //            // It will call PolygonT operator= to allocate new memory for point area.
            //            // In this case, we need the point area in memory vector buffer.
            //            ((PolygonT *)data_ptr_)[index].ptr = ptr;
            //            ((PolygonT *)data_ptr_)[index].point_count = point_count;
            //            ((PolygonT *)data_ptr_)[index].bounding_box = value.value_.polygon.bounding_box;
            //            break;
            //        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = value.GetValue<CircleT>();
            break;
        }
            //        case kBitmap: {
            //            u64 bit_count = value.value_.bitmap.count;
            //            u64 unit_count = BitmapT::UnitCount(bit_count);
            //
            //            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
            //            Memcpy(ptr, (void *)(value.value_.bitmap.ptr), bit_area_size);
            //
            //            ((BitmapT *)data_ptr_)[index].ptr = (u64 *)ptr;
            //            ((BitmapT *)data_ptr_)[index].count = bit_count;
            //            break;
            //        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = value.GetValue<UuidT>();
            break;
        }
            //        case kBlob: {
            //            u64 blob_size = value.value_.blob.size;
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
            //            Memcpy(ptr, (void *)(value.value_.blob.ptr), blob_size);
            //
            //            ((BlobT *)data_ptr_)[index].ptr = ptr;
            //            ((BlobT *)data_ptr_)[index].size = blob_size;
            //            break;
            //        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            Memcpy(ptr, value.value_.embedding.ptr, data_type_->Size());
            break;
        }
        case kRowID: {
            ((RowID *)data_ptr_)[index] = value.GetValue<RowID>();
            break;
        }
        case kMixed: {
            ((MixedT *)data_ptr_)[index] = value.GetValue<MixedT>();
            break;
        }
        default: {
            Error<TypeException>("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::Finalize(SizeT index) {
    Assert<StorageException>(index <= capacity_, Format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));
    tail_index_ = index;
}

void ColumnVector::SetByRawPtr(SizeT index, const_ptr_t raw_ptr) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    Assert<StorageException>(index <= capacity_, Format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_));

    Assert<StorageException>(index <= tail_index_,
                             Format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                                    ToStr(index),
                                    ToStr(tail_index_),
                                    ToStr(capacity_)));

    // We assume the value_ptr point to the same type data.

    switch (data_type_->type()) {

        case kBoolean: {
            ((BooleanT *)data_ptr_)[index] = *(BooleanT *)(raw_ptr);
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
                Error<StorageException>("Only can set value not column vector here.");
            }

            u64 varchar_len = src_ref.length_;
            VarcharT &target_ref = ((VarcharT *)data_ptr_)[index];
            target_ref.is_value_ = false;
            target_ref.length_ = varchar_len;
            if (src_ref.IsInlined()) {
                // Only prefix is enough to contain all string data.
                Memcpy(target_ref.short_.data_, src_ref.short_.data_, varchar_len);
            } else {
                Memcpy(target_ref.vector_.prefix_, src_ref.value_.prefix_, VARCHAR_PREFIX_LEN);
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
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns");
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
            //            auto *point_ptr = (PathT *)(raw_ptr);
            //            u32 point_count = point_ptr->point_count;
            //
            //            SizeT point_area_size = point_count * sizeof(PointT);
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //            Memcpy(ptr, point_ptr->ptr, point_area_size);
            //
            //            // Why not use value.GetValue<PathT>(); ?
            //            // It will call PathT operator= to allocate new memory for point area.
            //            // In this case, we need the point area in memory vector buffer.
            //            ((PathT *)data_ptr_)[index].ptr = ptr;
            //            ((PathT *)data_ptr_)[index].point_count = point_count;
            //            ((PathT *)data_ptr_)[index].closed = point_ptr->closed;
            //            break;
            //        }
            //        case kPolygon: {
            //            auto *polygon_ptr = (PolygonT *)(raw_ptr);
            //            u64 point_count = polygon_ptr->point_count;
            //
            //            SizeT point_area_size = point_count * sizeof(PointT);
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //            Memcpy(ptr, polygon_ptr->ptr, point_area_size);
            //
            //            // Why not use value.GetValue<PolygonT>(); ?
            //            // It will call PolygonT operator= to allocate new memory for point area.
            //            // In this case, we need the point area in memory vector buffer.
            //            ((PolygonT *)data_ptr_)[index].ptr = ptr;
            //            ((PolygonT *)data_ptr_)[index].point_count = point_count;
            //            ((PolygonT *)data_ptr_)[index].bounding_box = polygon_ptr->bounding_box;
            //            break;
            //        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = *(CircleT *)(raw_ptr);
            break;
        }
            //        case kBitmap: {
            //            auto *bitmap_ptr = (BitmapT *)(raw_ptr);
            //            u64 bit_count = bitmap_ptr->count;
            //            u64 unit_count = BitmapT::UnitCount(bit_count);
            //
            //            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
            //            Memcpy(ptr, (void *)(bitmap_ptr->ptr), bit_area_size);
            //
            //            ((BitmapT *)data_ptr_)[index].ptr = (u64 *)ptr;
            //            ((BitmapT *)data_ptr_)[index].count = bit_count;
            //            break;
            //        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = *(UuidT *)(raw_ptr);
            break;
        }
            //        case kBlob: {
            //            auto *blob_ptr = (BlobT *)(raw_ptr);
            //            u64 blob_size = blob_ptr->size;
            //            ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
            //            Memcpy(ptr, (void *)(blob_ptr->ptr), blob_size);
            //
            //            ((BlobT *)data_ptr_)[index].ptr = ptr;
            //            ((BlobT *)data_ptr_)[index].size = blob_size;
            //            break;
            //        }
        case kEmbedding: {
            //            auto *embedding_ptr = (EmbeddingT *)(value_ptr);
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            Memcpy(ptr, raw_ptr, data_type_->Size());
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
            Error<TypeException>("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::SetByPtr(SizeT index, const_ptr_t value_ptr) {
    // We assume the value_ptr point to the same type data.
    if (data_type()->type() == LogicalType::kEmbedding) {
        auto *embedding_ptr = (EmbeddingT *)(value_ptr);
        SetByRawPtr(index, embedding_ptr->ptr);
    } else {
        SetByRawPtr(index, value_ptr);
    }
}

void ColumnVector::AppendValue(const Value &value) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(tail_index_ < 1, Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_));
    }

    if (tail_index_ >= capacity_) {
        Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
    }
    SetValue(tail_index_++, value);
}

void ColumnVector::AppendByRawPtr(const_ptr_t raw_ptr) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(tail_index_ < 1, Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_));
    }
    if (tail_index_ >= capacity_) {
        Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
    }
    SetByRawPtr(tail_index_++, raw_ptr);
}

void ColumnVector::AppendByPtr(const_ptr_t value_ptr) {

    if (data_type_->type() == LogicalType::kEmbedding) {
        AppendByRawPtr(value_ptr);
    } else {
        Assert<StorageException>(initialized, "Column vector isn't initialized.");
        if (vector_type_ == ColumnVectorType::kConstant) {
            Assert<StorageException>(tail_index_ < 1, Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_));
        }
        if (tail_index_ >= capacity_) {
            Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
        }

        SetByPtr(tail_index_++, value_ptr);
    }
}

void ColumnVector::AppendWith(const ColumnVector &other) { return AppendWith(other, 0, other.Size()); }

void ColumnVector::AppendWith(const ColumnVector &other, SizeT from, SizeT count) {
    if (count == 0) {
        return;
    }

    Assert<StorageException>(
        *this->data_type_ == *other.data_type_,
        Format("Attempt to append column vector{} to column vector{}", other.data_type()->ToString(), this->data_type()->ToString()));

    Assert<StorageException>(
        this->tail_index_ + count <= this->capacity_,
        Format("Attempt to append {} rows data to {} rows data, which exceeds {} limit.", count, this->tail_index_, this->capacity_));

    switch (data_type_->type()) {
        case kBoolean: {
            auto *src_ptr = (BooleanT *)(other.data_ptr_);
            BooleanT *dst_ptr = &((BooleanT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kTinyInt: {
            auto *src_ptr = (TinyIntT *)(other.data_ptr_);
            TinyIntT *dst_ptr = &((TinyIntT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kSmallInt: {
            auto *src_ptr = (SmallIntT *)(other.data_ptr_);
            SmallIntT *dst_ptr = &((SmallIntT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kInteger: {
            auto *src_ptr = (IntegerT *)(other.data_ptr_);
            IntegerT *dst_ptr = &((IntegerT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kBigInt: {
            auto *src_ptr = (BigIntT *)(other.data_ptr_);
            BigIntT *dst_ptr = &((BigIntT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kHugeInt: {
            auto *src_ptr = (HugeIntT *)(other.data_ptr_);
            HugeIntT *dst_ptr = &((HugeIntT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kFloat: {
            auto *src_ptr = (FloatT *)(other.data_ptr_);
            FloatT *dst_ptr = &((FloatT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kDouble: {
            auto *src_ptr = (DoubleT *)(other.data_ptr_);
            DoubleT *dst_ptr = &((DoubleT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kDecimal: {
            auto *src_ptr = (DecimalT *)(other.data_ptr_);
            DecimalT *dst_ptr = &((DecimalT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kVarchar: {
            // Copy string
            auto *base_src_ptr = (VarcharT *)(other.data_ptr_);
            VarcharT *base_dst_ptr = &((VarcharT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                VarcharT &src_ref = base_src_ptr[idx];
                VarcharT &dst_ref = base_dst_ptr[idx];
                dst_ref.length_ = src_ref.length_;
                if (src_ref.IsInlined()) {
                    Memcpy(dst_ref.value_.ptr_, src_ref.value_.ptr_, src_ref.length_);
                } else {
                    // Assume the source must be column vector type.
                    Memcpy(dst_ref.vector_.prefix_, src_ref.vector_.prefix_, VARCHAR_PREFIX_LEN);
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
            auto *src_ptr = (DateT *)(other.data_ptr_);
            DateT *dst_ptr = &((DateT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kTime: {
            auto *src_ptr = (TimeT *)(other.data_ptr_);
            TimeT *dst_ptr = &((TimeT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kDateTime: {
            auto *src_ptr = (DateTimeT *)(other.data_ptr_);
            DateTimeT *dst_ptr = &((DateTimeT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kTimestamp: {
            auto *src_ptr = (TimestampT *)(other.data_ptr_);
            TimestampT *dst_ptr = &((TimestampT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kInterval: {
            auto *src_ptr = (IntervalT *)(other.data_ptr_);
            IntervalT *dst_ptr = &((IntervalT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kArray: {
            Error<NotImplementException>("Array copy");
            break;
        }
        case kTuple: {
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns");
        }
        case kPoint: {
            auto *src_ptr = (PointT *)(other.data_ptr_);
            PointT *dst_ptr = &((PointT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kLine: {
            auto *src_ptr = (LineT *)(other.data_ptr_);
            LineT *dst_ptr = &((LineT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kLineSeg: {
            auto *src_ptr = (LineSegT *)(other.data_ptr_);
            LineSegT *dst_ptr = &((LineSegT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kBox: {
            auto *src_ptr = (BoxT *)(other.data_ptr_);
            BoxT *dst_ptr = &((BoxT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
            //        case kPath: {
            //            auto *base_src_ptr = (PathT *)(other.data_ptr_);
            //            PathT *base_dst_ptr = &((PathT *)(data_ptr_))[this->tail_index_];
            //            for (SizeT idx = 0; idx < count; ++idx) {
            //                PathT &src_ref = base_src_ptr[idx];
            //                u32 point_count = src_ref.point_count;
            //                PathT &dst_ref = base_dst_ptr[idx];
            //
            //                // Allocate the mem
            //                SizeT point_area_size = point_count * sizeof(PointT);
            //                ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //
            //                // Copy the point data
            //                Memcpy(ptr, src_ref.ptr, point_area_size);
            //
            //                dst_ref.ptr = ptr;
            //                dst_ref.point_count = point_count;
            //                dst_ref.closed = src_ref.closed;
            //            }
            //            break;
            //        }
            //        case kPolygon: {
            //            auto *base_src_ptr = (PolygonT *)(other.data_ptr_);
            //            PolygonT *base_dst_ptr = &((PolygonT *)(data_ptr_))[this->tail_index_];
            //            for (SizeT idx = 0; idx < count; ++idx) {
            //                PolygonT &src_ref = base_src_ptr[idx];
            //                u32 point_count = src_ref.point_count;
            //                PolygonT &dst_ref = base_dst_ptr[idx];
            //
            //                // Allocate the mem
            //                SizeT point_area_size = point_count * sizeof(PointT);
            //                ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
            //
            //                // Copy the point data
            //                Memcpy(ptr, src_ref.ptr, point_area_size);
            //
            //                dst_ref.ptr = ptr;
            //                dst_ref.point_count = point_count;
            //                dst_ref.bounding_box = src_ref.bounding_box;
            //            }
            //            break;
            //        }
        case kCircle: {
            auto *src_ptr = (CircleT *)(other.data_ptr_);
            CircleT *dst_ptr = &((CircleT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
            //        case kBitmap: {
            //            auto *base_src_ptr = (BitmapT *)(other.data_ptr_);
            //            BitmapT *base_dst_ptr = &((BitmapT *)(data_ptr_))[this->tail_index_];
            //            for (SizeT idx = 0; idx < count; ++idx) {
            //                BitmapT &src_ref = base_src_ptr[idx];
            //                u64 bit_count = src_ref.count;
            //                u64 unit_count = BitmapT::UnitCount(bit_count);
            //
            //                BitmapT &dst_ref = base_dst_ptr[idx];
            //
            //                // Allocate the mem
            //                SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            //                ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
            //                Memcpy(ptr, (void *)(src_ref.ptr), bit_area_size);
            //
            //                dst_ref.count = bit_count;
            //                dst_ref.ptr = (u64 *)ptr;
            //            }
            //            break;
            //        }
        case kUuid: {
            auto *src_ptr = (UuidT *)(other.data_ptr_);
            UuidT *dst_ptr = &((UuidT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
            //        case kBlob: {
            //            auto *base_src_ptr = (BlobT *)(other.data_ptr_);
            //            BlobT *base_dst_ptr = &((BlobT *)(data_ptr_))[this->tail_index_];
            //            for (SizeT idx = 0; idx < count; ++idx) {
            //                BlobT &src_ref = base_src_ptr[idx];
            //                BlobT &dst_ref = base_dst_ptr[idx];
            //
            //                // Allocate the mem
            //                ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(src_ref.size);
            //                Memcpy(ptr, (void *)(src_ref.ptr), src_ref.size);
            //
            //                dst_ref.size = src_ref.size;
            //                dst_ref.ptr = ptr;
            //            }
            //            break;
            //        }
        case kEmbedding: {
            //            auto *base_src_ptr = (EmbeddingT *)(other.data_ptr_);
            auto *base_src_ptr = other.data_ptr_;
            ptr_t base_dst_ptr = data_ptr_ + this->tail_index_ * data_type_->Size();
            for (SizeT idx = 0; idx < count; ++idx) {
                ptr_t src_ptr = base_src_ptr + idx * data_type_->Size();
                ptr_t dst_ptr = base_dst_ptr + idx * data_type_->Size();
                Memcpy(dst_ptr, src_ptr, data_type_->Size());
            }
            break;
        }
        case kRowID: {
            auto *src_ptr = (RowID *)(other.data_ptr_);
            RowID *dst_ptr = &((RowID *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kMixed: {
            auto *src_ptr = (MixedT *)(other.data_ptr_);
            MixedT *dst_ptr = &((MixedT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        default: {
            Error<TypeException>("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
    this->tail_index_ += count;
}

SizeT ColumnVector::AppendWith(ColumnBuffer &column_buffer, SizeT start_row, SizeT row_count) {
    if (row_count == 0) {
        return 0;
    }

    SizeT appended_rows = row_count;
     if (tail_index_ + row_count > capacity_) {
         // attempt to append data rows more than the capacity;
         appended_rows = capacity_ - tail_index_;
     }

     switch (data_type_->type()) {
         case kBoolean:
         case kTinyInt:
         case kSmallInt:
         case kInteger:
         case kBigInt:
         case kHugeInt:
         case kDecimal:
         case kFloat:
         case kDouble:
         case kDate:
         case kTime:
         case kDateTime:
         case kTimestamp:
         case kInterval:
         case kPoint:
         case kLine:
         case kLineSeg:
         case kBox:
         case kCircle:
//         case kBitmap:
         case kUuid:
         case kEmbedding:
         case kRowID: {
             const_ptr_t ptr = column_buffer.GetAll();
             const_ptr_t src_ptr = ptr + start_row * data_type_size_;
             ptr_t dst_ptr = data_ptr_ + tail_index_ * data_type_size_;
             Memcpy(dst_ptr, src_ptr, appended_rows * data_type_size_);
             this->tail_index_ += appended_rows;
             break;
         }
//
         case kVarchar: {
             for (SizeT row_idx = 0; row_idx < appended_rows; row_idx++) {
                 auto [src_ptr, data_size] = column_buffer.GetVarcharAt(row_idx);
//                 auto varchar_type = reinterpret_cast<VarcharT *>(data_ptr_) + tail_index_;
//                 varchar_type->Initialize(src_ptr, data_size);
                 this->tail_index_++;
             }
             break;
         }
         case kArray:
         case kTuple:
//         case kPath:
//         case kPolygon:
//         case kBlob:
         case kMixed:
         case kNull: {
             LOG_ERROR(Format("{} isn't supported", data_type_->ToString()));
             Error<NotImplementException>("Not supported now in append data in column");
         }
         case kMissing:
         case kInvalid: {
             LOG_ERROR(Format("Invalid data type {}", data_type_->ToString()));
             Error<StorageException>("Invalid data type");
         }
         default: {
             Error<TypeException>("Attempt to store an unaccepted type");
             // Null/Missing/Invalid
         }
     }
    return appended_rows;
}

SizeT ColumnVector::AppendWith(RowID from, SizeT row_count) {
    Assert<StorageException>(data_type_->type() == LogicalType::kRowID, "Only RowID column vector supports this method");
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
        LOG_ERROR(Format("{} isn't supported", data_type_->ToString()));
        Error<StorageException>(
            Format("Attempt to shallow copy: {} column vector to: {}", other.data_type_->ToString(), this->data_type_->ToString()));
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

void ColumnVector::Reserve(SizeT new_capacity) {
    Assert<StorageException>(vector_type_ != ColumnVectorType::kConstant, "Constant column vector can only have one value");
    Assert<StorageException>(initialized, "Column vector isn't initialized.");

    if (new_capacity <= capacity_)
        return;

    SharedPtr<VectorBuffer> new_buffer = VectorBuffer::Make(data_type_size_, new_capacity, buffer_->buffer_type_);
    new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);
    new_buffer->fix_heap_mgr_ = Move(buffer_->fix_heap_mgr_);
    buffer_ = new_buffer;

    capacity_ = new_capacity;
    data_ptr_ = buffer_->GetData();
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
    switch (data_type_->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
//        case kBitmap:
        case kUuid:
        case kEmbedding: {
            return 0 == Memcmp(this->data_ptr_, other.data_ptr_, this->tail_index_ * this->data_type_size_);
        }
        default: {
            Error<NotImplementException>(Format("Not supported data_type {}", int(data_type_->type())));
        }
    }
    return true;
}

i32 ColumnVector::GetSizeInBytes() const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    Assert<StorageException>(vector_type_ == ColumnVectorType::kFlat || vector_type_ == ColumnVectorType::kConstant,
                             Format("Not supported vector_type {}", int(vector_type_)));
    i32 size = this->data_type_->GetSizeInBytes() + sizeof(ColumnVectorType);
    switch (data_type_->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
//        case kBitmap:
        case kUuid:
        case kRowID:
        case kEmbedding: {
            size += sizeof(i32) + this->tail_index_ * this->data_type_size_;
            break;
        }
        case kVarchar: {
            size += sizeof(i32);

            // FIME: on `Finalize` phase, the column vector size can be settled.
            VarcharT *base_dst_ptr = (VarcharT *)(this->data_ptr_);
            for (SizeT idx = 0; idx < this->tail_index_; idx++) {
                VarcharT &val_ref = base_dst_ptr[idx];
                size += val_ref.length_;
            }
            break;
        }
        default:
            // TODO: add support for kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type_->ToString()));
    }
    size += this->nulls_ptr_->GetSizeInBytes();
    return size;
}

void ColumnVector::WriteAdv(char *&ptr) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.");
    Assert<NotImplementException>(vector_type_ == ColumnVectorType::kFlat || vector_type_ == ColumnVectorType::kConstant,
                                  Format("Not supported vector_type {}", int(vector_type_)));
    this->data_type_->WriteAdv(ptr);
    WriteBufAdv<ColumnVectorType>(ptr, this->vector_type_);
    switch (data_type_->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
//        case kBitmap:
        case kUuid:
        case kEmbedding: {
            WriteBufAdv<i32>(ptr, tail_index_);
            Memcpy(ptr, this->data_ptr_, this->tail_index_ * this->data_type_size_);
            ptr += this->tail_index_ * this->data_type_size_;
            break;
        }
        case kVarchar: {
            Error<NotImplementException>(Format("Not supported data_type {}", data_type_->ToString()));
            break;
        }
        default: {
            // TODO: add support for kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type_->ToString()));
        }
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
    switch (data_type->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
//        case kBitmap:
        case kUuid:
        case kEmbedding: {
            i32 tail_index = ReadBufAdv<i32>(ptr);
            column_vector->tail_index_ = tail_index;
            i32 data_type_size = data_type->Size();
            Memcpy((void *)column_vector->data_ptr_, ptr, tail_index * data_type_size);
            ptr += tail_index * data_type_size;
            break;
        }
        case kVarchar: {
            Error<NotImplementException>(Format("Not supported data_type {}", data_type->ToString()));
            break;
        }
        default: {
            // TODO: add support for kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type->ToString()));
        }
    }
    maxbytes = ptr_end - ptr;
    Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading ColumnVector");
    column_vector->nulls_ptr_ = Bitmask::ReadAdv(ptr, maxbytes);
    maxbytes = ptr_end - ptr;
    Assert<StorageException>(maxbytes >= 0, "ptr goes out of range when reading ColumnVector");
    return column_vector;
}

} // namespace infinity
