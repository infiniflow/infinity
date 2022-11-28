//
// Created by JinHai on 2022/10/30.
//

#include "column_vector.h"

#include <utility>
#include "common/utility/infinity_assert.h"

namespace infinity {

void ColumnVector::Initialize(size_t capacity) {
    GeneralAssert(!initialized, "Column vector is already initialized.")
    capacity_ = capacity;
    tail_index_ = 0;
    data_type_size_ = data_type_.Size();
    buffer_ = VectorBuffer::Make(data_type_size_, capacity_);
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
            return Value::MakeDecimal16(((Decimal16T *) data_ptr_)[index]);
        }
        case kDecimal32: {
            return Value::MakeDecimal32(((Decimal32T *) data_ptr_)[index]);
        }
        case kDecimal64: {
            return Value::MakeDecimal64(((Decimal64T *) data_ptr_)[index]);
        }
        case kDecimal128: {
            return Value::MakeDecimal128(((Decimal128T *) data_ptr_)[index]);
        }
        case kVarchar: {
            return Value::MakeVarchar(((VarcharT *) data_ptr_)[index]);
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
            return Value::MakeEmbedding(((ptr_t *) data_ptr_)[index]);
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
            // TODO: store string data into sequential address?
            ((VarcharT *) data_ptr_)[index] = value.GetValue<VarcharT>();
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
            ((PathT *) data_ptr_)[index] = value.GetValue<PathT>();
            break;
        }
        case kPolygon: {
            ((PolygonT *) data_ptr_)[index] = value.GetValue<PolygonT>();
            break;
        }
        case kCircle: {
            ((CircleT *) data_ptr_)[index] = value.GetValue<CircleT>();
            break;
        }
        case kBitmap: {
            ((BitmapT *) data_ptr_)[index] = value.GetValue<BitmapT>();
            break;
        }
        case kUuid: {
            ((UuidT *) data_ptr_)[index] = value.GetValue<UuidT>();
            break;
        }
        case kBlob: {
            ((BlobT *) data_ptr_)[index] = value.GetValue<BlobT>();
            break;
        }
        case kEmbedding: {
            EmbeddingT embedding = value.GetValue<EmbeddingT>();
            memcpy(data_ptr_, embedding.ptr, data_type_.Size());
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
    SharedPtr<VectorBuffer> new_buffer = VectorBuffer::Make(data_type_size_, new_capacity);
    new_buffer->Copy(data_ptr_, data_type_size_ * tail_index_);
    capacity_ = new_capacity;
    buffer_ = new_buffer;
}

void
ColumnVector::Reset() {
    capacity_ = 0;
    tail_index_ = 0;
    data_type_size_ = 0;
    buffer_.reset();
    data_ptr_ = nullptr;
    initialized = false;
}

}
