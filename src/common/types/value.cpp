//
// Created by JinHai on 2022/11/7.
//

#include "value.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

#include <utility>

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
    value.value_.float32 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDouble(DoubleT input) {
    Value value(LogicalType::kDouble);
    value.value_.float64 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal16(Decimal16T input, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kDecimal16, type_info_ptr);
    value.value_.decimal16 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal32(Decimal32T input, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kDecimal32, type_info_ptr);
    value.value_.decimal32 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal64(Decimal64T input, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kDecimal64, type_info_ptr);
    value.value_.decimal64 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeDecimal128(Decimal128T input, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kDecimal128, type_info_ptr);
    value.value_.decimal128 = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeVarchar(VarcharT& input, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kVarchar, type_info_ptr);
    value.value_.varchar = input;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeVarchar(const String& str, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kVarchar, type_info_ptr);
    value.value_.varchar.Initialize(str);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeVarchar(const char* ptr, const SharedPtr<TypeInfo>& type_info_ptr) {
    Value value(LogicalType::kVarchar, type_info_ptr);
    value.value_.varchar.Initialize(ptr);
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
    value.value_.char2 = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar4(Char4T input) {
    Value value(LogicalType::kChar4);
    value.value_.char4 = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar8(Char8T input) {
    Value value(LogicalType::kChar8);
    value.value_.char8 = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar16(Char16T input) {
    Value value(LogicalType::kChar16);
    value.value_.char16 = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar32(Char32T input) {
    Value value(LogicalType::kChar32);
    value.value_.char32 = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeChar64(Char64T input) {
    Value value(LogicalType::kChar64);
    value.value_.char64 = std::move(input);
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
    value.array = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeTuple(TupleT input) {
    Value value(LogicalType::kTuple);
    value.array = std::move(input);
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
    value.value_.path = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakePolygon(PolygonT input) {
    Value value(LogicalType::kPolygon);
    value.value_.polygon = std::move(input);
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
    value.value_.bitmap = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeUuid(UuidT input) {
    Value value(LogicalType::kUuid);
    value.value_.uuid = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeBlob(BlobT input) {
    Value value(LogicalType::kBlob);
    value.value_.blob = std::move(input);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeEmbedding(EmbeddingDataType type, size_t dimension) {
    Value value(LogicalType::kEmbedding);
    value.value_.embedding = EmbeddingType(type, dimension);
    value.is_null_ = false;
    return value;
}

Value
Value::MakeEmbedding(ptr_t ptr) {
    Value value(LogicalType::kEmbedding);
    value.value_.embedding.ptr = ptr;
    value.is_null_ = false;
    return value;
}

Value
Value::MakeMixedData(MixedT input) {
    Value value(LogicalType::kMixed);
    value.value_.mixed_value = std::move(input);
    value.is_null_ = false;
    return value;
}

// Value getter
template <> BooleanT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBoolean, "Not matched type: " + type_.ToString())
    return value_.boolean;
}
template <> TinyIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTinyInt, "Not matched type: " + type_.ToString())
    return value_.tiny_int;
}

template <> SmallIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kSmallInt, "Not matched type: " + type_.ToString())
    return value_.small_int;
}

template <> IntegerT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kInteger, "Not matched type: " + type_.ToString())
    return value_.integer;
}

template <> BigIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBigInt, "Not matched type: " + type_.ToString())
    return value_.big_int;
}

template <> HugeIntT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kHugeInt, "Not matched type: " + type_.ToString())
    return value_.huge_int;
}

template <> FloatT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kFloat, "Not matched type: " + type_.ToString())
    return value_.float32;
}

template <> DoubleT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDouble, "Not matched type: " + type_.ToString())
    return value_.float64;
}

template <> Decimal16T
Value::GetValue() const {
TypeAssert(type_.type() == LogicalType::kDecimal16, "Not matched type: " + type_.ToString())
return value_.decimal16;
}

template <> Decimal32T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal32, "Not matched type: " + type_.ToString())
    return value_.decimal32;
}

template <> Decimal64T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal64, "Not matched type: " + type_.ToString())
    return value_.decimal64;
}

template <> Decimal128T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDecimal128, "Not matched type: " + type_.ToString())
    return value_.decimal128;
}

template <> VarcharT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kVarchar, "Not matched type: " + type_.ToString())
    return value_.varchar;
}

