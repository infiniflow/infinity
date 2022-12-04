//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "data_type.h"
#include "common/types/info/decimal16_info.h"
#include "common/types/info/decimal32_info.h"
#include "common/types/info/decimal64_info.h"
#include "common/types/info/decimal128_info.h"
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
    MakeDecimal16(Decimal16T input, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeDecimal32(Decimal32T input, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeDecimal64(Decimal64T input, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeDecimal128(Decimal128T input, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeVarchar(VarcharT& input_ref, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeVarchar(const String& str, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeVarchar(const char* ptr, const SharedPtr<TypeInfo>& type_info_ptr);

    static Value
    MakeChar1(Char1T input_ref);

    static Value
    MakeChar2(Char2T input_ref);

    static Value
    MakeChar4(Char4T input_ref);

    static Value
    MakeChar8(Char8T input_ref);

    static Value
    MakeChar16(Char16T input);

    static Value
    MakeChar32(Char32T input);

    static Value
    MakeChar64(Char64T input);

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
    MakeTuple(TupleT input);

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
    MakeEmbedding(EmbeddingDataType type, size_t dimension);

    static Value
    MakeEmbedding(ptr_t ptr);

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

    void
    Reset();
    // Member method
public:
//    Value() = default;
    explicit
    Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr = nullptr);

    Value(const Value& other);

    Value(Value&& other) noexcept ;

    Value&
    operator=(const Value& other);

    Value&
    operator=(Value&& other) noexcept ;

    ~Value();

private:
    void
    Init(bool in_constructor);

    void
    CopyUnionValue(const Value& other);

    void
    MoveUnionValue(Value&& other) noexcept;
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
        Char16T char16;
        Char32T char32;
        Char64T char64;

        DateT    date;
        TimeT    time;
        DateTimeT datetime;
        TimestampT timestamp;
        TimestampTZT  timestamp_tz;
        IntervalT  interval;

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

        UnionValue(UnionValue&& other) noexcept {
            ;
        };

        UnionValue& operator=(const UnionValue& other) {
            return *this;
        };

        UnionValue& operator=(UnionValue&& other) noexcept {
            return *this;
        };
    } value_ = {};
    bool is_null_{false};

    ArrayT array;
};

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
template <> Char16T Value::GetValue() const;
template <> Char32T Value::GetValue() const;
template <> Char64T Value::GetValue() const;
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
