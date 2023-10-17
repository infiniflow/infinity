//
// Created by jinhai on 23-10-17.
//
module;

import parser;
import stl;
import infinity_assert;
import infinity_exception;

export module value;


namespace infinity {

export struct Value {
    // class member
public:
    // Value creator from different type of input
    static Value MakeValue(DataType type);

    static Value MakeBool(BooleanT input);

    static Value MakeTinyInt(TinyIntT input);

    static Value MakeSmallInt(SmallIntT input);

    static Value MakeInt(IntegerT input);

    static Value MakeBigInt(BigIntT input);

    static Value MakeHugeInt(HugeIntT input);

    static Value MakeFloat(FloatT input);

    static Value MakeDouble(DoubleT input);

    static Value MakeDecimal(DecimalT input, const SharedPtr<TypeInfo> &type_info_ptr);

    static Value MakeVarchar(VarcharT &input_ref);

    static Value MakeVarchar(const String &str);

    static Value MakeVarchar(const char *ptr);

    static Value MakeDate(DateT input);

    static Value MakeTime(TimeT input);

    static Value MakeDateTime(DateTimeT input);

    static Value MakeTimestamp(TimestampT input);

    static Value MakeInterval(IntervalT input);

    static Value MakeArray(ArrayT input);

    static Value MakeTuple(TupleT input);

    static Value MakePoint(PointT input);

    static Value MakeLine(LineT input);

    static Value MakeLineSegment(LineSegT input);

    static Value MakeBox(BoxT input);

    static Value MakePath(PathT input);

    static Value MakePolygon(PolygonT input);

    static Value MakeCircle(CircleT input);

    static Value MakeBitmap(BitmapT input);

    static Value MakeUuid(UuidT input);

    static Value MakeBlob(BlobT input);

    static Value MakeEmbedding(EmbeddingDataType type, SizeT dimension);

    static Value MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> embedding_info);

    static Value MakeRow(RowT input);

    static Value MakeMixedData(MixedT input);

    static Value MakeNull();

    // Object member
public:
    // Value getter template
    template <class T>
    T GetValue() const {
        Error<TypeException>("Not implemented value getter.", __FILE_NAME__, __LINE__);
    }

    [[nodiscard]] const DataType &type() const { return type_; }
    [[nodiscard]] bool is_null() const { return is_null_; }

    [[nodiscard]] String ToString() const;

    void Reset();

    bool TryCastAs(const DataType &target_type, Value &new_value) const;

    // Member method
public:
    //    Value() = default;
    explicit Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr = nullptr);

    Value(const Value &other);

    Value(Value &&other) noexcept;

    Value &operator=(const Value &other);

    Value &operator=(Value &&other) noexcept;

    ~Value();

private:
    void Init(bool in_constructor);

    void CopyUnionValue(const Value &other);

    void MoveUnionValue(Value &&other) noexcept;

public:
    DataType type_;

    union UnionValue {
        BooleanT boolean{};

        TinyIntT tiny_int;
        SmallIntT small_int;
        IntegerT integer;
        BigIntT big_int;
        HugeIntT huge_int;
        FloatT float32;
        DoubleT float64;
        DecimalT decimal;

        VarcharT varchar;

        DateT date;
        TimeT time;
        DateTimeT datetime;
        TimestampT timestamp;
        IntervalT interval;

        PointT point;
        LineT line;
        LineSegT line_segment;
        BoxT box;
        PathT path;
        PolygonT polygon;
        CircleT circle;

        BitmapT bitmap;
        UuidT uuid;
        BlobT blob;
        EmbeddingT embedding;
        RowT row;

        MixedT mixed_value;

        ~UnionValue() {
            // This is important for the member of union to have non-trivial destructor/copy constructor ...
            ;
        };

        UnionValue() {}

        UnionValue(const UnionValue &other) { ; };

        UnionValue(UnionValue &&other) noexcept { ; };

        UnionValue &operator=(const UnionValue &other) { return *this; };

        UnionValue &operator=(UnionValue &&other) noexcept { return *this; };
    } value_ = {};

    bool is_null_{false};

    ArrayT array;
};

// Value getter
template <>
BooleanT Value::GetValue() const;

template <>
TinyIntT Value::GetValue() const;

template <>
SmallIntT Value::GetValue() const;

template <>
IntegerT Value::GetValue() const;

template <>
BigIntT Value::GetValue() const;

template <>
HugeIntT Value::GetValue() const;

template <>
FloatT Value::GetValue() const;

template <>
DoubleT Value::GetValue() const;

template <>
DecimalT Value::GetValue() const;

template <>
VarcharT Value::GetValue() const;

template <>
DateT Value::GetValue() const;

template <>
TimeT Value::GetValue() const;

template <>
DateTimeT Value::GetValue() const;

template <>
TimestampT Value::GetValue() const;

template <>
IntervalT Value::GetValue() const;

template <>
ArrayT Value::GetValue() const;

template <>
PointT Value::GetValue() const;

template <>
LineT Value::GetValue() const;

template <>
LineSegT Value::GetValue() const;

template <>
BoxT Value::GetValue() const;

template <>
PathT Value::GetValue() const;

template <>
PolygonT Value::GetValue() const;

template <>
CircleT Value::GetValue() const;

template <>
BitmapT Value::GetValue() const;

template <>
UuidT Value::GetValue() const;

template <>
BlobT Value::GetValue() const;

template <>
EmbeddingT Value::GetValue() const;

template <>
MixedT Value::GetValue() const;
} // namespace infinity