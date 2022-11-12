//
// Created by JinHai on 2022/11/7.
//

#include "value.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// Value maker

Value
Value::MakeBool(BooleanT input) {
    Value value(LogicalType::kBoolean);
    value.value_.boolean = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeTinyInt(TinyIntT input) {
    Value value(LogicalType::kTinyInt);
    value.value_.tiny_int = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeSmallInt(SmallIntT input) {
    Value value(LogicalType::kSmallInt);
    value.value_.small_int = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeInt(IntegerT input) {
    Value value(LogicalType::kInteger);
    value.value_.integer = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeBigInt(BigIntT input) {
    Value value(LogicalType::kBigInt);
    value.value_.big_int = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeHugeInt(HugeIntT input) {
    Value value(LogicalType::kHugeInt);
    value.value_.huge_int = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeFloat(FloatT input) {
    Value value(LogicalType::kFloat);
    value.value_.real32 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDouble(DoubleT input) {
    Value value(LogicalType::kDouble);
    value.value_.real64 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal16(Decimal16T input) {
    Value value(LogicalType::kDecimal16);
    value.value_.decimal16 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal32(Decimal32T input) {
    Value value(LogicalType::kDecimal32);
    value.value_.decimal32 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal64(Decimal64T input) {
    Value value(LogicalType::kDecimal64);
    value.value_.decimal64 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal128(Decimal128T input) {
    Value value(LogicalType::kDecimal128);
    value.value_.decimal128 = input;
    value.is_null_ = false;
    return value;
}


Value
Value::MakeVarchar(VarcharT input) {
    Value value(LogicalType::kVarchar);
    value.value_.varchar = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar1(Char1T input) {
    Value value(LogicalType::kChar1);
    value.value_.char1 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar2(Char2T input) {
    Value value(LogicalType::kChar2);
    value.value_.char2 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar4(Char4T input) {
    Value value(LogicalType::kChar4);
    value.value_.char4 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar8(Char8T input) {
    Value value(LogicalType::kChar8);
    value.value_.char8 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar16(Char16T input) {
    Value value(LogicalType::kChar16);
    value.value_.char16 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar32(Char32T input) {
    Value value(LogicalType::kChar32);
    value.value_.char32 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar64(Char64T input) {
    Value value(LogicalType::kChar64);
    value.value_.char64 = input;
    value.is_null_ = false;
    return value;
}


Value
Value::MakeDate(DateT input) {
    Value value(LogicalType::kDate);
    value.value_.date = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeTime(TimeT input) {
    Value value(LogicalType::kTime);
    value.value_.time = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDateTime(DateTimeT input) {
    Value value(LogicalType::kDateTime);
    value.value_.datetime = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeTimestamp(TimestampT input) {
    Value value(LogicalType::kTimestamp);
    value.value_.timestamp = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeTimestampTz(TimestampTZT input) {
    Value value(LogicalType::kTimestampTZ);
    value.value_.timestamp_tz = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeInterval(IntervalT input) {
    Value value(LogicalType::kInterval);
    value.value_.interval = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeArray(ArrayT input) {
    Value value(LogicalType::kArray);
    value.value_.array = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakePoint(PointT input) {
    Value value(LogicalType::kPoint);
    value.value_.point = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeLine(LineT input) {
    Value value(LogicalType::kLine);
    value.value_.line = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeLineSegment(LineSegT input) {
    Value value(LogicalType::kLineSeg);
    value.value_.line_segment = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeBox(BoxT input) {
    Value value(LogicalType::kBox);
    value.value_.box = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakePath(PathT input) {
    Value value(LogicalType::kPath);
    value.value_.path = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakePolygon(PolygonT input) {
    Value value(LogicalType::kPolygon);
    value.value_.polygon = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeCircle(CircleT input) {
    Value value(LogicalType::kCircle);
    value.value_.circle = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeBitmap(BitmapT input) {
    Value value(LogicalType::kBitmap);
    value.value_.bitmap = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeUuid(UuidT input) {
    Value value(LogicalType::kUuid);
    value.value_.uuid = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeBlob(BlobT input) {
    Value value(LogicalType::kBlob);
    value.value_.blob = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeEmbedding(EmbeddingT input) {
    Value value(LogicalType::kEmbedding);
    value.value_.embedding = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeMixedData(MixedT input) {
    Value value(LogicalType::kMixed);
    value.value_.mixed_value = input;
    value.is_null_ = false;
    return value;
}

// Value getter
template <> BooleanT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBoolean, "Not matched type: " + type_.ToString());
    return value_.boolean;
}
template <> TinyIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTinyInt, "Not matched type: " + type_.ToString());
    return value_.tiny_int;
}

template <> SmallIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kSmallInt, "Not matched type: " + type_.ToString());
    return value_.small_int;
}

template <> IntegerT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kInteger, "Not matched type: " + type_.ToString());
    return value_.integer;
}

template <> BigIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBigInt, "Not matched type: " + type_.ToString());
    return value_.big_int;
}

template <> HugeIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kHugeInt, "Not matched type: " + type_.ToString());
    return value_.huge_int;
}

template <> FloatT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kFloat, "Not matched type: " + type_.ToString());
    return value_.real32;
}

template <> DoubleT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDouble, "Not matched type: " + type_.ToString());
    return value_.real64;
}

template <> Decimal16T
Value::GetValue() const {
TypeAssert(type_.type() == LogicalType::kDecimal16, "Not matched type: " + type_.ToString());
return value_.decimal16;
}

template <> Decimal32T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal32, "Not matched type: " + type_.ToString());
    return value_.decimal32;
}

template <> Decimal64T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal64, "Not matched type: " + type_.ToString());
    return value_.decimal64;
}

template <> Decimal128T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal128, "Not matched type: " + type_.ToString());
    return value_.decimal128;
}

template <> VarcharT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kVarchar, "Not matched type: " + type_.ToString());
    return value_.varchar;
}

template <> Char1T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar1, "Not matched type: " + type_.ToString());
    return value_.char1;
}

template <> Char2T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar2, "Not matched type: " + type_.ToString());
    return value_.char2;
}

template <> Char4T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar4, "Not matched type: " + type_.ToString());
    return value_.char4;
}

template <> Char8T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar8, "Not matched type: " + type_.ToString());
    return value_.char8;
}

template <> Char16T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar16, "Not matched type: " + type_.ToString());
    return value_.char16;
}

template <> Char32T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar32, "Not matched type: " + type_.ToString());
    return value_.char32;
}