template <> Char1T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar1, "Not matched type: " + type_.ToString())
    return value_.char1;
}

template <> Char2T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar2, "Not matched type: " + type_.ToString())
    return value_.char2;
}

template <> Char4T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar4, "Not matched type: " + type_.ToString())
    return value_.char4;
}

template <> Char8T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar8, "Not matched type: " + type_.ToString())
    return value_.char8;
}

template <> Char16T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar16, "Not matched type: " + type_.ToString())
    return value_.char16;
}

template <> Char32T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar32, "Not matched type: " + type_.ToString())
    return value_.char32;
}

template <> Char64T
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kChar64, "Not matched type: " + type_.ToString())
    return value_.char64;
}

template <> DateT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDate, "Not matched type: " + type_.ToString())
    return value_.date;
}

template <> TimeT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTime, "Not matched type: " + type_.ToString())
    return value_.time;
}

template <> DateTimeT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kDateTime, "Not matched type: " + type_.ToString())
    return value_.datetime;
}

template <> TimestampT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTimestamp, "Not matched type: " + type_.ToString())
    return value_.timestamp;
}

template <> TimestampTZT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kTimestampTZ, "Not matched type: " + type_.ToString())
    return value_.timestamp_tz;
}

template <> IntervalT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kInterval, "Not matched type: " + type_.ToString())
    return value_.interval;
}

template <> ArrayT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kArray or type_.type() == LogicalType::kTuple,
               "Not matched type: " + type_.ToString())
    return array;
}

template <> PointT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPoint, "Not matched type: " + type_.ToString())
    return value_.point;
}

template <> LineT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kLine, "Not matched type: " + type_.ToString())
    return value_.line;
}

template <> LineSegT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kLineSeg, "Not matched type: " + type_.ToString())
    return value_.line_segment;
}

template <> BoxT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBox, "Not matched type: " + type_.ToString())
    return value_.box;
}

template <> PathT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPath, "Not matched type: " + type_.ToString())
    return value_.path;
}

template <> PolygonT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kPolygon, "Not matched type: " + type_.ToString())
    return value_.polygon;
}

template <> CircleT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kCircle, "Not matched type: " + type_.ToString())
    return value_.circle;
}

template <> BitmapT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBitmap, "Not matched type: " + type_.ToString())
    return value_.bitmap;
}

template <> UuidT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kUuid, "Not matched type: " + type_.ToString())
    return value_.uuid;
}

template <> BlobT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kBlob, "Not matched type: " + type_.ToString())
    return value_.blob;
}

template <> EmbeddingT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kEmbedding, "Not matched type: " + type_.ToString())
    return value_.embedding;
}

template <> MixedT
Value::GetValue() const {
    TypeAssert(type_.type() == LogicalType::kMixed, "Not matched type: " + type_.ToString())
    return value_.mixed_value;
}


Value::~Value() {
    switch(type_.type()) {
        case kVarchar: {
//            value_.varchar.~VarcharType();
            value_.varchar.Reset();
            break;
        }
        case kPolygon: {
//            value_.polygon.~PolygonType();
            value_.polygon.Reset();
            break;
        }
        case kPath: {
//            value_.path.~PathType();
            value_.path.Reset();
            break;
        }
        case kBitmap: {
//            value_.bitmap.~BitmapType();
            value_.bitmap.Reset();
            break;
        }
        case kBlob: {
//            value_.blob.~BlobType();
            value_.blob.Reset();
            break;
        }
        case kMixed: {
//            value_.mixed_value.~MixedType();
            value_.mixed_value.Reset();
        }
        default: {

        }
    }
    GlobalResourceUsage::DecrObjectCount();
}

Value::Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr): type_(type, std::move(typeinfo_ptr)) {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
}

Value::Value(const Value& other) : type_(other.type_), is_null_(other.is_null_)  {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
    CopyUnionValue(other);
}

Value::Value(Value&& other) noexcept :
    type_(std::move(other.type_)),
    is_null_(other.is_null_) {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
    MoveUnionValue(std::forward<Value>(other));
}

Value&
Value::operator=(const Value& other) {
    if(this == &other) return *this;
    this->Reset();
    if(this->type_ != other.type_) {
        this->type_ = other.type_;
        this->Init(false);
    }
    this->is_null_ = other.is_null_;
    CopyUnionValue(other);

    return *this;
}

