//
// Created by JinHai on 2022/11/7.
//

#include "value.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// Value maker

Value Value::MakeBool(bool input) {
    Value value(LogicalType::kBoolean);
    value.value_.boolean = input;
    value.null_ = false;
    return value;
}

Value Value::MakeTinyInt(i8 input) {
    Value value(LogicalType::kTinyInt);
    value.value_.tiny_int = input;
    value.null_ = false;
    return value;
}

Value Value::MakeSmallInt(i16 input) {
    Value value(LogicalType::kSmallInt);
    value.value_.small_int = input;
    value.null_ = false;
    return value;
}

Value Value::MakeInt(i32 input) {
    Value value(LogicalType::kInteger);
    value.value_.integer = input;
    value.null_ = false;
    return value;
}

Value Value::MakeBigInt(i64 input) {
    Value value(LogicalType::kBigInt);
    value.value_.big_int = input;
    value.null_ = false;
    return value;
}

Value Value::MakeHugeInt(HugeIntT input) {
    Value value(LogicalType::kHugeInt);
    value.value_.huge_int = input;
    value.null_ = false;
    return value;
}

Value Value::MakeFloat(float input) {
    Value value(LogicalType::kFloat);
    value.value_.real32 = input;
    value.null_ = false;
    return value;
}

Value Value::MakeDouble(double input) {
    Value value(LogicalType::kDouble);
    value.value_.real64 = input;
    value.null_ = false;
    return value;
}

Value Value::MakeDecimal(DecimalType input) {
    TypeError("Not implemented");
}

Value Value::MakeVarchar(VarcharT input) {
    TypeError("Not implemented");
}

Value Value::MakeDate(DateT input) {
    TypeError("Not implemented");
}

Value Value::MakeTime(TimeT input) {
    TypeError("Not implemented");
}

Value Value::MakeDateTime(DateTimeT input) {
    TypeError("Not implemented");
}

Value Value::MakeTimestamp(TimestampT input) {
    TypeError("Not implemented");
}

Value Value::MakeTimestampTz(TimestampTZT input) {
    TypeError("Not implemented");
}

Value Value::MakeInterval(IntervalT input) {
    TypeError("Not implemented");
}

Value Value::MakeArray(ArrayT input) {
    TypeError("Not implemented");
}

Value Value::MakePoint(PointT input) {
    TypeError("Not implemented");
}

Value Value::MakeLine(LineT input) {
    TypeError("Not implemented");
}

Value Value::MakeLineSegment(LineSegT input) {
    TypeError("Not implemented");
}

Value Value::MakeBox(BoxT input) {
    TypeError("Not implemented");
}

Value Value::MakePolygon(PolygonT input) {
    TypeError("Not implemented");
}

Value Value::MakeCircle(CircleT input) {
    TypeError("Not implemented");
}

Value Value::MakeBitmap(BitmapT input) {
    TypeError("Not implemented");
}

Value Value::MakeUuid(UuidT input) {
    TypeError("Not implemented");
}

Value Value::MakeBlob(BlobT input) {
    TypeError("Not implemented");
}

Value Value::MakeEmbedding(EmbeddingT input) {
    TypeError("Not implemented");
}

Value Value::MakeMixedData(MixedT input) {
    TypeError("Not implemented");
}

// Value getter
template <> BooleanT
Value::GetValue() const {
    TypeError("Not implemented");
}
template <> TinyIntT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> SmallIntT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> IntegerT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> BigIntT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> HugeIntT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> FloatT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> DoubleT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> DecimalT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> VarcharT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> DateT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> TimeT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> DateTimeT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> TimestampT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> TimestampTZT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> IntervalT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> ArrayT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> PointT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> LineT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> LineSegT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> BoxT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> PolygonT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> CircleT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> BitmapT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> UuidT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> BlobT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> EmbeddingT
Value::GetValue() const {
    TypeError("Not implemented");
}

template <> MixedT
Value::GetValue() const {
    TypeError("Not implemented");
}


}
