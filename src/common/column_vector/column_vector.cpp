//
// Created by JinHai on 2022/10/30.
//

#include "column_vector.h"

#include <utility>
#include "common/utility/infinity_assert.h"

namespace infinity {

void ColumnVector::Initialize(size_t capacity) {
    GeneralAssert(!initialized, "Column vector is already initialized.")
    GeneralAssert(data_type_.type() != LogicalType::kInvalid, "Data type isn't set.")
    GeneralAssert(vector_type_ != ColumnVectorType::kInvalid, "Column Vector type isn't set.")
    capacity_ = capacity;
    tail_index_ = 0;
    data_type_size_ = data_type_.Size();
    switch(data_type_.type()) {
        case LogicalType::kBlob:
        case LogicalType::kBitmap:
        case LogicalType::kPolygon:
        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            buffer_ = MemoryVectorBuffer::Make(data_type_size_, capacity_);
            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            GeneralError("Unexpected data type for column vector.")
        }
        default: {
            buffer_ = VectorBuffer::Make(data_type_size_, capacity_);
        }
    }
    data_ptr_ = buffer_->GetData();
    initialized = true;
}

String
ColumnVector::ToString() const {
    GeneralError("Not implemented");
}

Value
ColumnVector::GetValue(idx_t index) const {
    if(index >= tail_index_) {
        GeneralError("Attempt to access an invalid index of column vector.");
    }

    // TODO: Check the validity
    // TODO: If it is null / missing, return null/missing Value ?

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
        case kChar1: {
            return Value::MakeChar1(((Char1T *) data_ptr_)[index]);
        }
        case kChar2: {
            return Value::MakeChar2(((Char2T *) data_ptr_)[index]);
        }
        case kChar4: {
            return Value::MakeChar4(((Char4T *) data_ptr_)[index]);
        }
        case kChar8: {
            return Value::MakeChar8(((Char8T *) data_ptr_)[index]);
        }
        case kChar16: {
            return Value::MakeChar16(((Char16T *) data_ptr_)[index]);
        }
        case kChar32: {
            return Value::MakeChar32(((Char32T *) data_ptr_)[index]);
        }
        case kChar64: {
            return Value::MakeChar64(((Char64T *) data_ptr_)[index]);
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
ColumnVector::SetValue(idx_t index, const Value &value) {
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
            auto* string_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());

            // Copy string
            size_t varchar_len = value.value_.varchar.length;
            if(varchar_len <= VarcharType::INLINE_LENGTH) {
                // Only prefix is enough to contain all string data.
                memcpy(((VarcharT *) data_ptr_)[index].prefix, value.value_.varchar.prefix, varchar_len);
            } else {
                memcpy(((VarcharT *) data_ptr_)[index].prefix, value.value_.varchar.ptr, VarcharType::PREFIX_LENGTH);
                ptr_t ptr = string_vector_buffer_ptr->chunk_mgr_->Allocate(varchar_len);
                memcpy(ptr, value.value_.varchar.ptr, varchar_len);
                ((VarcharT *) data_ptr_)[index].ptr = ptr;
            }
            ((VarcharT *) data_ptr_)[index].length = static_cast<i16>(varchar_len);
            break;
        }
        case kChar1: {
            ((Char1T *) data_ptr_)[index] = value.GetValue<Char1T>();
            break;
        }
        case kChar2: {
            ((Char2T *) data_ptr_)[index] = value.GetValue<Char2T>();
            break;
        }
        case kChar4: {
            ((Char4T *) data_ptr_)[index] = value.GetValue<Char4T>();
            break;
        }
        case kChar8: {
            ((Char8T *) data_ptr_)[index] = value.GetValue<Char8T>();
            break;
        }
        case kChar16: {
            ((Char16T *) data_ptr_)[index] = value.GetValue<Char16T>();
            break;
        }
        case kChar32: {
            ((Char32T *) data_ptr_)[index] = value.GetValue<Char32T>();
            break;
        }
        case kChar64: {
            ((Char64T *) data_ptr_)[index] = value.GetValue<Char64T>();
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
            auto* path_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());
            u32 point_count = value.value_.path.point_count;

            size_t point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = path_vector_buffer_ptr->chunk_mgr_->Allocate(point_area_size);
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

            auto* polygon_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());
            u64 point_count = value.value_.polygon.point_count;

            size_t point_area_size = point_count * sizeof(PointT);
            ptr_t ptr = polygon_vector_buffer_ptr->chunk_mgr_->Allocate(point_area_size);
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
            auto* bitmap_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());
            u64 bit_count = value.value_.bitmap.count;
            u64 unit_count = BitmapT::UnitCount(bit_count);

            size_t bit_area_size = unit_count * BitmapT::UNIT_BYTES;
            ptr_t ptr = bitmap_vector_buffer_ptr->chunk_mgr_->Allocate(bit_area_size);
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
            auto* blob_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());
            ptr_t ptr = blob_vector_buffer_ptr->chunk_mgr_->Allocate(blob_size);
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
ColumnVector::AppendValue(const Value& value) {
    GeneralAssert(tail_index_ < capacity_, "Exceed the column vector capacity.");
    SetValue(tail_index_ ++, value);
}

void
ColumnVector::ShallowCopy(const ColumnVector &other) {
    GeneralError("Not implemented");
}

void
ColumnVector::Reserve(size_t new_capacity) {
    if(new_capacity <= capacity_) return ;
    switch(data_type_.type()) {
        case LogicalType::kBlob:
        case LogicalType::kBitmap:
        case LogicalType::kPolygon:
        case LogicalType::kPath:
        case LogicalType::kVarchar: {
            auto* string_vector_buffer_ptr = (MemoryVectorBuffer*)(this->buffer_.get());
            SharedPtr<MemoryVectorBuffer> new_buffer = MemoryVectorBuffer::Make(data_type_size_, new_capacity);

            // Copy the string header information.
            new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);

            // Move the string chunks.
            new_buffer->chunk_mgr_ = std::move(string_vector_buffer_ptr->chunk_mgr_);
            buffer_ = new_buffer;
            break;
        }
        case LogicalType::kInvalid:
        case LogicalType::kNull:
        case LogicalType::kMissing: {
            GeneralError("Unexpected data type for column vector.")
        }
        default: {
            SharedPtr<VectorBuffer> new_buffer = VectorBuffer::Make(data_type_size_, new_capacity);
            new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);
            buffer_ = new_buffer;
        }
    }

    capacity_ = new_capacity;
    data_ptr_ = buffer_->GetData();
}

void
ColumnVector::Reset() {

    if(data_type_.type() == LogicalType::kMixed) {
        // Current solution:
        // Tuple/Array/Long String will use heap memory which isn't managed by ColumnVector.
        // This part of memory should managed by ColumnVector, but it isn't now.
        // So, when ColumnVector is destructed, this part need to free here.
        // TODO: we are going to manage the nested object in ColumnVector.
        for(size_t idx = 0; idx < tail_index_; ++ idx) {
            ((MixedT *) data_ptr_)[idx].Reset();
        }
    }

    capacity_ = 0;
    tail_index_ = 0;
//    data_type_size_ = 0;
    buffer_.reset();
    data_ptr_ = nullptr;
    initialized = false;


//    data_type_ = DataType(LogicalType::kInvalid);
//    vector_type_ = ColumnVectorType::kInvalid;
}

}
