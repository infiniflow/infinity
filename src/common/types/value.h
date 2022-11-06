//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "data_type.h"

namespace infinity {

struct Value {
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
        TupleT     tuple;

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
};

}
