// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import parser;
import stl;
import global_resource_usage;
import logger;

import infinity_exception;
import third_party;
import bound_cast_func;
import cast_function;
import column_vector;
import default_values;

module value;

namespace infinity {

// Value maker
Value Value::MakeValue(DataType type) {
    Value value(type.type(), type.type_info());
    value.is_null_ = true;
    return value;
}

Value Value::MakeBool(BooleanT input) {
    Value value(LogicalType::kBoolean);
    value.value_.boolean = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeTinyInt(TinyIntT input) {
    Value value(LogicalType::kTinyInt);
    value.value_.tiny_int = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeSmallInt(SmallIntT input) {
    Value value(LogicalType::kSmallInt);
    value.value_.small_int = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeInt(IntegerT input) {
    Value value(LogicalType::kInteger);
    value.value_.integer = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeBigInt(BigIntT input) {
    Value value(LogicalType::kBigInt);
    value.value_.big_int = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeHugeInt(HugeIntT input) {
    Value value(LogicalType::kHugeInt);
    value.value_.huge_int = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeFloat(FloatT input) {
    Value value(LogicalType::kFloat);
    value.value_.float32 = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeDouble(DoubleT input) {
    Value value(LogicalType::kDouble);
    value.value_.float64 = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeDecimal(DecimalT input, const SharedPtr<TypeInfo> &type_info_ptr) {
    Value value(LogicalType::kDecimal, type_info_ptr);
    value.value_.decimal = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeVarchar(VarcharT &input) {
    Value value(LogicalType::kVarchar);
    if(input.IsInlined()) {
        value.value_.varchar = input;
    } else {
        if(input.IsValue()) {
            value.value_.varchar = input;
        } else {
            Error<StorageException>("Can't make varchar from column vector");
        }
    }
    value.value_.varchar.is_value_ = true;
    value.is_null_ = false;
    return value;
}

Value Value::MakeVarchar(const String &str) {
    Value value(LogicalType::kVarchar);
    value.value_.varchar.InitAsValue(str);
    value.is_null_ = false;
    return value;
}

Value Value::MakeVarchar(const char *ptr, bool is_move) {
    Value value(LogicalType::kVarchar);
    value.value_.varchar.InitAsValue(ptr, is_move);
    value.is_null_ = false;
    return value;
}

Value Value::MakeVarchar(const char* ptr, SizeT len) {
    Value value(LogicalType::kVarchar);
    value.value_.varchar.InitAsValue(ptr, len, false);
    value.is_null_ = false;
    return value;
}

Value Value::MakeDate(DateT input) {
    Value value(LogicalType::kDate);
    value.value_.date = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeTime(TimeT input) {
    Value value(LogicalType::kTime);
    value.value_.time = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeDateTime(DateTimeT input) {
    Value value(LogicalType::kDateTime);
    value.value_.datetime = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeTimestamp(TimestampT input) {
    Value value(LogicalType::kTimestamp);
    value.value_.timestamp = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeInterval(IntervalT input) {
    Value value(LogicalType::kInterval);
    value.value_.interval = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeArray(ArrayT input) {
    Value value(LogicalType::kArray);
    value.array = Move(input);
    value.is_null_ = false;
    return value;
}

Value Value::MakeTuple(TupleT input) {
    Value value(LogicalType::kTuple);
    value.array = Move(input);
    value.is_null_ = false;
    return value;
}

Value Value::MakePoint(PointT input) {
    Value value(LogicalType::kPoint);
    value.value_.point = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeLine(LineT input) {
    Value value(LogicalType::kLine);
    value.value_.line = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeLineSegment(LineSegT input) {
    Value value(LogicalType::kLineSeg);
    value.value_.line_segment = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeBox(BoxT input) {
    Value value(LogicalType::kBox);
    value.value_.box = input;
    value.is_null_ = false;
    return value;
}

//Value Value::MakePath(PathT input) {
//    Value value(LogicalType::kPath);
//    value.value_.path = Move(input);
//    value.is_null_ = false;
//    return value;
//}
//
//Value Value::MakePolygon(PolygonT input) {
//    Value value(LogicalType::kPolygon);
//    value.value_.polygon = Move(input);
//    value.is_null_ = false;
//    return value;
//}

Value Value::MakeCircle(CircleT input) {
    Value value(LogicalType::kCircle);
    value.value_.circle = input;
    value.is_null_ = false;
    return value;
}

//Value Value::MakeBitmap(BitmapT input) {
//    Value value(LogicalType::kBitmap);
//    value.value_.bitmap = Move(input);
//    value.is_null_ = false;
//    return value;
//}

Value Value::MakeUuid(UuidT input) {
    Value value(LogicalType::kUuid);
    value.value_.uuid = Move(input);
    value.is_null_ = false;
    return value;
}

//Value Value::MakeBlob(BlobT input) {
//    Value value(LogicalType::kBlob);
//    value.value_.blob = Move(input);
//    value.is_null_ = false;
//    return value;
//}

Value Value::MakeEmbedding(EmbeddingDataType type, SizeT dimension) {
    auto embedding_info_ptr = EmbeddingInfo::Make(type, dimension);
    Value value(LogicalType::kEmbedding, embedding_info_ptr);
    value.value_.embedding = EmbeddingT(type, dimension);
    value.is_null_ = false;
    return value;
}

Value Value::MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> embedding_info) {
    Value value(LogicalType::kEmbedding, Move(embedding_info));
    value.value_.embedding = EmbeddingT(static_cast<char *>(ptr), false);
    value.is_null_ = false;
    return value;
}

Value Value::MakeRow(RowID input) {
    Value value(LogicalType::kRowID);
    value.value_.row = input;
    value.is_null_ = false;
    return value;
}

Value Value::MakeMixedData(MixedT input) {
    Value value(LogicalType::kMixed);
    value.value_.mixed_value = Move(input);
    value.is_null_ = false;
    return value;
}

Value Value::MakeNull() {
    Value value(LogicalType::kNull);
    value.is_null_ = true;
    return value;
}

Value Value::MakeInvalid() {
    Value value(LogicalType::kInvalid);
    return value;
}

// Value getter
template <>
BooleanT Value::GetValue() const {
    if (type_.type() != LogicalType::kBoolean) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.boolean;
}

template <>
TinyIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kTinyInt) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.tiny_int;
}

template <>
SmallIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kSmallInt) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.small_int;
}

template <>
IntegerT Value::GetValue() const {
    if (type_.type() != LogicalType::kInteger) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.integer;
}

template <>
BigIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kBigInt) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.big_int;
}

template <>
HugeIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kHugeInt) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.huge_int;
}

template <>
FloatT Value::GetValue() const {
    if (type_.type() != LogicalType::kFloat) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.float32;
}

template <>
DoubleT Value::GetValue() const {
    if (type_.type() != LogicalType::kDouble) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.float64;
}

template <>
DecimalT Value::GetValue() const {
    if (type_.type() != LogicalType::kDecimal) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.decimal;
}

template <>
VarcharT Value::GetValue() const {
    if (type_.type() != LogicalType::kVarchar) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.varchar;
}

template <>
DateT Value::GetValue() const {
    if (type_.type() != LogicalType::kDate) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.date;
}

template <>
TimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kTime) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.time;
}

template <>
DateTimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kDateTime) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.datetime;
}

template <>
TimestampT Value::GetValue() const {
    if (type_.type() != LogicalType::kTimestamp) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.timestamp;
}

template <>
IntervalT Value::GetValue() const {
    if (type_.type() != LogicalType::kInterval) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.interval;
}

template <>
ArrayT Value::GetValue() const {
    if (type_.type() != LogicalType::kArray) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return array;
}

template <>
PointT Value::GetValue() const {
    if (type_.type() != LogicalType::kPoint) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.point;
}

template <>
LineT Value::GetValue() const {
    if (type_.type() != LogicalType::kLine) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.line;
}

template <>
LineSegT Value::GetValue() const {
    if (type_.type() != LogicalType::kLineSeg) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.line_segment;
}

template <>
BoxT Value::GetValue() const {
    if (type_.type() != LogicalType::kBox) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.box;
}

//template <>
//PathT Value::GetValue() const {
//    Assert<TypeException>(type_.type() == LogicalType::kPath, Format("Not matched type: {}", type_.ToString()));
//    return value_.path;
//}
//
//template <>
//PolygonT Value::GetValue() const {
//    Assert<TypeException>(type_.type() == LogicalType::kPolygon, Format("Not matched type: {}", type_.ToString()));
//    return value_.polygon;
//}

template <>
CircleT Value::GetValue() const {
    if (type_.type() != LogicalType::kCircle) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.circle;
}

//template <>
//BitmapT Value::GetValue() const {
//    Assert<TypeException>(type_.type() == LogicalType::kBitmap, Format("Not matched type: {}", type_.ToString()));
//    return value_.bitmap;
//}

template <>
UuidT Value::GetValue() const {
    if (type_.type() != LogicalType::kUuid) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.uuid;
}

//template <>
//BlobT Value::GetValue() const {
//    Assert<TypeException>(type_.type() == LogicalType::kBlob, Format("Not matched type: {}", type_.ToString()));
//    return value_.blob;
//}

template <>
EmbeddingT Value::GetValue() const {
    if (type_.type() != LogicalType::kEmbedding) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.embedding;
}

template <>
RowID Value::GetValue() const {
    if (type_.type() != LogicalType::kRowID) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.row;
}

template <>
MixedT Value::GetValue() const {
    if (type_.type() != LogicalType::kMixed) {
        Error<TypeException>(Format("Not matched type: {}", type_.ToString()));
    }
    return value_.mixed_value;
}

Value::~Value() {
    switch (type_.type()) {
        case kVarchar: {
            //            value_.varchar.~VarcharType();
            value_.varchar.Reset();
            break;
        }
//        case kPolygon: {
//            //            value_.polygon.~PolygonType();
//            value_.polygon.Reset();
//            break;
//        }
//        case kPath: {
//            //            value_.path.~PathType();
//            value_.path.Reset();
//            break;
//        }
//        case kBitmap: {
//            //            value_.bitmap.~BitmapType();
//            value_.bitmap.Reset();
//            break;
//        }
//        case kBlob: {
//            //            value_.blob.~BlobType();
//            value_.blob.Reset();
//            break;
//        }
        case kEmbedding: {
            value_.embedding.Reset();
            break;
        }
        case kMixed: {
            //            value_.mixed_value.~MixedType();
            value_.mixed_value.Reset();
            break;
        }
        default: {
        }
    }
    GlobalResourceUsage::DecrObjectCount();
}

Value::Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr) : type_(type, Move(typeinfo_ptr)) {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
}

Value::Value(const Value &other) : type_(other.type_), is_null_(other.is_null_) {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
    CopyUnionValue(other);
}

Value::Value(Value &&other) noexcept : type_(Move(other.type_)), is_null_(other.is_null_) {
    GlobalResourceUsage::IncrObjectCount();
    Init(true);
    MoveUnionValue(Forward<Value>(other));
}

Value &Value::operator=(const Value &other) {
    if (this == &other)
        return *this;
    this->Reset();
    if (this->type_ != other.type_) {
        this->type_ = other.type_;
        this->Init(false);
    }
    this->is_null_ = other.is_null_;
    CopyUnionValue(other);

    return *this;
}

Value &Value::operator=(Value &&other) noexcept {
    // Clear exist value, since some type need to free the allocated heap memory.
    this->Reset();
    if (this->type_ != other.type_) {
        this->type_ = Move(other.type_);
        this->Init(false);
    }
    this->is_null_ = other.is_null_;
    MoveUnionValue(Forward<Value>(other));

    return *this;
}

void Value::Init(bool in_constructor) {
    switch (type_.type()) {
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
        case kDecimal: {
            value_.decimal.Reset();
            break;
        }
        case kVarchar: {
            value_.varchar.value_.ptr_ = nullptr;
            value_.varchar.length_ = 0;
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
//        case kPath: {
//            value_.path.closed = false;
//            value_.path.ptr = nullptr;
//            value_.path.point_count = 0;
//            break;
//        }
//        case kPolygon: {
//            value_.polygon.point_count = 0;
//            value_.polygon.ptr = nullptr;
//            value_.polygon.bounding_box.Reset();
//            break;
//        }
        case kCircle: {
            value_.circle.Reset();
            break;
        }
//        case kBitmap: {
//            value_.bitmap.ptr = nullptr;
//            value_.bitmap.count = 0;
//            break;
//        }
        case kUuid: {
            value_.uuid.Reset();
            break;
        }
//        case kBlob: {
//            value_.blob.ptr = nullptr;
//            value_.blob.size = 0;
//            break;
//        }
        case kEmbedding: {
            value_.embedding.SetNull();
            break;
        }
        case kRowID: {
            value_.row = -1;
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

void Value::CopyUnionValue(const Value &other) {
    switch (type_.type()) {
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
        case kDecimal: {
            // trivial copy-assignment
            value_.decimal = other.value_.decimal;
            break;
        }
        case kVarchar: {
            // Will use Varchar copy-assignment
            value_.varchar = other.value_.varchar;
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
        case kInterval: {
            // trivial copy-assignment
            value_.interval = other.value_.interval;
            break;
        }
        case kArray:
        case kTuple: {
            // vector copy-assignment
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
//        case kPath: {
//            // PathT copy assignment
//            value_.path = other.value_.path;
//            break;
//        }
//        case kPolygon: {
//            // Polygon copy assignment
//            value_.polygon = other.value_.polygon;
//            break;
//        }
        case kCircle: {
            // trivial copy-assignment
            value_.circle = other.value_.circle;
            break;
        }
//        case kBitmap: {
//            // bitmap copy assignment
//            value_.bitmap = other.value_.bitmap;
//            break;
//        }
        case kUuid: {
            // UUID copy assignment
            value_.uuid = other.value_.uuid;
            break;
        }
//        case kBlob: {
//            // Blob copy assignment
//            value_.blob = other.value_.blob;
//            break;
//        }
        case kEmbedding: {
            SizeT embedding_size = type_.type_info()->Size();

            if (value_.embedding.new_allocated_ && value_.embedding.ptr) {
                delete[] value_.embedding.ptr;
            }
            if (other.value_.embedding.new_allocated_) {
                const_cast<bool &>(value_.embedding.new_allocated_) = true;
                value_.embedding.ptr = new char_t[embedding_size]{0};
                Memcpy(value_.embedding.ptr, other.value_.embedding.ptr, embedding_size);
            } else {
                const_cast<bool &>(value_.embedding.new_allocated_) = false;
                value_.embedding.ptr = other.value_.embedding.ptr;
            }
            break;
        }
        case kRowID: {
            value_.row = other.value_.row;
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
            LOG_ERROR("Unexpected error!");
            break;
        }
    }
}

void Value::MoveUnionValue(Value &&other) noexcept {
    switch (this->type_.type()) {
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
        case kDecimal: {
            this->value_.decimal = other.value_.decimal;
            break;
        }
        case kVarchar: {
            this->value_.varchar = Move(other.value_.varchar);
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
        case kInterval: {
            this->value_.interval = other.value_.interval;
            break;
        }
        case kArray:
        case kTuple: {
            this->array = Move(other.array);
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
//        case kPath: {
//            this->value_.path = Move(other.value_.path);
//            break;
//        }
//        case kPolygon: {
//            this->value_.polygon = Move(other.value_.polygon);
//            break;
//        }
        case kCircle: {
            this->value_.circle = other.value_.circle;
            break;
        }
//        case kBitmap: {
//            this->value_.bitmap = Move(other.value_.bitmap);
//            break;
//        }
        case kUuid: {
            this->value_.uuid = Move(other.value_.uuid);
            break;
        }
//        case kBlob: {
//            this->value_.blob = Move(other.value_.blob);
//            break;
//        }
        case kEmbedding: {
            this->value_.embedding = Move(other.value_.embedding);
            break;
        }
        case kRowID: {
            value_.row = other.value_.row;
            break;
        }
        case kMixed: {
            this->value_.mixed_value = Move(other.value_.mixed_value);
            break;
        }
        case kNull: {
            // No value for null type
            break;
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Unexpected error!");
            break;
        }
    }
}

void Value::Reset() {
    switch (type_.type()) {
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
        case kDecimal: {
            value_.decimal.Reset();
            break;
        }
        case kVarchar: {
            value_.varchar.Reset();
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
//        case kPath: {
//            value_.path.Reset();
//            break;
//        }
//        case kPolygon: {
//            value_.polygon.Reset();
//            break;
//        }
        case kCircle: {
            value_.circle.Reset();
            break;
        }
//        case kBitmap: {
//            value_.bitmap.Reset();
//            break;
//        }
        case kUuid: {
            value_.uuid.Reset();
            break;
        }
//        case kBlob: {
//            value_.blob.Reset();
//            break;
//        }
        case kEmbedding: {
            if (value_.embedding.ptr != nullptr) {
                LOG_TRACE("Need to manually reset the embedding type.");
            }
            break;
        }
        case kRowID: {
            value_.row = -1;
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

bool Value::TryCastAs(const DataType &target_type, Value &new_value) const {
    BoundCastFunc cast = CastFunction::GetBoundFunc(this->type_, target_type);
    SharedPtr<ColumnVector> source_ptr = MakeShared<ColumnVector>(MakeShared<DataType>(this->type_));
    source_ptr->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    source_ptr->AppendValue(*this);

    SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(MakeShared<DataType>(target_type));
    col_varchar_ptr->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    CastParameters parameters;
    cast.function(source_ptr, col_varchar_ptr, 1, parameters);
    new_value = col_varchar_ptr->GetValue(0);
    return true;
}

String Value::ToString() const {
    switch (type_.type()) {
        case kBoolean: {
            return value_.boolean ? "true" : "false";
        }
        case kTinyInt: {
            return ToStr(value_.tiny_int);
        }
        case kSmallInt: {
            return ToStr(value_.small_int);
        }
        case kInteger: {
            return ToStr(value_.integer);
        }
        case kBigInt: {
            return ToStr(value_.big_int);
        }
        case kHugeInt: {
            return value_.huge_int.ToString();
        }
        case kFloat: {
            return ToStr(value_.float32);
        }
        case kDouble: {
            return ToStr(value_.float64);
        }
        case kDecimal: {
            Error<NotImplementException>("Decimal");
            break;
        }
        case kVarchar: {
            return value_.varchar.ToString();
        }
        case kDate: {
            return value_.date.ToString();
        }
        case kTime: {
            Error<NotImplementException>("Time");
            break;
        }
        case kDateTime: {
            Error<NotImplementException>("DateTime");
            break;
        }
        case kTimestamp: {
            Error<NotImplementException>("Timestamp");
            break;
        }
        case kInterval: {
            return value_.interval.ToString();
        }
        case kArray: {
            Error<NotImplementException>("Array");
            break;
        }
        case kTuple: {
            Error<NotImplementException>("Tuple");
            break;
        }
        case kPoint: {
            Error<NotImplementException>("Point");
            break;
        }
        case kLine: {
            Error<NotImplementException>("Line");
            break;
        }
        case kLineSeg: {
            Error<NotImplementException>("Line Seg");
            break;
        }
        case kBox: {
            Error<NotImplementException>("Box");
            break;
        }
//        case kPath: {
//            Error<NotImplementException>("Path");
//            break;
//        }
//        case kPolygon: {
//            Error<NotImplementException>("Polygon");
//            break;
//        }
        case kCircle: {
            Error<NotImplementException>("Circle");
            break;
        }
//        case kBitmap: {
//            Error<NotImplementException>("Bitmap");
//            break;
//        }
        case kUuid: {
            Error<NotImplementException>("Uuid");
            break;
        }
//        case kBlob: {
//            return value_.blob.ToString();
//        }
        case kEmbedding: {
            Error<NotImplementException>("Embedding");
            break;
        }
        case kRowID: {
            return value_.row.ToString();
        }
        case kMixed: {
            Error<NotImplementException>("Embedding");
            break;
        }
        case kNull:
            break;
        case kMissing:
            break;
        case kInvalid:
            break;
    }
    Error<TypeException>("Unexpected error.");
    return String();
}
} // namespace infinity
