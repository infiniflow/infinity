//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct Value {
// class member
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
    MakeDecimal16(Decimal16T input);

    static Value
    MakeDecimal32(Decimal32T input);

    static Value
    MakeDecimal64(Decimal64T input);

    static Value
    MakeDecimal128(Decimal128T input);

    static Value
    MakeVarchar(VarcharT& input_ref);

    static Value
    MakeVarchar(const String& str);

    static Value
    MakeVarchar(const char* ptr);

    static Value
    MakeChar1(Char1T input_ref);

    static Value
    MakeChar2(Char2T input_ref);

    static Value
    MakeChar4(Char4T input_ref);

    static Value
    MakeChar8(Char8T input_ref);

    static Value
    MakeChar15(Char15T input_ref);

    static Value
    MakeChar31(Char31T input_ref);

    static Value
    MakeChar63(Char63T input_ref);

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
    MakePath(PathT input);

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

    // Object member
public:
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

    [[nodiscard]] const DataType& type() const { return type_; }
    [[nodiscard]] bool is_null() const { return is_null_; }

    [[nodiscard]] String
    ToString() const;
    // Member method
public:
//    Value() = default;
    explicit
    Value(LogicalType type): type_(type) {}

    Value(const Value& other);
//
//    Value(Value&& other) noexcept ;
//
//    Value&
//    operator=(const Value& other);

    Value&
    operator=(Value&& other) noexcept ;

    ~Value();

public:
    DataType type_;
    union UnionValue {
        BooleanT boolean;

        TinyIntT tiny_int;
        SmallIntT small_int;
        IntegerT integer;
        BigIntT  big_int;
        HugeIntT huge_int;
        FloatT   float32;
        DoubleT  float64;
        Decimal16T decimal16;
        Decimal32T decimal32;
        Decimal64T decimal64;
        Decimal128T decimal128;

        VarcharT varchar;
        Char1T char1;
        Char2T char2;
        Char4T char4;
        Char8T char8;
        Char15T char15;
        Char31T char31;
        Char63T char63;

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
        PathT      path;
        PolygonT   polygon;
        CircleT    circle;

        BitmapT    bitmap;
        UuidT      uuid;
        BlobT      blob;
        EmbeddingT embedding;

        MixedT     mixed_value;

        ~UnionValue() {
            // This is important for the member of union to have non-trivial destructor/copy constructor ...
            ;
        };

        UnionValue() {

        }

        UnionValue(const UnionValue& other) {
            ;
        };
//
//        UnionValue(UnionValue&& other) noexcept {
//            ;
//        };
//
//        UnionValue& operator=(const UnionValue& other) {
//            return *this;
//        };

        UnionValue& operator=(UnionValue&& other) noexcept {
            return *this;
        };
    } value_;
    bool is_null_{false};
};

// Value maker
template <> Value Value::MakeValue(BooleanT input);
template <> Value Value::MakeValue(TinyIntT input);
template <> Value Value::MakeValue(SmallIntT input);
template <> Value Value::MakeValue(IntegerT input);
template <> Value Value::MakeValue(BigIntT input);
template <> Value Value::MakeValue(HugeIntT input);
template <> Value Value::MakeValue(FloatT input);
template <> Value Value::MakeValue(DoubleT input);
template <> Value Value::MakeValue(Decimal16T input);
template <> Value Value::MakeValue(Decimal32T input);
template <> Value Value::MakeValue(Decimal64T input);
template <> Value Value::MakeValue(Decimal128T input);
template <> Value Value::MakeValue(VarcharT& input);
template <> Value Value::MakeValue(const String& input);
template <> Value Value::MakeValue(const char_t* input_ptr);
template <> Value Value::MakeValue(Char1T input);
template <> Value Value::MakeValue(Char2T input);
template <> Value Value::MakeValue(Char4T input);
template <> Value Value::MakeValue(Char8T input);
template <> Value Value::MakeValue(Char15T input);
template <> Value Value::MakeValue(Char31T input);
template <> Value Value::MakeValue(Char63T input);

template <> Value Value::MakeValue(DateT input);
template <> Value Value::MakeValue(TimeT input);
template <> Value Value::MakeValue(DateTimeT input);
template <> Value Value::MakeValue(TimestampT input) ;
template <> Value Value::MakeValue(TimestampTZT input);
template <> Value Value::MakeValue(IntervalT input);
template <> Value Value::MakeValue(ArrayT input);
template <> Value Value::MakeValue(PointT input);
template <> Value Value::MakeValue(LineT input);
template <> Value Value::MakeValue(LineSegT input);
template <> Value Value::MakeValue(BoxT input);
template <> Value Value::MakeValue(PathT input);
template <> Value Value::MakeValue(PolygonT input);
template <> Value Value::MakeValue(CircleT input);
template <> Value Value::MakeValue(BitmapT input);
template <> Value Value::MakeValue(UuidT input);
template <> Value Value::MakeValue(BlobT input);
template <> Value Value::MakeValue(EmbeddingT input);
template <> Value Value::MakeValue(MixedT input);

// Value getter
template <> BooleanT Value::GetValue() const;
template <> TinyIntT Value::GetValue() const;
template <> SmallIntT Value::GetValue() const;
template <> IntegerT Value::GetValue() const;
template <> BigIntT Value::GetValue() const;
template <> HugeIntT Value::GetValue() const;
template <> FloatT Value::GetValue() const;
template <> DoubleT Value::GetValue() const;
template <> Decimal16T Value::GetValue() const;
template <> Decimal32T Value::GetValue() const;
template <> Decimal64T Value::GetValue() const;
template <> Decimal128T Value::GetValue() const;
template <> VarcharT Value::GetValue() const;
template <> Char1T Value::GetValue() const;
template <> Char2T Value::GetValue() const;
template <> Char4T Value::GetValue() const;
template <> Char8T Value::GetValue() const;
template <> Char15T Value::GetValue() const;
template <> Char31T Value::GetValue() const;
template <> Char63T Value::GetValue() const;
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
template <> PathT Value::GetValue() const;
template <> PolygonT Value::GetValue() const;
template <> CircleT Value::GetValue() const;
template <> BitmapT Value::GetValue() const;
template <> UuidT Value::GetValue() const;
template <> BlobT Value::GetValue() const;
template <> EmbeddingT Value::GetValue() const;
template <> MixedT Value::GetValue() const;
}
