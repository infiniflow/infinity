//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct Value {
// static method
public:
    // Value creator from different type of input

    static Value
    MakeBool(BooleanT input);

    static Value
    MakeTinyInt(TinyIntT input);

    static Value
    MakeSmallInt(SmallIntT input);

    static Value
    MakeInt(IntegerT input);

    static Value
    MakeBigInt(BigIntT input);

    static Value
    MakeHugeInt(HugeIntT input);

    static Value
    MakeFloat(FloatT input);

    static Value
    MakeDouble(DoubleT input);

    static Value
    MakeDecimal(DecimalT input);

    static Value
    MakeVarchar(VarcharT input_ref);

    static Value
    MakeDate(DateT input);

    static Value
    MakeTime(TimeT input);

    static Value
    MakeDateTime(DateTimeT input);

    static Value
    MakeTimestamp(TimestampT input);

    static Value
    MakeTimestampTz(TimestampTZT input);

    static Value
    MakeInterval(IntervalT input);

    static Value
    MakeArray(ArrayT input);

    static Value
    MakePoint(PointT input);

    static Value
    MakeLine(LineT input);

    static Value
    MakeLineSegment(LineSegT input);

    static Value
    MakeBox(BoxT input);

    static Value
    MakePolygon(PolygonT input);

    static Value
    MakeCircle(CircleT input);

    static Value
    MakeBitmap(BitmapT input);

    static Value
    MakeUuid(UuidT input);

    static Value
    MakeBlob(BlobT input);

    static Value
    MakeEmbedding(EmbeddingT input);

    static Value
    MakeMixedData(MixedT input);

    // Value maker template
    template <class T>
    static Value MakeValue(T value) {
        TypeError("Not implemented value maker.");
    }

    // Value getter template
    template <class T>
    T GetValue() const {
        TypeError("Not implemented value getter.");
    }

// Member method
public:
//    Value() = default;
    explicit Value(LogicalType type): type_(type) {}

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

// Value maker
template <> Value Value::MakeValue(BooleanT input) { return MakeBool(input); }
template <> Value Value::MakeValue(TinyIntT input) { return MakeBool(input); }
template <> Value Value::MakeValue(SmallIntT input) { return MakeBool(input); }
template <> Value Value::MakeValue(IntegerT input) { return MakeBool(input); }
template <> Value Value::MakeValue(BigIntT input) { return MakeBool(input); }
template <> Value Value::MakeValue(HugeIntT input) { return MakeHugeInt(input); }
template <> Value Value::MakeValue(FloatT input) { return MakeFloat(input); }
template <> Value Value::MakeValue(DoubleT input) { return MakeDouble(input); }
template <> Value Value::MakeValue(DecimalT input) { return MakeDecimal(input); }
template <> Value Value::MakeValue(VarcharT input) { return MakeVarchar(input); }
template <> Value Value::MakeValue(DateT input) { return MakeDate(input); }
template <> Value Value::MakeValue(TimeT input) { return MakeTime(input); }
template <> Value Value::MakeValue(DateTimeT input) { return MakeDateTime(input); }
template <> Value Value::MakeValue(TimestampT input) { return MakeTimestamp(input); }
template <> Value Value::MakeValue(TimestampTZT input) { return MakeTimestampTz(input); }
template <> Value Value::MakeValue(IntervalT input) { return MakeInterval(input); }
template <> Value Value::MakeValue(ArrayT input) { return MakeArray(input); }
template <> Value Value::MakeValue(PointT input) { return MakePoint(input); }
template <> Value Value::MakeValue(LineT input) { return MakeLine(input); }
template <> Value Value::MakeValue(LineSegT input) { return MakeLineSegment(input); }
template <> Value Value::MakeValue(BoxT input) { return MakeBox(input); }
template <> Value Value::MakeValue(PolygonT input) { return MakePolygon(input); }
template <> Value Value::MakeValue(CircleT input) { return MakeCircle(input); }
template <> Value Value::MakeValue(BitmapT input) { return MakeBitmap(input); }
template <> Value Value::MakeValue(UuidT input) { return MakeUuid(input); }
template <> Value Value::MakeValue(BlobT input) { return MakeBlob(input); }
template <> Value Value::MakeValue(EmbeddingT input) { return MakeEmbedding(input); }
template <> Value Value::MakeValue(MixedT input) { return MakeMixedData(input); }

// Value getter
template <> BooleanT Value::GetValue() const;
template <> TinyIntT Value::GetValue() const;
template <> SmallIntT Value::GetValue() const;
template <> IntegerT Value::GetValue() const;
template <> BigIntT Value::GetValue() const;
template <> HugeIntT Value::GetValue() const;
template <> FloatT Value::GetValue() const;
template <> DoubleT Value::GetValue() const;
template <> DecimalT Value::GetValue() const;
template <> VarcharT Value::GetValue() const;
template <> DateT Value::GetValue() const;
template <> TimeT Value::GetValue() const;
template <> DateTimeT Value::GetValue() const;
template <> TimestampT Value::GetValue() const;
template <> TimestampTZT Value::GetValue() const;
template <> IntervalT Value::GetValue() const;
template <> ArrayT Value::GetValue() const;
template <> PointT Value::GetValue() const;
template <> LineT Value::GetValue() const;
template <> LineSegT Value::GetValue() const;
template <> BoxT Value::GetValue() const;
template <> PolygonT Value::GetValue() const;
template <> CircleT Value::GetValue() const;
template <> BitmapT Value::GetValue() const;
template <> UuidT Value::GetValue() const;
template <> BlobT Value::GetValue() const;
template <> EmbeddingT Value::GetValue() const;
template <> MixedT Value::GetValue() const;
}