template <> Char64T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar64, "Not matched type: " + type_.ToString());
    return value_.char64;
}

template <> DateT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDate, "Not matched type: " + type_.ToString());
    return value_.date;
}

template <> TimeT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTime, "Not matched type: " + type_.ToString());
    return value_.time;
}

template <> DateTimeT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDateTime, "Not matched type: " + type_.ToString());
    return value_.datetime;
}

template <> TimestampT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTimestamp, "Not matched type: " + type_.ToString());
    return value_.timestamp;
}

template <> TimestampTZT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTimestampTZ, "Not matched type: " + type_.ToString());
    return value_.timestamp_tz;
}

template <> IntervalT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kInterval, "Not matched type: " + type_.ToString());
    return value_.interval;
}

template <> ArrayT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kArray, "Not matched type: " + type_.ToString());
    return value_.array;
}

template <> PointT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPoint, "Not matched type: " + type_.ToString());
    return value_.point;
}

template <> LineT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kLine, "Not matched type: " + type_.ToString());
    return value_.line;
}

template <> LineSegT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kLineSeg, "Not matched type: " + type_.ToString());
    return value_.line_segment;
}

template <> BoxT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBox, "Not matched type: " + type_.ToString());
    return value_.box;
}

template <> PathT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPath, "Not matched type: " + type_.ToString());
    return value_.path;
}

template <> PolygonT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPolygon, "Not matched type: " + type_.ToString());
    return value_.polygon;
}

template <> CircleT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kCircle, "Not matched type: " + type_.ToString());
    return value_.circle;
}

template <> BitmapT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBitmap, "Not matched type: " + type_.ToString());
    return value_.bitmap;
}

