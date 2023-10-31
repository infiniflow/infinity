//
// Created by JinHai on 2022/10/30.
//

module;

#include <sstream>

import stl;
import selection;
import parser;
import infinity_assert;
import infinity_exception;
import default_values;
import bitmask;
import vector_buffer;
import heap_chunk;
import column_buffer;
import serialize;
import third_party;
import logger;
import value;

module column_vector;

namespace infinity {

void ColumnVector::Initialize(const ColumnVector &other, const Selection &input_select) {
    Assert<StorageException>(!initialized, "Column vector is already initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.", __FILE_NAME__, __LINE__);

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
        case LogicalType::kBlob:
        case LogicalType::kBitmap:
        case LogicalType::kPolygon:
        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.", __FILE_NAME__, __LINE__);
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
            Assert<StorageException>(buffer_->heap_mgr_.get() == nullptr, "Vector heap should be null.", __FILE_NAME__, __LINE__);
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
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
                CopyFrom<BooleanT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kTinyInt: {
                CopyFrom<TinyIntT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kSmallInt: {
                CopyFrom<SmallIntT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kInteger: {
                CopyFrom<IntegerT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kBigInt: {
                CopyFrom<BigIntT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kHugeInt: {
                CopyFrom<HugeIntT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kFloat: {
                CopyFrom<FloatT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kDouble: {
                CopyFrom<DoubleT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kDecimal: {
                CopyFrom<DecimalT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kDate: {
                CopyFrom<DateT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kTime: {
                CopyFrom<TimeT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kDateTime: {
                CopyFrom<DateTimeT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kTimestamp: {
                CopyFrom<TimestampT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kInterval: {
                CopyFrom<IntervalT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kArray: {
                CopyFrom<ArrayT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kTuple: {
                CopyFrom<TupleT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kPoint: {
                CopyFrom<PointT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kLine: {
                CopyFrom<LineT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kLineSeg: {
                CopyFrom<LineSegT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kBox: {
                CopyFrom<BoxT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kPath: {
                CopyFrom<PathT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kPolygon: {
                CopyFrom<PolygonT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kCircle: {
                CopyFrom<CircleT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kBitmap: {
                CopyFrom<BitmapT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kUuid: {
                CopyFrom<UuidT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kBlob: {
                CopyFrom<BlobT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kEmbedding: {
                CopyFrom<EmbeddingT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kRowID: {
                CopyFrom<RowT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kMixed: {
                CopyFrom<MixedT>(other.data(), this->data(), tail_index_, input_select);
                break;
            }
            case kNull: {
                Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
            }
            case kMissing: {
                Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type", __FILE_NAME__, __LINE__);
            }
        }
    }
}

void ColumnVector::Initialize(const ColumnVector &other, SizeT start_idx, SizeT end_idx) {
    Initialize(other.vector_type_, other, start_idx, end_idx);
}

void ColumnVector::Initialize(ColumnVectorType vector_type, SizeT capacity) {
    Assert<StorageException>(!initialized, "Column vector is already initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(vector_type != ColumnVectorType::kInvalid,
                             "Attempt to initialize column vector to invalid type.",
                             __FILE_NAME__,
                             __LINE__);
    // TODO: No check on capacity value.

    vector_type_ = vector_type;
    capacity_ = capacity;

    tail_index_ = 0;
    data_type_size_ = data_type_->Size();
    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type_->type()) {
        case LogicalType::kBlob:
        case LogicalType::kBitmap:
        case LogicalType::kPolygon:
        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.", __FILE_NAME__, __LINE__);
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
            Assert<StorageException>(buffer_->heap_mgr_.get() == nullptr, "Vector heap should be null.", __FILE_NAME__, __LINE__);
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
        }
    }

    initialized = true;
}

void ColumnVector::Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx) {

    Assert<StorageException>(!initialized, "Column vector is already initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(end_idx > start_idx, "End index should larger than start index.", __FILE_NAME__, __LINE__);

    vector_type_ = vector_type;
    capacity_ = end_idx - start_idx;

    tail_index_ = 0;
    data_type_size_ = data_type_->Size();

    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch (data_type_->type()) {
        case LogicalType::kBlob:
        case LogicalType::kBitmap:
        case LogicalType::kPolygon:
        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            vector_buffer_type = VectorBufferType::kHeap;

            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            Error<TypeException>("Unexpected data type for column vector.", __FILE_NAME__, __LINE__);
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
            Assert<StorageException>(buffer_->heap_mgr_.get() == nullptr, "Vector heap should be null.", __FILE_NAME__, __LINE__);
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
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
                CopyFrom<BooleanT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTinyInt: {
                CopyFrom<TinyIntT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kSmallInt: {
                CopyFrom<SmallIntT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kInteger: {
                CopyFrom<IntegerT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBigInt: {
                CopyFrom<BigIntT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kHugeInt: {
                CopyFrom<HugeIntT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kFloat: {
                CopyFrom<FloatT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDouble: {
                CopyFrom<DoubleT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDecimal: {
                CopyFrom<DecimalT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kVarchar: {
                CopyFrom<VarcharT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDate: {
                CopyFrom<DateT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTime: {
                CopyFrom<TimeT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kDateTime: {
                CopyFrom<DateTimeT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTimestamp: {
                CopyFrom<TimestampT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kInterval: {
                CopyFrom<IntervalT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kArray: {
                CopyFrom<ArrayT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kTuple: {
                CopyFrom<TupleT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kPoint: {
                CopyFrom<PointT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kLine: {
                CopyFrom<LineT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kLineSeg: {
                CopyFrom<LineSegT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBox: {
                CopyFrom<BoxT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kPath: {
                CopyFrom<PathT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kPolygon: {
                CopyFrom<PolygonT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kCircle: {
                CopyFrom<CircleT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBitmap: {
                CopyFrom<BitmapT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kUuid: {
                CopyFrom<UuidT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kBlob: {
                CopyFrom<BlobT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kEmbedding: {
                CopyFrom<EmbeddingT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kRowID: {
                CopyFrom<RowT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kMixed: {
                CopyFrom<MixedT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
                break;
            }
            case kNull: {
                Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
            }
            case kMissing: {
                Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type", __FILE_NAME__, __LINE__);
            }
        }
    }
}

void ColumnVector::CopyRow(const ColumnVector &other, SizeT dst_idx, SizeT src_idx) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(data_type_->type() != LogicalType::kInvalid, "Data type isn't assigned.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(*data_type_ == *other.data_type_, "Data type isn't assigned.", __FILE_NAME__, __LINE__);
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(dst_idx == 0, "Attempting to access non-zero position of constant vector", __FILE_NAME__, __LINE__);
        tail_index_ = 1;
    } else {
        Assert<StorageException>(dst_idx < tail_index_, "Attempting to access invalid position of target column vector", __FILE_NAME__, __LINE__);
    }
    if (other.vector_type_ == ColumnVectorType::kConstant) {
        // Copy from constant vector, only first row have value.
        src_idx = 0;
    }

    Assert<StorageException>(src_idx < other.tail_index_, "Attempting to access invalid position of source column vector", __FILE_NAME__, __LINE__);
    switch (data_type_->type()) {
        case kBoolean: {
            CopyRowFrom<BooleanT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kTinyInt: {
            CopyRowFrom<TinyIntT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kSmallInt: {
            CopyRowFrom<SmallIntT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kInteger: {
            CopyRowFrom<IntegerT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kBigInt: {
            CopyRowFrom<BigIntT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kHugeInt: {
            CopyRowFrom<HugeIntT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kFloat: {
            CopyRowFrom<FloatT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kDouble: {
            CopyRowFrom<DoubleT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kDecimal: {
            CopyRowFrom<DecimalT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kVarchar: {
            CopyRowFrom<VarcharT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kDate: {
            CopyRowFrom<DateT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kTime: {
            CopyRowFrom<TimeT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kDateTime: {
            CopyRowFrom<DateTimeT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kTimestamp: {
            CopyRowFrom<TimestampT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kInterval: {
            CopyRowFrom<IntervalT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kArray: {
            CopyRowFrom<ArrayT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kTuple: {
            CopyRowFrom<TupleT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kPoint: {
            CopyRowFrom<PointT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kLine: {
            CopyRowFrom<LineT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kLineSeg: {
            CopyRowFrom<LineSegT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kBox: {
            CopyRowFrom<BoxT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kPath: {
            CopyRowFrom<PathT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kPolygon: {
            CopyRowFrom<PolygonT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kCircle: {
            CopyRowFrom<CircleT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kBitmap: {
            CopyRowFrom<BitmapT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kUuid: {
            CopyRowFrom<UuidT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kBlob: {
            CopyRowFrom<BlobT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kEmbedding: {
            CopyRowFrom<EmbeddingT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kRowID: {
            CopyRowFrom<RowT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kMixed: {
            CopyRowFrom<MixedT>(other.data(), src_idx, this->data(), dst_idx);
            break;
        }
        case kNull: {
            Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kMissing: {
            Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kInvalid: {
            Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
        }
    }
}

String ColumnVector::ToString() const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    StringStream ss;
    switch (data_type_->type()) {
        case kBoolean: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((BooleanT *)data_ptr_)[row_index] ? "true" : "false") << std::endl;
            }
            break;
        }
        case kTinyInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((TinyIntT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kSmallInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((SmallIntT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kInteger: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((IntegerT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kBigInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((BigIntT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kHugeInt: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((HugeIntT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kFloat: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((FloatT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kDouble: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << (((DoubleT *)data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kDecimal: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DecimalT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kVarchar: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((VarcharT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kDate: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DateT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kTime: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((TimeT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kDateTime: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((DateT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kTimestamp: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((TimestampT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kInterval: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((IntervalT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kArray: {
            Error<NotImplementException>("Not implemented.", __FILE_NAME__, __LINE__);
        }
        case kTuple: {
            Error<NotImplementException>("Not implemented.", __FILE_NAME__, __LINE__);
        }
        case kPoint: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((PointT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kLine: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((LineT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kLineSeg: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((LineSegT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kBox: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((BoxT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kPath: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((PathT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kPolygon: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((PolygonT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kCircle: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((CircleT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kBitmap: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((BitmapT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kUuid: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((UuidT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kBlob: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((BlobT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kEmbedding: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((EmbeddingT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kRowID: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((RowT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        case kMixed: {
            for (SizeT row_index = 0; row_index < tail_index_; ++row_index) {
                ss << ((MixedT *)data_ptr_)[row_index].ToString().c_str() << std::endl;
            }
            break;
        }
        default: {
            Error<TypeException>("Unexpected type", __FILE_NAME__, __LINE__);
        }
    }
    return ss.str();
}

String ColumnVector::ToString(SizeT row_index) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);

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
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kFloat: {
            return ToStr(((FloatT *)data_ptr_)[row_index]);
        }
        case kDouble: {
            return ToStr(((DoubleT *)data_ptr_)[row_index]);
        }
        case kDecimal: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kVarchar: {
            VarcharT varchar_ptr = ((VarcharT *)data_ptr_)[row_index];
            if (varchar_ptr.IsInlined()) {
                return {varchar_ptr.prefix, varchar_ptr.length};
            } else {
                return {varchar_ptr.ptr, varchar_ptr.length};
            }
        }
        case kDate: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kTime: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kDateTime: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kTimestamp: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kInterval: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kArray: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kTuple: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kPoint: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kLine: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kLineSeg: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kBox: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kPath: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kPolygon: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kCircle: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kBitmap: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kUuid: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kBlob: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case kEmbedding: {
            //            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
            if (data_type_->type_info()->type() != TypeInfoType::kEmbedding) {
                Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
            }
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(data_type_->type_info().get());
            EmbeddingT embedding_element(nullptr);
            embedding_element.ptr = (data_ptr_ + row_index * data_type_->type_info()->Size());
            String embedding_str = EmbeddingT::Embedding2String(embedding_element, embedding_info->Type(), embedding_info->Dimension());
            embedding_element.SetNull();
            return embedding_str;
        }
        case kRowID: {
            return (((RowT *)data_ptr_)[row_index]).ToString();
        }
        case kMixed: {
            Error<TypeException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        default: {
            Error<TypeException>("Attempt to access an unaccepted type", __FILE_NAME__, __LINE__);
            // Null/Missing/Invalid
        }
    }
}

Value ColumnVector::GetValue(SizeT index) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    if (index >= tail_index_) {
        Error<TypeException>(Format("Attempt to access an invalid index of column vector: {}", ToStr(index)), __FILE_NAME__, __LINE__);
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
            Error<TypeException>("Shouldn't access tuple directly, a tuple is flatten as many columns", __FILE_NAME__, __LINE__);
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
        case kPath: {
            return Value::MakePath(((PathT *)data_ptr_)[index]);
        }
        case kPolygon: {
            return Value::MakePolygon(((PolygonT *)data_ptr_)[index]);
        }
        case kCircle: {
            return Value::MakeCircle(((CircleT *)data_ptr_)[index]);
        }
        case kBitmap: {
            return Value::MakeBitmap(((BitmapT *)data_ptr_)[index]);
        }
        case kUuid: {
            return Value::MakeUuid(((UuidT *)data_ptr_)[index]);
        }
        case kBlob: {
            return Value::MakeBlob(((BlobT *)data_ptr_)[index]);
        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            return Value::MakeEmbedding(ptr, data_type_->type_info());
        }
        case kRowID: {
            return Value::MakeRow(((RowT *)data_ptr_)[index]);
        }
        case kMixed: {
            return Value::MakeMixedData(((MixedT *)data_ptr_)[index]);
        }
        default: {
            Error<TypeException>("Attempt to access an unaccepted type", __FILE_NAME__, __LINE__);
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::SetValue(SizeT index, const Value &value) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(index <= tail_index_,
                             Format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                                    ToStr(index),
                                    ToStr(tail_index_),
                                    ToStr(capacity_)),
                             __FILE_NAME__,
                             __LINE__);

    // TODO: Check if the value type is same as column vector type
    // TODO: if not, try to cast
    Assert<StorageException>(value.type() == *data_type_, "Attempt to store a different type value into column vector.", __FILE_NAME__, __LINE__);

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
            u16 varchar_len = value.value_.varchar.length;
            if (varchar_len <= VarcharT::INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                Memcpy(((VarcharT *)data_ptr_)[index].prefix, value.value_.varchar.prefix, varchar_len);
            } else {
                Memcpy(((VarcharT *)data_ptr_)[index].prefix, value.value_.varchar.ptr, VarcharT::PREFIX_LENGTH);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(varchar_len);
                Memcpy(ptr, value.value_.varchar.ptr, varchar_len);
                ((VarcharT *)data_ptr_)[index].ptr = ptr;
            }
            ((VarcharT *)data_ptr_)[index].length = varchar_len;
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
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns", __FILE_NAME__, __LINE__);
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
        case kPath: {
            u32 point_count = value.value_.path.point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            Memcpy(ptr, value.value_.path.ptr, point_area_size);

            // Why not use value.GetValue<PathT>(); ?
            // It will call PathT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PathT *)data_ptr_)[index].ptr = ptr;
            ((PathT *)data_ptr_)[index].point_count = point_count;
            ((PathT *)data_ptr_)[index].closed = value.value_.path.closed;
            break;
        }
        case kPolygon: {

            u64 point_count = value.value_.polygon.point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            Memcpy(ptr, value.value_.polygon.ptr, point_area_size);

            // Why not use value.GetValue<PolygonT>(); ?
            // It will call PolygonT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PolygonT *)data_ptr_)[index].ptr = ptr;
            ((PolygonT *)data_ptr_)[index].point_count = point_count;
            ((PolygonT *)data_ptr_)[index].bounding_box = value.value_.polygon.bounding_box;
            break;
        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = value.GetValue<CircleT>();
            break;
        }
        case kBitmap: {
            u64 bit_count = value.value_.bitmap.count;
            u64 unit_count = BitmapT::UnitCount(bit_count);

            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
            Memcpy(ptr, (void *)(value.value_.bitmap.ptr), bit_area_size);

            ((BitmapT *)data_ptr_)[index].ptr = (u64 *)ptr;
            ((BitmapT *)data_ptr_)[index].count = bit_count;
            break;
        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = value.GetValue<UuidT>();
            break;
        }
        case kBlob: {
            u64 blob_size = value.value_.blob.size;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(blob_size);
            Memcpy(ptr, (void *)(value.value_.blob.ptr), blob_size);

            ((BlobT *)data_ptr_)[index].ptr = ptr;
            ((BlobT *)data_ptr_)[index].size = blob_size;
            break;
        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            Memcpy(ptr, value.value_.embedding.ptr, data_type_->Size());
            break;
        }
        case kRowID: {
            ((RowT *)data_ptr_)[index] = value.GetValue<RowT>();
            break;
        }
        case kMixed: {
            ((MixedT *)data_ptr_)[index] = value.GetValue<MixedT>();
            break;
        }
        default: {
            Error<TypeException>("Attempt to store an unaccepted type", __FILE_NAME__, __LINE__);
            // Null/Missing/Invalid
        }
    }
}

void ColumnVector::Finalize(SizeT index) {
    Assert<StorageException>(index <= capacity_,
                             Format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_),
                             __FILE_NAME__,
                             __LINE__);
    tail_index_ = index;
}

void ColumnVector::SetByRawPtr(SizeT index, const_ptr_t raw_ptr) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(index <= capacity_,
                             Format("Attempt to set column vector tail index to {}, capacity: {}", index, capacity_),
                             __FILE_NAME__,
                             __LINE__);

    Assert<StorageException>(index <= tail_index_,
                             Format("Attempt to store value into unavailable row of column vector: {}, current column tail index: {}, capacity: {}",
                                    ToStr(index),
                                    ToStr(tail_index_),
                                    ToStr(capacity_)),
                             __FILE_NAME__,
                             __LINE__);

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
            auto *varchar_ptr = (VarcharT *)(raw_ptr);
            u16 varchar_len = varchar_ptr->length;
            if (varchar_len <= VarcharT::INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                Memcpy(((VarcharT *)data_ptr_)[index].prefix, varchar_ptr->prefix, varchar_len);
            } else {
                Memcpy(((VarcharT *)data_ptr_)[index].prefix, varchar_ptr->ptr, VarcharT::PREFIX_LENGTH);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(varchar_len);
                Memcpy(ptr, varchar_ptr->ptr, varchar_len);
                ((VarcharT *)data_ptr_)[index].ptr = ptr;
            }
            ((VarcharT *)data_ptr_)[index].length = varchar_len;
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
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns", __FILE_NAME__, __LINE__);
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
        case kPath: {
            auto *point_ptr = (PathT *)(raw_ptr);
            u32 point_count = point_ptr->point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            Memcpy(ptr, point_ptr->ptr, point_area_size);

            // Why not use value.GetValue<PathT>(); ?
            // It will call PathT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PathT *)data_ptr_)[index].ptr = ptr;
            ((PathT *)data_ptr_)[index].point_count = point_count;
            ((PathT *)data_ptr_)[index].closed = point_ptr->closed;
            break;
        }
        case kPolygon: {
            auto *polygon_ptr = (PolygonT *)(raw_ptr);
            u64 point_count = polygon_ptr->point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            Memcpy(ptr, polygon_ptr->ptr, point_area_size);

            // Why not use value.GetValue<PolygonT>(); ?
            // It will call PolygonT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PolygonT *)data_ptr_)[index].ptr = ptr;
            ((PolygonT *)data_ptr_)[index].point_count = point_count;
            ((PolygonT *)data_ptr_)[index].bounding_box = polygon_ptr->bounding_box;
            break;
        }
        case kCircle: {
            ((CircleT *)data_ptr_)[index] = *(CircleT *)(raw_ptr);
            break;
        }
        case kBitmap: {
            auto *bitmap_ptr = (BitmapT *)(raw_ptr);
            u64 bit_count = bitmap_ptr->count;
            u64 unit_count = BitmapT::UnitCount(bit_count);

            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
            Memcpy(ptr, (void *)(bitmap_ptr->ptr), bit_area_size);

            ((BitmapT *)data_ptr_)[index].ptr = (u64 *)ptr;
            ((BitmapT *)data_ptr_)[index].count = bit_count;
            break;
        }
        case kUuid: {
            ((UuidT *)data_ptr_)[index] = *(UuidT *)(raw_ptr);
            break;
        }
        case kBlob: {
            auto *blob_ptr = (BlobT *)(raw_ptr);
            u64 blob_size = blob_ptr->size;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(blob_size);
            Memcpy(ptr, (void *)(blob_ptr->ptr), blob_size);

            ((BlobT *)data_ptr_)[index].ptr = ptr;
            ((BlobT *)data_ptr_)[index].size = blob_size;
            break;
        }
        case kEmbedding: {
            //            auto *embedding_ptr = (EmbeddingT *)(value_ptr);
            ptr_t ptr = data_ptr_ + index * data_type_->Size();
            Memcpy(ptr, raw_ptr, data_type_->Size());
            break;
        }
        case kRowID: {
            ((RowT *)data_ptr_)[index] = *(RowT *)(raw_ptr);
            break;
        }
        case kMixed: {
            ((MixedT *)data_ptr_)[index] = *(MixedT *)(raw_ptr);
            break;
        }
        default: {
            Error<TypeException>("Attempt to store an unaccepted type", __FILE_NAME__, __LINE__);
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
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(tail_index_ < 1,
                                 Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_),
                                 __FILE_NAME__,
                                 __LINE__);
    }

    if (tail_index_ >= capacity_) {
        Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_), __FILE_NAME__, __LINE__);
    }
    SetValue(tail_index_++, value);
}

void ColumnVector::AppendByRawPtr(const_ptr_t raw_ptr) {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    if (vector_type_ == ColumnVectorType::kConstant) {
        Assert<StorageException>(tail_index_ < 1,
                                 Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_),
                                 __FILE_NAME__,
                                 __LINE__);
    }
    if (tail_index_ >= capacity_) {
        Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_), __FILE_NAME__, __LINE__);
    }
    SetByRawPtr(tail_index_++, raw_ptr);
}

void ColumnVector::AppendByPtr(const_ptr_t value_ptr) {

    if (data_type_->type() == LogicalType::kEmbedding) {
        AppendByRawPtr(value_ptr);
    } else {
        Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
        if (vector_type_ == ColumnVectorType::kConstant) {
            Assert<StorageException>(tail_index_ < 1,
                                     Format("Constant column vector will only have 1 value.({}/{})", tail_index_, capacity_),
                                     __FILE_NAME__,
                                     __LINE__);
        }
        if (tail_index_ >= capacity_) {
            Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_), __FILE_NAME__, __LINE__);
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
        Format("Attempt to append column vector{} to column vector{}", other.data_type()->ToString(), this->data_type()->ToString()),
        __FILE_NAME__,
        __LINE__);

    Assert<StorageException>(
        this->tail_index_ + count <= this->capacity_,
        Format("Attempt to append {} rows data to {} rows data, which exceeds {} limit.", count, this->tail_index_, this->capacity_),
        __FILE_NAME__,
        __LINE__);

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
                if (src_ref.IsInlined()) {
                    Memcpy(dst_ref.prefix, src_ref.prefix, src_ref.length);
                } else {
                    Memcpy(dst_ref.prefix, src_ref.prefix, VarcharT::PREFIX_LENGTH);
                    ptr_t ptr = this->buffer_->heap_mgr_->Allocate(src_ref.length);
                    Memcpy(ptr, src_ref.ptr, src_ref.length);
                    dst_ref.ptr = ptr;
                }
                dst_ref.length = src_ref.length;
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
            Error<NotImplementException>("Array copy", __FILE_NAME__, __LINE__);
            break;
        }
        case kTuple: {
            Error<StorageException>("Shouldn't store tuple directly, a tuple is flatten as many columns", __FILE_NAME__, __LINE__);
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
        case kPath: {
            auto *base_src_ptr = (PathT *)(other.data_ptr_);
            PathT *base_dst_ptr = &((PathT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                PathT &src_ref = base_src_ptr[idx];
                u32 point_count = src_ref.point_count;
                PathT &dst_ref = base_dst_ptr[idx];

                // Allocate the mem
                SizeT point_area_size = point_count * sizeof(PointT);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);

                // Copy the point data
                Memcpy(ptr, src_ref.ptr, point_area_size);

                dst_ref.ptr = ptr;
                dst_ref.point_count = point_count;
                dst_ref.closed = src_ref.closed;
            }
            break;
        }
        case kPolygon: {
            auto *base_src_ptr = (PolygonT *)(other.data_ptr_);
            PolygonT *base_dst_ptr = &((PolygonT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                PolygonT &src_ref = base_src_ptr[idx];
                u32 point_count = src_ref.point_count;
                PolygonT &dst_ref = base_dst_ptr[idx];

                // Allocate the mem
                SizeT point_area_size = point_count * sizeof(PointT);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);

                // Copy the point data
                Memcpy(ptr, src_ref.ptr, point_area_size);

                dst_ref.ptr = ptr;
                dst_ref.point_count = point_count;
                dst_ref.bounding_box = src_ref.bounding_box;
            }
            break;
        }
        case kCircle: {
            auto *src_ptr = (CircleT *)(other.data_ptr_);
            CircleT *dst_ptr = &((CircleT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kBitmap: {
            auto *base_src_ptr = (BitmapT *)(other.data_ptr_);
            BitmapT *base_dst_ptr = &((BitmapT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                BitmapT &src_ref = base_src_ptr[idx];
                u64 bit_count = src_ref.count;
                u64 unit_count = BitmapT::UnitCount(bit_count);

                BitmapT &dst_ref = base_dst_ptr[idx];

                // Allocate the mem
                SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
                Memcpy(ptr, (void *)(src_ref.ptr), bit_area_size);

                dst_ref.count = bit_count;
                dst_ref.ptr = (u64 *)ptr;
            }
            break;
        }
        case kUuid: {
            auto *src_ptr = (UuidT *)(other.data_ptr_);
            UuidT *dst_ptr = &((UuidT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                dst_ptr[idx] = src_ptr[idx];
            }
            break;
        }
        case kBlob: {
            auto *base_src_ptr = (BlobT *)(other.data_ptr_);
            BlobT *base_dst_ptr = &((BlobT *)(data_ptr_))[this->tail_index_];
            for (SizeT idx = 0; idx < count; ++idx) {
                BlobT &src_ref = base_src_ptr[idx];
                BlobT &dst_ref = base_dst_ptr[idx];

                // Allocate the mem
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(src_ref.size);
                Memcpy(ptr, (void *)(src_ref.ptr), src_ref.size);

                dst_ref.size = src_ref.size;
                dst_ref.ptr = ptr;
            }
            break;
        }
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
            auto *src_ptr = (RowT *)(other.data_ptr_);
            RowT *dst_ptr = &((RowT *)(data_ptr_))[this->tail_index_];
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
            Error<TypeException>("Attempt to store an unaccepted type", __FILE_NAME__, __LINE__);
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
        case kBitmap:
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

        case kVarchar: {
            for (SizeT row_idx = 0; row_idx < appended_rows; row_idx++) {
                auto [src_ptr, data_size] = column_buffer.GetVarcharAt(row_idx);
                auto varchar_type = reinterpret_cast<VarcharT *>(data_ptr_) + tail_index_;
                varchar_type->Initialize(src_ptr, data_size);
                this->tail_index_++;
            }
            break;
        }
        case kArray:
        case kTuple:
        case kPath:
        case kPolygon:
        case kBlob:
        case kMixed:
        case kNull: {
            LOG_ERROR(Format("{} isn't supported", data_type_->ToString()));
            Error<NotImplementException>("Not supported now in append data in column", __FILE_NAME__, __LINE__);
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR(Format("Invalid data type {}", data_type_->ToString()));
            Error<StorageException>("Invalid data type", __FILE_NAME__, __LINE__);
        }
        default: {
            Error<TypeException>("Attempt to store an unaccepted type", __FILE_NAME__, __LINE__);
            // Null/Missing/Invalid
        }
    }
    return appended_rows;
}

void ColumnVector::ShallowCopy(const ColumnVector &other) {
    if (*this->data_type_ != *other.data_type_) {
        LOG_ERROR(Format("{} isn't supported", data_type_->ToString()));
        Error<StorageException>(
            Format("Attempt to shallow copy: {} column vector to: {}", other.data_type_->ToString(), this->data_type_->ToString()),
            __FILE_NAME__,
            __LINE__);
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
    Assert<StorageException>(vector_type_ != ColumnVectorType::kConstant, "Constant column vector can only have one value", __FILE_NAME__, __LINE__);
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);

    if (new_capacity <= capacity_)
        return;

    SharedPtr<VectorBuffer> new_buffer = VectorBuffer::Make(data_type_size_, new_capacity, buffer_->buffer_type_);
    new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);
    new_buffer->heap_mgr_ = Move(buffer_->heap_mgr_);
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
        buffer_->heap_mgr_ = nullptr;
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
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            return 0 == Memcmp(this->data_ptr_, other.data_ptr_, this->tail_index_ * this->data_type_size_);
        }
        default: {
            Error<NotImplementException>(Format("Not supported data_type {}", int(data_type_->type())), __FILE_NAME__, __LINE__);
        }
    }
    return true;
}

i32 ColumnVector::GetSizeInBytes() const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    Assert<StorageException>(vector_type_ == ColumnVectorType::kFlat || vector_type_ == ColumnVectorType::kConstant,
                             Format("Not supported vector_type {}", int(vector_type_)),
                             __FILE_NAME__,
                             __LINE__);
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
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            size += sizeof(i32) + this->tail_index_ * this->data_type_size_;
            break;
        }
        default:
            // TODO: add support for kVarchar, kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type_->ToString()), __FILE_NAME__, __LINE__);
    }
    size += this->nulls_ptr_->GetSizeInBytes();
    return size;
}

void ColumnVector::WriteAdv(char *&ptr) const {
    Assert<StorageException>(initialized, "Column vector isn't initialized.", __FILE_NAME__, __LINE__);
    Assert<NotImplementException>(vector_type_ == ColumnVectorType::kFlat || vector_type_ == ColumnVectorType::kConstant,
                                  Format("Not supported vector_type {}", int(vector_type_)),
                                  __FILE_NAME__,
                                  __LINE__);
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
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            WriteBufAdv<i32>(ptr, tail_index_);
            Memcpy(ptr, this->data_ptr_, this->tail_index_ * this->data_type_size_);
            ptr += this->tail_index_ * this->data_type_size_;
            break;
        }
        default:
            // TODO: add support for kVarchar, kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type_->ToString()), __FILE_NAME__, __LINE__);
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
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            i32 tail_index = ReadBufAdv<i32>(ptr);
            column_vector->tail_index_ = tail_index;
            i32 data_type_size = data_type->Size();
            Memcpy((void *)column_vector->data_ptr_, ptr, tail_index * data_type_size);
            ptr += tail_index * data_type_size;
            break;
        }
        default:
            // TODO: add support for kVarchar, kPath, kPolygon, kArray, kBlob, kMix etc.
            Error<NotImplementException>(Format("Not supported data_type {}", data_type->ToString()), __FILE_NAME__, __LINE__);
    }
    maxbytes = ptr_end - ptr;
    Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading ColumnVector", __FILE_NAME__, __LINE__);
    column_vector->nulls_ptr_ = Bitmask::ReadAdv(ptr, maxbytes);
    maxbytes = ptr_end - ptr;
    Assert<StorageException>(maxbytes >= 0, "ptr goes out of range when reading ColumnVector", __FILE_NAME__, __LINE__);
    return column_vector;
}

} // namespace infinity
