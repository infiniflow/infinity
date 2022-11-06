//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "data_type.h"

namespace infinity {

struct Value {
// static method
public:
    static Value
    MakeBool(bool input);

    static Value
    MakeTinyInt(i8 input);

    static Value
    MakeSmallInt(i16 input);

    static Value
    MakeInt(i32 input);

    static Value
    MakeBigInt(i64 input);

    static Value
    MakeHugeInt(i64 low, i64 high);

    static Value
    MakeFloat(float input);

    static Value
    MakeDouble(double input);

    static Value
    MakeDecimal(i64 low, i64 high, i8 precision, i8 scale);

    static Value
    MakeVarchar(const String& str_ref);

    static Value
    MakeDate(i32 input);

    static Value
    MakeTime(i32 input);

    static Value
    MakeDateTime(i32 date, i32 time);

    static Value
    MakeTimestamp(i64 input);

    static Value
    MakeTimestampTz(i64 input);

    static Value
    MakeInterval(i64 input);

    static Value
    MakeArray(const ArrayType& array_ref);

    static Value
    MakePoint(PointType point);

    static Value
    MakeLine(LineType line);

    static Value
    MakeLineSegment(LineSegmentType line);

    static Value
    MakeBox(BoxType box);

    static Value
    MakePolygon(PolygonType polygon);

    static Value
    MakeCircle(CircleType circle);

    static Value
    MakeBitmap(BitmapType bitmap);

    static Value
    MakeUuid(UuidType uuid);

    static Value
    MakeBlob(BlobType blob);

    static Value
    MakeEmbedding(EmbeddingType blob);

    static Value
    MakeMixedData(MixedT blob);

public:
    DataType type_;
    union {
        BooleanT boolean;

        TinyIntT tiny_int;
        SmallIntT small_int;
        IntegerT integer;
        BigIntT  big_int;
        HugeIntT huge_int;
        FloatT   real32;
        DoubleT  real64;
        DecimalT decimal;

        VarcharT varchar;

        DateT    date;
        TimeT    time;
        DateTimeT datetime;
        TimestampT timestamp;
        TimestampTZT  timestamp_tz;
        IntervalT  interval;

        ArrayT     array;
        TupleT     tuple; // No such value, a tuple will be resolved as other value.

        PointT     point;
        LineT      line;
        LineSegT   line_segment;
        BoxT       box;
        PolygonT   polygon;
        CircleT    circle;

        BitmapT    bitmap;
        UuidT      uuid;
        BlobT      blob;
        EmbeddingT embedding;

        MixedT     mixed_value;
    } value_;
    bool null_{false};
};

}