Value&
Value::operator=(Value&& other)  noexcept {
    // Clear exist value, since some type need to free the allocated heap memory.
    this->Reset();
    if(this->type_ != other.type_) {
        this->type_ = std::move(other.type_);
        this->Init(false);
    }
    this->is_null_ = other.is_null_;
    MoveUnionValue(std::forward<Value>(other));

    return *this;
}

void
Value::Init(bool in_constructor) {
    switch(type_.type()) {
        case kBoolean: {
            value_.boolean = false;
            break;
        }
        case kTinyInt: {
            value_.tiny_int = 0;
            break;
        }
        case kSmallInt: {
            value_.small_int = 0;
            break;
        }
        case kInteger: {
            value_.integer = 0;
            break;
        }
        case kBigInt: {
            value_.big_int = 0;
            break;
        }
        case kHugeInt: {
            value_.huge_int.Reset();
            break;
        }
        case kFloat: {
            value_.float32 = 0;
            break;
        }
        case kDouble: {
            value_.float64 = 0;
            break;
        }
        case kDecimal16: {
            value_.decimal16.value = 0;
            break;
        }
        case kDecimal32: {
            value_.decimal32.value = 0;
            break;
        }
        case kDecimal64: {
            value_.decimal64.value = 0;
            break;
        }
        case kDecimal128: {
            value_.decimal128.Reset();
            break;
        }
        case kVarchar: {
            value_.varchar.ptr = nullptr;
            value_.varchar.length = 0;
            break;
        }
        case kChar1: {
            value_.char1.value = 0;
            break;
        }
        case kChar2: {
            value_.char2.Reset();
            break;
        }
        case kChar4: {
            value_.char4.Reset();
            break;
        }
        case kChar8: {
            value_.char8.Reset();
            break;
        }
        case kChar16: {
            value_.char16.Reset();
            break;
        }
        case kChar32: {
            value_.char32.Reset();
            break;
        }
        case kChar64: {
            value_.char64.Reset();
            break;
        }
        case kDate: {
            value_.date.value = 0;
            break;
        }
        case kTime: {
            value_.time.value = 0;
            break;
        }
        case kDateTime: {
            value_.datetime.Reset();
            break;
        }
        case kTimestamp: {
            value_.timestamp.Reset();
            break;
        }
        case kTimestampTZ: {
            value_.timestamp_tz.Reset();
            break;
        }
        case kInterval: {
            value_.interval.Reset();
            break;
        }
        case kArray:
        case kTuple: {
            // empty function
            array.clear();
            break;
        }
        case kPoint: {
            value_.point.Reset();
            break;
        }
        case kLine: {
            value_.line.Reset();
            break;
        }
        case kLineSeg: {
            value_.line_segment.Reset();
            break;
        }
        case kBox: {
            value_.box.Reset();
            break;
        }
        case kPath: {
            value_.path.closed = false;
            value_.path.ptr = nullptr;
            value_.path.point_count = 0;
            break;
        }
        case kPolygon: {
            value_.polygon.point_count = 0;
            value_.polygon.ptr = nullptr;
            value_.polygon.bounding_box.Reset();
            break;
        }
        case kCircle: {
            value_.circle.Reset();
            break;
        }
        case kBitmap: {
            value_.bitmap.ptr = nullptr;
            value_.bitmap.count = 0;
            break;
        }
        case kUuid: {
            value_.uuid.Reset();
            break;
        }
        case kBlob: {
            value_.blob.ptr = nullptr;
            value_.blob.size = 0;
            break;
        }
        case kEmbedding: {
            value_.embedding.SetNull();
            break;
        }
        case kMixed: {
            value_.mixed_value.Reset(in_constructor);
            break;
        }
        case kNull:
        case kMissing:
        case kInvalid:
            break;
    }
}

