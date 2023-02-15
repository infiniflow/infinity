//
// Created by JinHai on 2022/10/30.
//

#include "column_vector.h"

#include <utility>
#include "common/utility/infinity_assert.h"

namespace infinity {

void
ColumnVector::Initialize(const ColumnVector& other, const Selection& input_select) {
    StorageAssert(!initialized, "Column vector is already initialized.")
    StorageAssert(data_type_.type() != LogicalType::kInvalid, "Data type isn't assigned.")

    vector_type_ = other.vector_type_;
    if(vector_type_ == ColumnVectorType::kConstant) {
        capacity_ = 1;
    } else {
        capacity_ = input_select.Size();
    }

    data_type_size_ = data_type_.Size();
    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch(data_type_.type()) {
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
            TypeError("Unexpected data type for column vector.")
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }

    if(buffer_ == nullptr) {
        buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
        data_ptr_ = buffer_->GetData();
        nulls_ptr_ = Bitmask::Make(capacity_);
    } else {
        // Initialize after reset will come to this branch
        if(vector_buffer_type == VectorBufferType::kHeap) {
            StorageAssert(buffer_->heap_mgr_ == nullptr, "Vector heap should be null.")
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
        }
    }

    initialized = true;
    tail_index_ = input_select.Size();

    // Copy data from other column vector to here according to the select
    switch(data_type_.type()) {
        case kBoolean: {
            CopyFrom<BooleanT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kTinyInt: {
            CopyFrom<TinyIntT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kSmallInt: {
            CopyFrom<SmallIntT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kInteger: {
            CopyFrom<IntegerT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kBigInt: {
            CopyFrom<BigIntT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kHugeInt: {
            CopyFrom<HugeIntT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kFloat: {
            CopyFrom<FloatT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDouble: {
            CopyFrom<DoubleT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDecimal16: {
            CopyFrom<Decimal16T>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDecimal32: {
            CopyFrom<Decimal32T>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDecimal64: {
            CopyFrom<Decimal64T>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDecimal128: {
            CopyFrom<Decimal128T>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kVarchar: {
            CopyFrom<VarcharT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kChar: {
            CopyFrom<CharT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDate: {
            CopyFrom<DateT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kTime: {
            CopyFrom<TimeT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kDateTime: {
            CopyFrom<DateTimeT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kTimestamp: {
            CopyFrom<TimestampT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kTimestampTZ: {
            CopyFrom<TimestampTZT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kInterval: {
            CopyFrom<IntervalT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kArray: {
            CopyFrom<ArrayT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kTuple: {
            CopyFrom<TupleT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kPoint: {
            CopyFrom<PointT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kLine: {
            CopyFrom<LineT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kLineSeg: {
            CopyFrom<LineSegT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kBox: {
            CopyFrom<BoxT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kPath: {
            CopyFrom<PathT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kPolygon: {
            CopyFrom<PolygonT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kCircle: {
            CopyFrom<CircleT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kBitmap: {
            CopyFrom<BitmapT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kUuid: {
            CopyFrom<UuidT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kBlob: {
            CopyFrom<BlobT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kEmbedding: {
            CopyFrom<EmbeddingT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kMixed: {
            CopyFrom<MixedT>(other.data(), this->data(), input_select.Size(), input_select);
            break;
        }
        case kNull: {
            ExecutorError("Not implemented")
        }
        case kMissing: {
            ExecutorError("Not implemented")
        }
        case kInvalid: {
            ExecutorError("Invalid data type")
        }
    }
}

void
ColumnVector::Initialize(const ColumnVector& other, SizeT start_idx, SizeT end_idx) {
    Initialize(other.vector_type_, other, start_idx, end_idx);
}

void
ColumnVector::Initialize(ColumnVectorType vector_type, SizeT capacity) {
    StorageAssert(!initialized, "Column vector is already initialized.")
    StorageAssert(data_type_.type() != LogicalType::kInvalid, "Data type isn't assigned.")
    StorageAssert(vector_type != ColumnVectorType::kInvalid, "Attempt to initialize column vector to invalid type.")
    // TODO: No check on capacity value.

    vector_type_ = vector_type;
    if(vector_type_ == ColumnVectorType::kConstant) {
        capacity_ = 1;
    } else {
        capacity_ = capacity;
    }

    tail_index_ = 0;
    data_type_size_ = data_type_.Size();
    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch(data_type_.type()) {
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
            TypeError("Unexpected data type for column vector.")
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }
    if(buffer_ == nullptr) {
        buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
        data_ptr_ = buffer_->GetData();
        nulls_ptr_ = Bitmask::Make(capacity_);
    } else {
        // Initialize after reset will come to this branch
        if(vector_buffer_type == VectorBufferType::kHeap) {
            StorageAssert(buffer_->heap_mgr_ == nullptr, "Vector heap should be null.")
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
        }
    }

    initialized = true;
}

void
ColumnVector::Initialize(ColumnVectorType vector_type,
                         const ColumnVector& other,
                         SizeT start_idx,
                         SizeT end_idx) {

    StorageAssert(!initialized, "Column vector isn't initialized.")
    StorageAssert(data_type_.type() != LogicalType::kInvalid, "Data type isn't assigned.")
    // TODO: No check on capacity value.

    vector_type_ = vector_type;
    if(vector_type_ == ColumnVectorType::kConstant) {
        capacity_ = 1;
    } else {
        capacity_ = end_idx - start_idx;
    }

    tail_index_ = 0;
    data_type_size_ = data_type_.Size();

    VectorBufferType vector_buffer_type = VectorBufferType::kInvalid;
    switch(data_type_.type()) {
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
            TypeError("Unexpected data type for column vector.")
        }
        default: {
            vector_buffer_type = VectorBufferType::kStandard;
        }
    }

    if(buffer_ == nullptr) {
        buffer_ = VectorBuffer::Make(data_type_size_, capacity_, vector_buffer_type);
        data_ptr_ = buffer_->GetData();
        nulls_ptr_ = Bitmask::Make(capacity_);
    } else {
        // Initialize after reset will come to this branch
        if(vector_buffer_type == VectorBufferType::kHeap) {
            StorageAssert(buffer_->heap_mgr_ == nullptr, "Vector heap should be null.")
            buffer_->heap_mgr_ = MakeUnique<StringHeapMgr>();
        }
    }

    initialized = true;
    tail_index_ = capacity_;

    // Copy data from other column vector to here according to the range
    switch(data_type_.type()) {
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
        case kDecimal16: {
            CopyFrom<Decimal16T>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kDecimal32: {
            CopyFrom<Decimal32T>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kDecimal64: {
            CopyFrom<Decimal64T>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kDecimal128: {
            CopyFrom<Decimal128T>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kVarchar: {
            CopyFrom<VarcharT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kChar: {
            CopyFrom<CharT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
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
        case kTimestampTZ: {
            CopyFrom<TimestampTZT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
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
        case kMixed: {
            CopyFrom<MixedT>(other.data(), this->data(), start_idx, 0, end_idx - start_idx);
            break;
        }
        case kNull: {
            ExecutorError("Not implemented")
        }
        case kMissing: {
            ExecutorError("Not implemented")
        }
        case kInvalid: {
            ExecutorError("Invalid data type")
        }
    }
}

String
ColumnVector::ToString() const {
    StorageAssert(initialized, "Column vector isn't initialized.")
    std::stringstream ss;
    switch(data_type_.type()) {
        case kBoolean: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((BooleanT *) data_ptr_)[row_index] ? "true" : "false") << std::endl;
            }
            break;
        }
        case kTinyInt: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((TinyIntT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kSmallInt: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((SmallIntT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kInteger: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((IntegerT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kBigInt: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((BigIntT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kHugeInt: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((HugeIntT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kFloat: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((FloatT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kDouble: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << (((DoubleT *) data_ptr_)[row_index]) << std::endl;
            }
            break;
        }
        case kDecimal16: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((Decimal16T *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kDecimal32: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((Decimal32T *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kDecimal64: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((Decimal64T *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kDecimal128: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((Decimal128T *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kVarchar: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((VarcharT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kChar: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((CharT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kDate: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((DateT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kTime: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((TimeT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kDateTime: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((DateT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kTimestamp: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((TimestampT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kTimestampTZ: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((TimestampTZT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kInterval: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((IntervalT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kArray: {
            NotImplementError("Not implemented.")
        }
        case kTuple: {
            NotImplementError("Not implemented.")
        }
        case kPoint: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((PointT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kLine: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((LineT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kLineSeg: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((LineSegT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kBox: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((BoxT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kPath: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((PathT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kPolygon: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((PolygonT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kCircle: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((CircleT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kBitmap: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((BitmapT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kUuid: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((UuidT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kBlob: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((BlobT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kEmbedding: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((EmbeddingT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        case kMixed: {
            for(SizeT row_index = 0; row_index < tail_index_; ++ row_index) {
                ss << ((MixedT *) data_ptr_)[row_index].ToString() << std::endl;
            }
            break;
        }
        default: {
            TypeError("Unexpected type")
        }
    }
    return ss.str();
}

String
ColumnVector::ToString(SizeT row_index) const {
    StorageAssert(initialized, "Column vector isn't initialized.")

    // Not valid, make a same data type with null indicator
    if(!(this->nulls_ptr_->IsTrue(row_index))) {
        return "null";
    }

    switch(data_type_.type()) {
        case kBoolean: {
            return ((BooleanT *) data_ptr_)[row_index] ? "true": "false";
        }
        case kTinyInt: {
            return std::to_string(((TinyIntT *) data_ptr_)[row_index]);
        }
        case kSmallInt: {
            return std::to_string(((SmallIntT *) data_ptr_)[row_index]);
        }
        case kInteger: {
            return std::to_string(((IntegerT *) data_ptr_)[row_index]);
        }
        case kBigInt: {
            return std::to_string(((BigIntT *) data_ptr_)[row_index]);
        }
        case kHugeInt: {
            TypeError("Not implemented");
        }
        case kFloat: {
            return std::to_string(((FloatT *) data_ptr_)[row_index]);
        }
        case kDouble: {
            return std::to_string(((DoubleT *) data_ptr_)[row_index]);
        }
        case kDecimal16: {
            TypeError("Not implemented");
        }
        case kDecimal32: {
            TypeError("Not implemented");
        }
        case kDecimal64: {
            TypeError("Not implemented");
        }
        case kDecimal128: {
            TypeError("Not implemented");
        }
        case kVarchar: {
            VarcharT varchar_ptr = ((VarcharT *) data_ptr_)[row_index];
            if(varchar_ptr.IsInlined()) {
                return {varchar_ptr.prefix, VarcharT::INLINE_LENGTH};
            } else {
                return {varchar_ptr.ptr, varchar_ptr.length};
            }
        }
        case kChar: {
            CharT char_ptr = ((CharT *) data_ptr_)[row_index];
            return {char_ptr.ptr, data_type_.type_info()->Size()};
        }
        case kDate: {
            TypeError("Not implemented");
        }
        case kTime: {
            TypeError("Not implemented");
        }
        case kDateTime: {
            TypeError("Not implemented");
        }
        case kTimestamp: {
            TypeError("Not implemented");
        }
        case kTimestampTZ: {
            TypeError("Not implemented");
        }
        case kInterval: {
            TypeError("Not implemented");
        }
        case kArray: {
            TypeError("Not implemented");
        }
        case kTuple: {
            TypeError("Not implemented");
        }
        case kPoint: {
            TypeError("Not implemented");
        }
        case kLine: {
            TypeError("Not implemented");
        }
        case kLineSeg: {
            TypeError("Not implemented");
        }
        case kBox: {
            TypeError("Not implemented");
        }
        case kPath: {
            TypeError("Not implemented");
        }
        case kPolygon: {
            TypeError("Not implemented");
        }
        case kCircle: {
            TypeError("Not implemented");
        }
        case kBitmap: {
            TypeError("Not implemented");
        }
        case kUuid: {
            TypeError("Not implemented");
        }
        case kBlob: {
            TypeError("Not implemented");
        }
        case kEmbedding: {
            TypeError("Not implemented");
        }
        case kMixed: {
            TypeError("Not implemented");
        }
        default: {
            TypeError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

Value
ColumnVector::GetValue(SizeT index) const {
    StorageAssert(initialized, "Column vector isn't initialized.")
    if(index >= tail_index_) {
        TypeError("Attempt to access an invalid index of column vector: " + std::to_string(index));
    }

    // Not valid, make a same data type with null indicator
    if(!(this->nulls_ptr_->IsTrue(index))) {
        return Value::MakeValue(this->data_type_);
    }

    switch(data_type_.type()) {

        case kBoolean: {
            return Value::MakeBool(((BooleanT *) data_ptr_)[index]);
        }
        case kTinyInt: {
            return Value::MakeTinyInt(((TinyIntT *) data_ptr_)[index]);
        }
        case kSmallInt: {
            return Value::MakeSmallInt(((SmallIntT *) data_ptr_)[index]);
        }
        case kInteger: {
            return Value::MakeInt(((IntegerT *) data_ptr_)[index]);
        }
        case kBigInt: {
            return Value::MakeBigInt(((BigIntT *) data_ptr_)[index]);
        }
        case kHugeInt: {
            return Value::MakeHugeInt(((HugeIntT *) data_ptr_)[index]);
        }
        case kFloat: {
            return Value::MakeFloat(((FloatT *) data_ptr_)[index]);
        }
        case kDouble: {
            return Value::MakeDouble(((DoubleT *) data_ptr_)[index]);
        }
        case kDecimal16: {
            return Value::MakeDecimal16(((Decimal16T *) data_ptr_)[index], data_type_.type_info());
        }
        case kDecimal32: {
            return Value::MakeDecimal32(((Decimal32T *) data_ptr_)[index], data_type_.type_info());
        }
        case kDecimal64: {
            return Value::MakeDecimal64(((Decimal64T *) data_ptr_)[index], data_type_.type_info());
        }
        case kDecimal128: {
            return Value::MakeDecimal128(((Decimal128T *) data_ptr_)[index], data_type_.type_info());
        }
        case kVarchar: {
            return Value::MakeVarchar(((VarcharT *) data_ptr_)[index], data_type_.type_info());
        }
        case kChar: {
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            return Value::MakeChar(ptr, data_type_.type_info());
        }
        case kDate: {
            return Value::MakeDate(((DateT *) data_ptr_)[index]);
        }
        case kTime: {
            return Value::MakeTime(((TimeT *) data_ptr_)[index]);
        }
        case kDateTime: {
            return Value::MakeDateTime(((DateTimeT *) data_ptr_)[index]);
        }
        case kTimestamp: {
            return Value::MakeTimestamp(((TimestampT *) data_ptr_)[index]);
        }
        case kTimestampTZ: {
            return Value::MakeTimestampTz(((TimestampTZT *) data_ptr_)[index]);
        }
        case kInterval: {
            return Value::MakeInterval(((IntervalT *) data_ptr_)[index]);
        }
        case kArray: {
            return Value::MakeArray(((ArrayT *) data_ptr_)[index]);
        }
        case kTuple: {
            TypeError("Shouldn't access tuple directly, a tuple is flatten as many columns");
        }
        case kPoint: {
            return Value::MakePoint(((PointT *) data_ptr_)[index]);
        }
        case kLine: {
            return Value::MakeLine(((LineT *) data_ptr_)[index]);
        }
        case kLineSeg: {
            return Value::MakeLineSegment(((LineSegT *) data_ptr_)[index]);
        }
        case kBox: {
            return Value::MakeBox(((BoxT *) data_ptr_)[index]);
        }
        case kPath: {
            return Value::MakePath(((PathT *) data_ptr_)[index]);
        }
        case kPolygon: {
            return Value::MakePolygon(((PolygonT *) data_ptr_)[index]);
        }
        case kCircle: {
            return Value::MakeCircle(((CircleT *) data_ptr_)[index]);
        }
        case kBitmap: {
            return Value::MakeBitmap(((BitmapT *) data_ptr_)[index]);
        }
        case kUuid: {
            return Value::MakeUuid(((UuidT *) data_ptr_)[index]);
        }
        case kBlob: {
            return Value::MakeBlob(((BlobT *) data_ptr_)[index]);
        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            return Value::MakeEmbedding(ptr, data_type_.type_info());
        }
        case kMixed: {
            return Value::MakeMixedData(((MixedT *) data_ptr_)[index]);
        }
        default: {
            TypeError("Attempt to access an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void
ColumnVector::SetValue(SizeT index, const Value &value) {
    StorageAssert(initialized, "Column vector isn't initialized.")
    StorageAssert(index <= tail_index_,
                  "Attempt to store value into unavailable row of column vector: "
                  + std::to_string(index) + ", current column tail index: " + std::to_string(tail_index_)
                  + ", capacity: " + std::to_string(capacity_))

    // TODO: Check if the value type is same as column vector type
    // TODO: if not, try to cast
    TypeAssert(value.type() == data_type_, "Attempt to store a different type value into column vector.");

    // TODO: Check if the value is null, then set the column vector validity.
    switch(data_type_.type()) {

        case kBoolean: {
            ((BooleanT*)data_ptr_)[index] = value.GetValue<BooleanT>();
            break;
        }
        case kTinyInt: {
            ((TinyIntT *)data_ptr_)[index] = value.GetValue<TinyIntT>();
            break;
        }
        case kSmallInt: {
            ((SmallIntT *) data_ptr_)[index] = value.GetValue<SmallIntT>();
            break;
        }
        case kInteger: {
            ((IntegerT *) data_ptr_)[index] = value.GetValue<IntegerT>();
            break;
        }
        case kBigInt: {
            ((BigIntT *) data_ptr_)[index] = value.GetValue<BigIntT>();
            break;
        }
        case kHugeInt: {
            ((HugeIntT *) data_ptr_)[index] = value.GetValue<HugeIntT>();
            break;
        }
        case kFloat: {
            ((FloatT *) data_ptr_)[index] = value.GetValue<FloatT>();
            break;
        }
        case kDouble: {
            ((DoubleT *) data_ptr_)[index] = value.GetValue<DoubleT>();
            break;
        }
        case kDecimal16: {
            ((Decimal16T *) data_ptr_)[index] = value.GetValue<Decimal16T>();
            break;
        }
        case kDecimal32: {
            ((Decimal32T *) data_ptr_)[index] = value.GetValue<Decimal32T>();
            break;
        }
        case kDecimal64: {
            ((Decimal64T *) data_ptr_)[index] = value.GetValue<Decimal64T>();
            break;
        }
        case kDecimal128: {
            ((Decimal128T *) data_ptr_)[index] = value.GetValue<Decimal128T>();
            break;
        }
        case kVarchar: {
            // Copy string
            u16 varchar_len = value.value_.varchar.length;
            if(varchar_len <= VarcharType::INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                memcpy(((VarcharT *) data_ptr_)[index].prefix, value.value_.varchar.prefix, varchar_len);
            } else {
                memcpy(((VarcharT *) data_ptr_)[index].prefix, value.value_.varchar.ptr, VarcharType::PREFIX_LENGTH);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(varchar_len);
                memcpy(ptr, value.value_.varchar.ptr, varchar_len);
                ((VarcharT *) data_ptr_)[index].ptr = ptr;
            }
            ((VarcharT *) data_ptr_)[index].length = varchar_len;
            break;
        }
        case kChar: {
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            memcpy(ptr, value.value_.char_n.ptr, data_type_.Size());
            break;
        }
        case kDate: {
            ((DateT *) data_ptr_)[index] = value.GetValue<DateT>();
            break;
        }
        case kTime: {
            ((TimeT *) data_ptr_)[index] = value.GetValue<TimeT>();
            break;
        }
        case kDateTime: {
            ((DateTimeT *) data_ptr_)[index] = value.GetValue<DateTimeT>();
            break;
        }
        case kTimestamp: {
            ((TimestampT *) data_ptr_)[index] = value.GetValue<TimestampT>();
            break;
        }
        case kTimestampTZ: {
            ((TimestampTZT *) data_ptr_)[index] = value.GetValue<TimestampTZT>();
            break;
        }
        case kInterval: {
            ((IntervalT *) data_ptr_)[index] = value.GetValue<IntervalT>();
            break;
        }
        case kArray: {
            ((ArrayT *) data_ptr_)[index] = value.GetValue<ArrayT>();
            break;
        }
        case kTuple: {
            TypeError("Shouldn't store tuple directly, a tuple is flatten as many columns");
        }
        case kPoint: {
            ((PointT *) data_ptr_)[index] = value.GetValue<PointT>();
            break;
        }
        case kLine: {
            ((LineT *) data_ptr_)[index] = value.GetValue<LineT>();
            break;
        }
        case kLineSeg: {
            ((LineSegT *) data_ptr_)[index] = value.GetValue<LineSegT>();
            break;
        }
        case kBox: {
            ((BoxT *) data_ptr_)[index] = value.GetValue<BoxT>();
            break;
        }
        case kPath: {
            u32 point_count = value.value_.path.point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            memcpy(ptr, value.value_.path.ptr, point_area_size);

            // Why not use value.GetValue<PathT>(); ?
            // It will call PathT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PathT *) data_ptr_)[index].ptr = ptr;
            ((PathT *) data_ptr_)[index].point_count = point_count;
            ((PathT *) data_ptr_)[index].closed = value.value_.path.closed;
            break;
        }
        case kPolygon: {

            u64 point_count = value.value_.polygon.point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            memcpy(ptr, value.value_.polygon.ptr, point_area_size);

            // Why not use value.GetValue<PolygonT>(); ?
            // It will call PolygonT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PolygonT *) data_ptr_)[index].ptr = ptr;
            ((PolygonT *) data_ptr_)[index].point_count = point_count;
            ((PolygonT *) data_ptr_)[index].bounding_box = value.value_.polygon.bounding_box;
            break;
        }
        case kCircle: {
            ((CircleT *) data_ptr_)[index] = value.GetValue<CircleT>();
            break;
        }
        case kBitmap: {
            u64 bit_count = value.value_.bitmap.count;
            u64 unit_count = BitmapT::UnitCount(bit_count);

            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
            memcpy(ptr, (void*)(value.value_.bitmap.ptr), bit_area_size);

            ((BitmapT *) data_ptr_)[index].ptr = (u64*)ptr;
            ((BitmapT *) data_ptr_)[index].count = bit_count;
            break;
        }
        case kUuid: {
            ((UuidT *) data_ptr_)[index] = value.GetValue<UuidT>();
            break;
        }
        case kBlob: {
            u64 blob_size = value.value_.blob.size;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(blob_size);
            memcpy(ptr, (void*)(value.value_.blob.ptr), blob_size);

            ((BlobT *) data_ptr_)[index].ptr = ptr;
            ((BlobT *) data_ptr_)[index].size = blob_size;
            break;
        }
        case kEmbedding: {
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            memcpy(ptr, value.value_.embedding.ptr, data_type_.Size());
            break;
        }
        case kMixed: {
            ((MixedT *) data_ptr_)[index] = value.GetValue<MixedT>();
            break;
        }
        default: {
            TypeError("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void
ColumnVector::SetByPtr(SizeT index, const ptr_t value_ptr) {
    StorageAssert(initialized, "Column vector isn't initialized.")
    StorageAssert(index <= tail_index_,
                  "Attempt to store value into unavailable row of column vector: "
                  + std::to_string(index) + ", current column tail index: " + std::to_string(tail_index_)
                  + ", capacity: " + std::to_string(capacity_))

    // We assume the value_ptr point to the same type data.

    switch(data_type_.type()) {

        case kBoolean: {
            ((BooleanT*)data_ptr_)[index] = *(BooleanT*)(value_ptr);
            break;
        }
        case kTinyInt: {
            ((TinyIntT*)data_ptr_)[index] = *(TinyIntT*)(value_ptr);
            break;
        }
        case kSmallInt: {
            ((SmallIntT*)data_ptr_)[index] = *(SmallIntT*)(value_ptr);
            break;
        }
        case kInteger: {
            ((IntegerT*)data_ptr_)[index] = *(IntegerT*)(value_ptr);
            break;
        }
        case kBigInt: {
            ((BigIntT*)data_ptr_)[index] = *(BigIntT*)(value_ptr);
            break;
        }
        case kHugeInt: {
            ((HugeIntT*)data_ptr_)[index] = *(HugeIntT*)(value_ptr);
            break;
        }
        case kFloat: {
            ((FloatT*)data_ptr_)[index] = *(FloatT*)(value_ptr);
            break;
        }
        case kDouble: {
            ((DoubleT*)data_ptr_)[index] = *(DoubleT*)(value_ptr);
            break;
        }
        case kDecimal16: {
            ((Decimal16T*)data_ptr_)[index] = *(Decimal16T*)(value_ptr);
            break;
        }
        case kDecimal32: {
            ((Decimal32T*)data_ptr_)[index] = *(Decimal32T*)(value_ptr);
            break;
        }
        case kDecimal64: {
            ((Decimal64T*)data_ptr_)[index] = *(Decimal64T*)(value_ptr);
            break;
        }
        case kDecimal128: {
            ((Decimal128T*)data_ptr_)[index] = *(Decimal128T*)(value_ptr);
            break;
        }
        case kVarchar: {
            // Copy string
            auto* varchar_ptr = (VarcharT*)(value_ptr);
            u16 varchar_len = varchar_ptr->length;
            if(varchar_len <= VarcharType::INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                memcpy(((VarcharT *) data_ptr_)[index].prefix, varchar_ptr->prefix, varchar_len);
            } else {
                memcpy(((VarcharT *) data_ptr_)[index].prefix, varchar_ptr->ptr, VarcharType::PREFIX_LENGTH);
                ptr_t ptr = this->buffer_->heap_mgr_->Allocate(varchar_len);
                memcpy(ptr, varchar_ptr->ptr, varchar_len);
                ((VarcharT *) data_ptr_)[index].ptr = ptr;
            }
            ((VarcharT *) data_ptr_)[index].length = varchar_len;
            break;
        }
        case kChar: {
            auto* char_ptr = (CharT*)(value_ptr);
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            memcpy(ptr, char_ptr->ptr, data_type_.Size());
            break;
        }
        case kDate: {
            ((DateT*)data_ptr_)[index] = *(DateT*)(value_ptr);
            break;
        }
        case kTime: {
            ((TimeT*)data_ptr_)[index] = *(TimeT*)(value_ptr);
            break;
        }
        case kDateTime: {
            ((DateTimeT*)data_ptr_)[index] = *(DateTimeT*)(value_ptr);
            break;
        }
        case kTimestamp: {
            ((TimestampT*)data_ptr_)[index] = *(TimestampT*)(value_ptr);
            break;
        }
        case kTimestampTZ: {
            ((TimestampTZT*)data_ptr_)[index] = *(TimestampTZT*)(value_ptr);
            break;
        }
        case kInterval: {
            ((IntervalT*)data_ptr_)[index] = *(IntervalT*)(value_ptr);
            break;
        }
        case kArray: {
            ((ArrayT*)data_ptr_)[index] = *(ArrayT*)(value_ptr);
            break;
        }
        case kTuple: {
            TypeError("Shouldn't store tuple directly, a tuple is flatten as many columns");
        }
        case kPoint: {
            ((PointT*)data_ptr_)[index] = *(PointT*)(value_ptr);
            break;
        }
        case kLine: {
            ((LineT*)data_ptr_)[index] = *(LineT*)(value_ptr);
            break;
        }
        case kLineSeg: {
            ((LineSegT*)data_ptr_)[index] = *(LineSegT*)(value_ptr);
            break;
        }
        case kBox: {
            ((BoxT*)data_ptr_)[index] = *(BoxT*)(value_ptr);
            break;
        }
        case kPath: {
            auto* point_ptr = (PathT*)(value_ptr);
            u32 point_count = point_ptr->point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            memcpy(ptr, point_ptr->ptr, point_area_size);

            // Why not use value.GetValue<PathT>(); ?
            // It will call PathT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PathT *) data_ptr_)[index].ptr = ptr;
            ((PathT *) data_ptr_)[index].point_count = point_count;
            ((PathT *) data_ptr_)[index].closed = point_ptr->closed;
            break;
        }
        case kPolygon: {
            auto* polygon_ptr = (PolygonT*)(value_ptr);
            u64 point_count = polygon_ptr->point_count;

            SizeT point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
            memcpy(ptr, polygon_ptr->ptr, point_area_size);

            // Why not use value.GetValue<PolygonT>(); ?
            // It will call PolygonT operator= to allocate new memory for point area.
            // In this case, we need the point area in memory vector buffer.
            ((PolygonT *) data_ptr_)[index].ptr = ptr;
            ((PolygonT *) data_ptr_)[index].point_count = point_count;
            ((PolygonT *) data_ptr_)[index].bounding_box = polygon_ptr->bounding_box;
            break;
        }
        case kCircle: {
            ((CircleT*)data_ptr_)[index] = *(CircleT*)(value_ptr);
            break;
        }
        case kBitmap: {
            auto* bitmap_ptr = (BitmapT*)(value_ptr);
            u64 bit_count = bitmap_ptr->count;
            u64 unit_count = BitmapT::UnitCount(bit_count);

            SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
            memcpy(ptr, (void*)(bitmap_ptr->ptr), bit_area_size);

            ((BitmapT *) data_ptr_)[index].ptr = (u64*)ptr;
            ((BitmapT *) data_ptr_)[index].count = bit_count;
            break;
        }
        case kUuid: {
            ((UuidT*)data_ptr_)[index] = *(UuidT*)(value_ptr);
            break;
        }
        case kBlob: {
            auto* blob_ptr = (BlobT*)(value_ptr);
            u64 blob_size = blob_ptr->size;
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(blob_size);
            memcpy(ptr, (void*)(blob_ptr->ptr), blob_size);

            ((BlobT *) data_ptr_)[index].ptr = ptr;
            ((BlobT *) data_ptr_)[index].size = blob_size;
            break;
        }
        case kEmbedding: {
            auto* embedding_ptr = (EmbeddingT*)(value_ptr);
            ptr_t ptr = data_ptr_ + index * data_type_.Size();
            memcpy(ptr, embedding_ptr->ptr, data_type_.Size());
            break;
        }
        case kMixed: {
            ((MixedT*)data_ptr_)[index] = *(MixedT*)(value_ptr);
            break;
        }
        default: {
            TypeError("Attempt to store an unaccepted type");
            // Null/Missing/Invalid
        }
    }
}

void
ColumnVector::AppendValue(const Value& value) {
    StorageAssert(initialized, "Column vector isn't initialized.")
    StorageAssert(tail_index_ < capacity_, "Exceed the column vector capacity.");
    SetValue(tail_index_ ++, value);
}

void
ColumnVector::AppendByPtr(const ptr_t value_ptr) {
    StorageAssert(initialized, "Column vector isn't initialized.")
    StorageAssert(tail_index_ < capacity_, "Exceed the column vector capacity.");
    SetByPtr(tail_index_ ++, value_ptr);
}

void
ColumnVector::ShallowCopy(const ColumnVector &other) {
    StorageAssert(this->data_type_ == other.data_type_,
                  "Attempt to shallow copy: " + other.data_type_.ToString() + "column vector to: " + this->data_type_.ToString());
    if(this->buffer_ != other.buffer_) {
        this->buffer_ = other.buffer_;
    }
    if(this->nulls_ptr_ != other.nulls_ptr_) {
        this->nulls_ptr_ = other.nulls_ptr_;
    }
    this->vector_type_ = other.vector_type_;
    this->data_ptr_ = other.data_ptr_;
    this->data_type_size_ = other.data_type_size_;
    this->initialized = other.initialized;
    this->capacity_ = other.capacity_;
    this->tail_index_ = other.tail_index_;
}

void
ColumnVector::Reserve(SizeT new_capacity) {
    StorageAssert(vector_type_ != ColumnVectorType::kConstant, "Constant column vector can only have one value")
    StorageAssert(initialized, "Column vector isn't initialized.")
    if(new_capacity <= capacity_) return ;

    SharedPtr<VectorBuffer> new_buffer = VectorBuffer::Make(data_type_size_, new_capacity, buffer_->buffer_type_);
    new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);
    new_buffer->heap_mgr_ = std::move(buffer_->heap_mgr_);
    buffer_ = new_buffer;

    capacity_ = new_capacity;
    data_ptr_ = buffer_->GetData();
}

void
ColumnVector::Reset() {
    // 1. Vector type is reset to invalid.
    vector_type_ = ColumnVectorType::kInvalid;

    // 2. Date type won't be changed.

    // 3. Since data type isn't change, data_type_size_ won't be changed, either.

    // 4. For trivial data type, the VectorBuffer will not be reset.
    // But for non-trivial data type, the heap memory manage need to be reset
    if(data_type_.type() == LogicalType::kMixed) {
        // Current solution:
        // Tuple/Array/Long String will use heap memory which isn't managed by ColumnVector.
        // This part of memory should managed by ColumnVector, but it isn't now.
        // So, when ColumnVector is destructed, this part need to free here.
        // TODO: we are going to manage the nested object in ColumnVector.
        for(SizeT idx = 0; idx < tail_index_; ++ idx) {
            ((MixedT *) data_ptr_)[idx].Reset();
        }
    }

//    buffer_.reset();
    if(buffer_ != nullptr) {
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

}