template <> UuidT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kUuid, "Not matched type: " + type_.ToString());
    return value_.uuid;
}

template <> BlobT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBlob, "Not matched type: " + type_.ToString());
    return value_.blob;
}

template <> EmbeddingT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kEmbedding, "Not matched type: " + type_.ToString());
    return value_.embedding;
}

template <> MixedT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kMixed, "Not matched type: " + type_.ToString());
    return value_.mixed_value;
}

template <> Value
Value::MakeValue(BooleanT input) {
    return MakeBool(input);
}

template <> Value
Value::MakeValue(TinyIntT input) {
    return MakeBool(input);
}

template <> Value
Value::MakeValue(SmallIntT input) {
    return MakeBool(input);
}

template <> Value
Value::MakeValue(IntegerT input) {
    return MakeBool(input);
}

template <> Value
Value::MakeValue(BigIntT input) {
    return MakeBool(input);
}

template <> Value
Value::MakeValue(HugeIntT input) {
    return MakeHugeInt(input);
}

template <> Value
Value::MakeValue(FloatT input) {
    return MakeFloat(input);
}

template <> Value
Value::MakeValue(DoubleT input) {
    return MakeDouble(input);
}

template <> Value
Value::MakeValue(Decimal16T input) {
    return MakeDecimal16(input);
}

template <> Value
Value::MakeValue(Decimal32T input) {
    return MakeDecimal32(input);
}

template <> Value
Value::MakeValue(Decimal64T input) {
    return MakeDecimal64(input);
}

template <> Value
Value::MakeValue(Decimal128T input) {
    return MakeDecimal128(input);
}

template <> Value
Value::MakeValue(VarcharT input) {
    return MakeVarchar(input);
}

template <> Value Value::MakeValue(Char1T input) {
    return MakeChar1(input);
}

template <> Value Value::MakeValue(Char2T input) {
    return MakeChar2(input);
}

template <> Value Value::MakeValue(Char4T input) {
    return MakeChar4(input);
}

template <> Value Value::MakeValue(Char8T input) {
    return MakeChar8(input);
}

template <> Value Value::MakeValue(Char16T input) {
    return MakeChar16(input);
}

template <> Value Value::MakeValue(Char32T input) {
    return MakeChar32(input);
}

template <> Value Value::MakeValue(Char64T input) {
    return MakeChar64(input);
}

template <> Value
Value::MakeValue(DateT input) {
    return MakeDate(input);
}

template <> Value
Value::MakeValue(TimeT input) {
    return MakeTime(input);
}

template <> Value
Value::MakeValue(DateTimeT input) {
    return MakeDateTime(input);
}

template <> Value
Value::MakeValue(TimestampT input) {
    return MakeTimestamp(input);
}

template <> Value
Value::MakeValue(TimestampTZT input) {
    return MakeTimestampTz(input);
}

template <> Value
Value::MakeValue(IntervalT input) {
    return MakeInterval(input);
}

template <> Value
Value::MakeValue(ArrayT input) {
    return MakeArray(input);
}

template <> Value
Value::MakeValue(PointT input) {
    return MakePoint(input);
}

template <> Value
Value::MakeValue(LineT input) {
    return MakeLine(input);
}

template <> Value
Value::MakeValue(LineSegT input) {
    return MakeLineSegment(input);
}

template <> Value
Value::MakeValue(BoxT input) {
    return MakeBox(input);
}

template <> Value
Value::MakeValue(PathT input) {
    return MakePath(input);
}

template <> Value
Value::MakeValue(PolygonT input) {
    return MakePolygon(input);
}

template <> Value
Value::MakeValue(CircleT input) {
    return MakeCircle(input);
}

template <> Value
Value::MakeValue(BitmapT input) {
    return MakeBitmap(input);
}

template <> Value
Value::MakeValue(UuidT input) {
    return MakeUuid(input);
}

template <> Value
Value::MakeValue(BlobT input) {
    return MakeBlob(input);
}

template <> Value
Value::MakeValue(EmbeddingT input) {
    return MakeEmbedding(input);
}

template <> Value
Value::MakeValue(MixedT input) {
    return MakeMixedData(input);
}

}