void
Value::CopyUnionValue(const Value& other) {
    switch(type_.type()) {
        case kBoolean: {
            value_.boolean = other.value_.boolean;
            break;
        }
        case kTinyInt: {
            value_.tiny_int = other.value_.tiny_int;
            break;
        }
        case kSmallInt: {
            value_.small_int = other.value_.small_int;
            break;
        }
        case kInteger: {
            value_.integer = other.value_.integer;
            break;
        }
        case kBigInt: {
            value_.big_int = other.value_.big_int;
            break;
        }
        case kHugeInt: {
            // trivial copy-assignment
            value_.huge_int = other.value_.huge_int;
            break;
        }
        case kFloat: {
            value_.float32 = other.value_.float32;
            break;
        }
        case kDouble: {
            value_.float64 = other.value_.float64;
            break;
        }
        case kDecimal16: {
            // trivial copy-assignment
            value_.decimal16 = other.value_.decimal16;
            break;
        }
        case kDecimal32: {
            // trivial copy-assignment
            value_.decimal32 = other.value_.decimal32;
            break;
        }
        case kDecimal64: {
            // trivial copy-assignment
            value_.decimal64 = other.value_.decimal64;
            break;
        }
        case kDecimal128: {
            // trivial copy-assignment
            value_.decimal128 = other.value_.decimal128;
            break;
        }
        case kVarchar: {
            // Will use Varchar copy-assignment
            value_.varchar = other.value_.varchar;
            break;
        }
        case kChar1: {
            // trivial copy-assignment
            value_.char1 = other.value_.char1;
            break;
        }
        case kChar2: {
            // Char2Type copy assignment
            value_.char2 = other.value_.char2;
            break;
        }
        case kChar4: {
            // Char4Type copy assignment
            value_.char4 = other.value_.char4;
            break;
        }
        case kChar8: {
            // Char8Type copy assignment
            value_.char8 = other.value_.char8;
            break;
        }
        case kChar16: {
            // Char16Type copy assignment
            value_.char16 = other.value_.char16;
            break;
        }
        case kChar32: {
            // Char32Type copy assignment
            value_.char32 = other.value_.char32;
            break;
        }
        case kChar64: {
            // Char64Type copy assignment
            value_.char64 = other.value_.char64;
            break;
        }
        case kDate: {
            // trivial copy-assignment
            value_.date = other.value_.date;
            break;
        }
        case kTime: {
            // trivial copy-assignment
            value_.time = other.value_.time;
            break;
        }
        case kDateTime: {
            // trivial copy-assignment
            value_.datetime = other.value_.datetime;
            break;
        }
        case kTimestamp: {
            // trivial copy-assignment
            value_.timestamp = other.value_.timestamp;
            break;
        }
        case kTimestampTZ: {
            // trivial copy-assignment
            value_.timestamp_tz = other.value_.timestamp_tz;
            break;
        }
        case kInterval: {
            // trivial copy-assignment
            value_.interval = other.value_.interval;
            break;
        }
        case kArray:
        case kTuple: {
            // std::vector copy-assignment
            array = other.array;
            break;
        }
        case kPoint: {
            // trivial copy-assignment
            value_.point = other.value_.point;
            break;
        }
        case kLine: {
            // trivial copy-assignment
            value_.line = other.value_.line;
            break;
        }
        case kLineSeg: {
            // trivial copy-assignment
            value_.line_segment = other.value_.line_segment;
            break;
        }
        case kBox: {
            // trivial copy-assignment
            value_.box = other.value_.box;
            break;
        }
        case kPath: {
            // PathT copy assignment
            value_.path = other.value_.path;
            break;
        }
        case kPolygon: {
            // Polygon copy assignment
            value_.polygon = other.value_.polygon;
            break;
        }
        case kCircle: {
            // trivial copy-assignment
            value_.circle = other.value_.circle;
            break;
        }
        case kBitmap: {
            // bitmap copy assignment
            value_.bitmap = other.value_.bitmap;
            break;
        }
        case kUuid: {
            // UUID copy assignment
            value_.uuid = other.value_.uuid;
            break;
        }
        case kBlob: {
            // Blob copy assignment
            value_.blob = other.value_.blob;
            break;
        }
        case kEmbedding: {
            size_t embedding_size = type_.type_info()->Size();

            value_.embedding.ptr = new char_t[embedding_size]{0};
            GlobalResourceUsage::IncrRawMemCount();

            memcpy(value_.embedding.ptr, other.value_.embedding.ptr, embedding_size);
            break;
        }
        case kMixed: {
            // Heterogeneous data copy assignment
            value_.mixed_value = other.value_.mixed_value;
            break;
        }
        case kNull: {
            // No value for null value.
            break;
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Unexpected error!")
            break;
        }
    }
}

