//
// Created by JinHai on 2022/11/7.
//

#include "value.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

Value Value::MakeBool(bool input) {
    TypeError("Not implemented");
}

Value Value::MakeTinyInt(i8 input) {
    TypeError("Not implemented");
}

Value Value::MakeSmallInt(i16 input) {
    TypeError("Not implemented");
}

Value Value::MakeInt(i32 input) {
    TypeError("Not implemented");
}

Value Value::MakeBigInt(i64 input) {
    TypeError("Not implemented");
}

Value Value::MakeHugeInt(i64 low, i64 high) {
    TypeError("Not implemented");
}

Value Value::MakeFloat(float input) {
    TypeError("Not implemented");
}

Value Value::MakeDouble(double input) {
    TypeError("Not implemented");
}

Value Value::MakeDecimal(i64 low, i64 high, i8 precision, i8 scale) {
    TypeError("Not implemented");
}

Value Value::MakeVarchar(const String &str_ref) {
    TypeError("Not implemented");
}

Value Value::MakeDate(i32 input) {
    TypeError("Not implemented");
}

Value Value::MakeTime(i32 input) {
    TypeError("Not implemented");
}

Value Value::MakeDateTime(i32 date, i32 time) {
    TypeError("Not implemented");
}

Value Value::MakeTimestamp(i64 input) {
    TypeError("Not implemented");
}

Value Value::MakeTimestampTz(i64 input) {
    TypeError("Not implemented");
}

Value Value::MakeInterval(i64 input) {
    TypeError("Not implemented");
}

Value Value::MakeArray(const ArrayType &array_ref) {
    TypeError("Not implemented");
}

Value Value::MakePoint(PointType point) {
    TypeError("Not implemented");
}

Value Value::MakeLine(LineType line) {
    TypeError("Not implemented");
}

Value Value::MakeLineSegment(LineSegmentType line) {
    TypeError("Not implemented");
}

Value Value::MakeBox(BoxType box) {
    TypeError("Not implemented");
}

Value Value::MakePolygon(PolygonType polygon) {
    TypeError("Not implemented");
}

Value Value::MakeCircle(CircleType circle) {
    TypeError("Not implemented");
}

Value Value::MakeBitmap(BitmapType bitmap) {
    TypeError("Not implemented");
}

Value Value::MakeUuid(UuidType uuid) {
    TypeError("Not implemented");
}

Value Value::MakeBlob(BlobType blob) {
    TypeError("Not implemented");
}

Value Value::MakeEmbedding(EmbeddingType blob) {
    TypeError("Not implemented");
}

Value Value::MakeMixedData(MixedT blob) {
    TypeError("Not implemented");
}

}