void
Value::MoveUnionValue(Value&& other) noexcept {
    switch(this->type_.type()) {
        case kBoolean: {
            this->value_.boolean = other.value_.boolean;
            break;
        }
        case kTinyInt: {
            this->value_.tiny_int = other.value_.tiny_int;
            break;
        }
        case kSmallInt: {
            this->value_.small_int = other.value_.small_int;
            break;
        }
        case kInteger: {
            this->value_.integer = other.value_.integer;
            break;
        }
        case kBigInt: {
            this->value_.big_int = other.value_.big_int;
            break;
        }
        case kHugeInt: {
            this->value_.huge_int = other.value_.huge_int;
            break;
        }
        case kFloat: {
            this->value_.float32 = other.value_.float32;
            break;
        }
        case kDouble: {
            this->value_.float64 = other.value_.float64;
            break;
        }
        case kDecimal16: {
            this->value_.decimal16 = other.value_.decimal16;
            break;
        }
        case kDecimal32: {
            this->value_.decimal32 = other.value_.decimal32;
            break;
        }
        case kDecimal64: {
            this->value_.decimal64 = other.value_.decimal64;
            break;
        }
        case kDecimal128: {
            this->value_.decimal128 = other.value_.decimal128;
            break;
        }
        case kVarchar: {
            this->value_.varchar = std::move(other.value_.varchar);
            break;
        }
        case kChar1: {
            this->value_.char1.value = other.value_.char1.value;
            break;
        }
        case kChar2: {
            this->value_.char2 = std::move(other.value_.char2);
            break;
        }
        case kChar4: {
            this->value_.char4 = std::move(other.value_.char4);
            break;
        }
        case kChar8: {
            this->value_.char8 = std::move(other.value_.char8);
            break;
        }
        case kChar16: {
            this->value_.char16 = std::move(other.value_.char16);
            break;
        }
        case kChar32: {
            this->value_.char32 = std::move(other.value_.char32);
            break;
        }
        case kChar64: {
            this->value_.char64 = std::move(other.value_.char64);
            break;
        }
        case kDate: {
            this->value_.date = other.value_.date;
            break;
        }
        case kTime: {
            this->value_.time = other.value_.time;
            break;
        }
        case kDateTime: {
            this->value_.datetime = other.value_.datetime;
            break;
        }
        case kTimestamp: {
            this->value_.timestamp = other.value_.timestamp;
            break;
        }
        case kTimestampTZ: {
            this->value_.timestamp_tz = other.value_.timestamp_tz;
            break;
        }
        case kInterval: {
            this->value_.interval = other.value_.interval;
            break;
        }
        case kArray:
        case kTuple: {
            this->array = std::move(other.array);
            break;
        }
        case kPoint: {
            this->value_.point = other.value_.point;
            break;
        }
        case kLine: {
            this->value_.line = other.value_.line;
            break;
        }
        case kLineSeg: {
            this->value_.line_segment = other.value_.line_segment;
            break;
        }
        case kBox: {
            this->value_.box = other.value_.box;
            break;
        }
        case kPath: {
            this->value_.path = std::move(other.value_.path);
            break;
        }
        case kPolygon: {
            this->value_.polygon = std::move(other.value_.polygon);
            break;
        }
        case kCircle: {
            this->value_.circle = other.value_.circle;
            break;
        }
        case kBitmap: {
            this->value_.bitmap = std::move(other.value_.bitmap);
            break;
        }
        case kUuid: {
            this->value_.uuid = std::move(other.value_.uuid);
            break;
        }
        case kBlob: {
            this->value_.blob = std::move(other.value_.blob);
            break;
        }
        case kEmbedding: {
            this->value_.embedding = other.value_.embedding;
            other.value_.embedding.SetNull();
            break;
        }
        case kMixed: {
            this->value_.mixed_value = std::move(other.value_.mixed_value);
            break;
        }
        case kNull: {
            // No value for null type
            break;
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Unexpected error!")
            break;
        }
    }
}

void
Value::Reset() {
    switch(type_.type()) {
        case kBoolean: {
            value_.boolean = false;
            break;
        }
        case kTinyInt: {
            value_.tiny_int = 0;
            break;
        }
        case kSmallInt: {
            value_.small_int = 0;
            break;
        }
        case kInteger: {
            value_.integer = 0;
            break;
        }
        case kBigInt: {
            value_.big_int = 0;
            break;
        }
        case kHugeInt: {
            value_.huge_int.Reset();
            break;
        }
        case kFloat: {
            value_.float32 = 0;
            break;
        }
        case kDouble: {
            value_.float64 = 0;
            break;
        }
        case kDecimal16: {
            value_.decimal16.value = 0;
            break;
        }
        case kDecimal32: {
            value_.decimal32.value = 0;
            break;
        }
        case kDecimal64: {
            value_.decimal64.value = 0;
            break;
        }
        case kDecimal128: {
            value_.decimal128.Reset();
            break;
        }
        case kVarchar: {
            value_.varchar.Reset();
            break;
        }
        case kChar1: {
            value_.char1.value = 0;
            break;
        }
        case kChar2: {
            value_.char2.Reset();
            break;
        }
        case kChar4: {
            value_.char4.Reset();
            break;
        }
        case kChar8: {
            value_.char8.Reset();
            break;
        }
        case kChar16: {
            value_.char16.Reset();
            break;
        }
        case kChar32: {
            value_.char32.Reset();
            break;
        }
        case kChar64: {
            value_.char64.Reset();
            break;
        }
        case kDate: {
            value_.date.value = 0;
            break;
        }
        case kTime: {
            value_.time.value = 0;
            break;
        }
        case kDateTime: {
            value_.datetime.Reset();
            break;
        }
        case kTimestamp: {
            value_.timestamp.Reset();
            break;
        }
        case kTimestampTZ: {
            value_.timestamp_tz.Reset();
            break;
        }
        case kInterval: {
            value_.interval.Reset();
            break;
        }
        case kArray:
        case kTuple: {
            // empty function
            array.clear();
            break;
        }
        case kPoint: {
            value_.point.Reset();
            break;
        }
        case kLine: {
            value_.line.Reset();
            break;
        }
        case kLineSeg: {
            value_.line_segment.Reset();
            break;
        }
        case kBox: {
            value_.box.Reset();
            break;
        }
        case kPath: {
            value_.path.Reset();
            break;
        }
        case kPolygon: {
            value_.polygon.Reset();
            break;
        }
        case kCircle: {
            value_.circle.Reset();
            break;
        }
        case kBitmap: {
            value_.bitmap.Reset();
            break;
        }
        case kUuid: {
            value_.uuid.Reset();
            break;
        }
        case kBlob: {
            value_.blob.Reset();
            break;
        }
        case kEmbedding: {
            if(value_.embedding.ptr != nullptr) {
                LOG_TRACE("Need to manually reset the embedding type.");
            }
            break;
        }
        case kMixed: {
            value_.mixed_value.Reset();
            break;
        }
        case kNull:
        case kMissing:
        case kInvalid:
            break;
    }
    type_.Reset();
}

String
Value::ToString() const {
    switch(type_.type()) {
        case kBoolean:
            return value_.boolean ? "true": "false";
        case kTinyInt:
            return std::to_string(value_.tiny_int);
        case kSmallInt:
            return std::to_string(value_.small_int);
        case kInteger:
            return std::to_string(value_.integer);
        case kBigInt:
            return std::to_string(value_.big_int);
        case kHugeInt:
            return value_.huge_int.ToString();
        case kFloat:
            return std::to_string(value_.float32);
        case kDouble:
            return std::to_string(value_.float64);
        case kDecimal16:
            break;
        case kDecimal32:
            break;
        case kDecimal64:
            break;
        case kDecimal128:
            break;
        case kVarchar:
            break;
        case kChar1:
            break;
        case kChar2:
            break;
        case kChar4:
            break;
        case kChar8:
            break;
        case kChar16:
            break;
        case kChar32:
            break;
        case kChar64:
            break;
        case kDate:
            break;
        case kTime:
            break;
        case kDateTime:
            break;
        case kTimestamp:
            break;
        case kTimestampTZ:
            break;
        case kInterval:
            break;
        case kArray:
            break;
        case kTuple:
            break;
        case kPoint:
            break;
        case kLine:
            break;
        case kLineSeg:
            break;
        case kBox:
            break;
        case kPath:
            break;
        case kPolygon:
            break;
        case kCircle:
            break;
        case kBitmap:
            break;
        case kUuid:
            break;
        case kBlob:
            break;
        case kEmbedding:
            break;
        case kMixed:
            break;
        case kNull:
            break;
        case kMissing:
            break;
        case kInvalid:
            break;
    }
    TypeError("Unexpected error.")
}

}
