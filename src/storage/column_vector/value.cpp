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

#include <ranges>

module value;

import stl;
import global_resource_usage;
import logger;

import infinity_exception;
import third_party;
import bound_cast_func;
import cast_function;
import column_vector;
import default_values;
import status;
import type_info;

namespace infinity {

// Value maker
Value Value::MakeValue(DataType type) {
    Value value(type.type(), type.type_info());
    return value;
}

Value Value::MakeNull() {
    Value value(LogicalType::kNull);
    return value;
}

Value Value::MakeInvalid() {
    Value value(LogicalType::kInvalid);
    return value;
}

Value Value::MakeBool(BooleanT input) {
    Value value(LogicalType::kBoolean);
    value.value_.boolean = input;
    return value;
}

Value Value::MakeTinyInt(TinyIntT input) {
    Value value(LogicalType::kTinyInt);
    value.value_.tiny_int = input;
    return value;
}

Value Value::MakeSmallInt(SmallIntT input) {
    Value value(LogicalType::kSmallInt);
    value.value_.small_int = input;
    return value;
}

Value Value::MakeInt(IntegerT input) {
    Value value(LogicalType::kInteger);
    value.value_.integer = input;
    return value;
}

Value Value::MakeBigInt(BigIntT input) {
    Value value(LogicalType::kBigInt);
    value.value_.big_int = input;
    return value;
}

Value Value::MakeHugeInt(HugeIntT input) {
    Value value(LogicalType::kHugeInt);
    value.value_.huge_int = input;
    return value;
}

Value Value::MakeFloat(FloatT input) {
    Value value(LogicalType::kFloat);
    value.value_.float32 = input;
    return value;
}

Value Value::MakeDouble(DoubleT input) {
    Value value(LogicalType::kDouble);
    value.value_.float64 = input;
    return value;
}

Value Value::MakeDecimal(DecimalT input, SharedPtr<TypeInfo> type_info_ptr) {
    Value value(LogicalType::kDecimal, type_info_ptr);
    value.value_.decimal = input;
    return value;
}

Value Value::MakeDate(DateT input) {
    Value value(LogicalType::kDate);
    value.value_.date = input;
    return value;
}

Value Value::MakeTime(TimeT input) {
    Value value(LogicalType::kTime);
    value.value_.time = input;
    return value;
}

Value Value::MakeDateTime(DateTimeT input) {
    Value value(LogicalType::kDateTime);
    value.value_.datetime = input;
    return value;
}

Value Value::MakeTimestamp(TimestampT input) {
    Value value(LogicalType::kTimestamp);
    value.value_.timestamp = input;
    return value;
}

Value Value::MakeInterval(IntervalT input) {
    Value value(LogicalType::kInterval);
    value.value_.interval = input;
    return value;
}

Value Value::MakePoint(PointT input) {
    Value value(LogicalType::kPoint);
    value.value_.point = input;
    return value;
}

Value Value::MakeLine(LineT input) {
    Value value(LogicalType::kLine);
    value.value_.line = input;
    return value;
}

Value Value::MakeLineSegment(LineSegT input) {
    Value value(LogicalType::kLineSeg);
    value.value_.line_segment = input;
    return value;
}

Value Value::MakeBox(BoxT input) {
    Value value(LogicalType::kBox);
    value.value_.box = input;
    return value;
}

Value Value::MakeCircle(CircleT input) {
    Value value(LogicalType::kCircle);
    value.value_.circle = input;
    return value;
}

Value Value::MakeUuid(UuidT input) {
    Value value(LogicalType::kUuid);
    value.value_.uuid = std::move(input);
    return value;
}

Value Value::MakeRow(RowID input) {
    Value value(LogicalType::kRowID);
    value.value_.row = input;
    return value;
}

Value Value::MakeVarchar(const String &str) {
    Value value(LogicalType::kVarchar);
    value.value_info_ = MakeShared<StringValueInfo>(str);
    return value;
}

Value Value::MakeVarchar(const char *ptr, SizeT len) {
    String tmp_str(ptr, len);
    Value value(LogicalType::kVarchar);
    value.value_info_ = MakeShared<StringValueInfo>(String(ptr, len));
    return value;
}

Value Value::MakeVarchar(const VarcharT &input) {
    Value value(LogicalType::kVarchar);
    if (input.IsInlined()) {
        String tmp_str(input.short_.data_, input.length_);
        value.value_info_ = MakeShared<StringValueInfo>(std::move(tmp_str));
    } else if (input.IsValue()) {
        String tmp_str(input.value_.ptr_, input.length_);
        value.value_info_ = MakeShared<StringValueInfo>(std::move(tmp_str));
    } else {
        UnrecoverableError("Value::MakeVarchar(VectorVarchar) is unsupported!");
    }
    return value;
}

Value Value::MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        UnrecoverableError(fmt::format("Value::MakeEmbedding(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString()));
    }
    EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_info_ptr.get());
    SizeT len = embedding_info->Size();
    SharedPtr<EmbeddingValueInfo> embedding_value_info = MakeShared<EmbeddingValueInfo>();
    embedding_value_info->data_.resize(len);
    std::memcpy(embedding_value_info->data_.data(), ptr, len);
    Value value(LogicalType::kEmbedding, type_info_ptr);
    value.value_info_ = embedding_value_info;
    return value;
}

// Value getter
template <>
BooleanT Value::GetValue() const {
    if (type_.type() != LogicalType::kBoolean) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.boolean;
}

template <>
TinyIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kTinyInt) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.tiny_int;
}

template <>
SmallIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kSmallInt) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.small_int;
}

template <>
IntegerT Value::GetValue() const {
    if (type_.type() != LogicalType::kInteger) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.integer;
}

template <>
BigIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kBigInt) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.big_int;
}

template <>
HugeIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kHugeInt) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.huge_int;
}

template <>
FloatT Value::GetValue() const {
    if (type_.type() != LogicalType::kFloat) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.float32;
}

template <>
DoubleT Value::GetValue() const {
    if (type_.type() != LogicalType::kDouble) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.float64;
}

template <>
DecimalT Value::GetValue() const {
    if (type_.type() != LogicalType::kDecimal) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.decimal;
}

template <>
DateT Value::GetValue() const {
    if (type_.type() != LogicalType::kDate) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.date;
}

template <>
TimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kTime) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.time;
}

template <>
DateTimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kDateTime) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.datetime;
}

template <>
TimestampT Value::GetValue() const {
    if (type_.type() != LogicalType::kTimestamp) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.timestamp;
}

template <>
IntervalT Value::GetValue() const {
    if (type_.type() != LogicalType::kInterval) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.interval;
}

template <>
PointT Value::GetValue() const {
    if (type_.type() != LogicalType::kPoint) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.point;
}

template <>
LineT Value::GetValue() const {
    if (type_.type() != LogicalType::kLine) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.line;
}

template <>
LineSegT Value::GetValue() const {
    if (type_.type() != LogicalType::kLineSeg) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.line_segment;
}

template <>
BoxT Value::GetValue() const {
    if (type_.type() != LogicalType::kBox) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.box;
}

template <>
CircleT Value::GetValue() const {
    if (type_.type() != LogicalType::kCircle) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.circle;
}

template <>
UuidT Value::GetValue() const {
    if (type_.type() != LogicalType::kUuid) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.uuid;
}

template <>
RowID Value::GetValue() const {
    if (type_.type() != LogicalType::kRowID) {
        UnrecoverableError(fmt::format("Not matched type: {}", type_.ToString()));
    }
    return value_.row;
}

Value::~Value() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Value");
#endif
}

Value::Value(const DataType &data_type) : type_(data_type) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
}

Value::Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr) : type_(type, std::move(typeinfo_ptr)) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
}

Value::Value(const Value &other) : type_(other.type_) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
    CopyUnionValue(other);
}

Value::Value(Value &&other) noexcept : type_(other.type_) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
    MoveUnionValue(std::forward<Value>(other));
}

Value &Value::operator=(const Value &other) {
    if (this == &other)
        return *this;
    this->Reset();
    CopyUnionValue(other);
    return *this;
}

Value &Value::operator=(Value &&other) noexcept {
    this->Reset();
    MoveUnionValue(std::forward<Value>(other));
    return *this;
}

bool Value::operator==(const Value &other) const {
    if (this->type_ != other.type_)
        return false;
    if (this->type_.type() == LogicalType::kInvalid)
        return true;
    switch (this->type_.type()) {
        case kBoolean: {
            return value_.boolean == other.value_.boolean;
        }
        case kTinyInt: {
            return value_.tiny_int == other.value_.tiny_int;
        }
        case kSmallInt: {
            return value_.small_int == other.value_.small_int;
        }
        case kInteger: {
            return value_.integer == other.value_.integer;
        }
        case kBigInt: {
            return value_.big_int == other.value_.big_int;
        }
        case kHugeInt: {
            return value_.huge_int == other.value_.huge_int;
        }
        case kFloat: {
            return value_.float32 == other.value_.float32;
        }
        case kDouble: {
            return value_.float64 == other.value_.float64;
        }
        case kDecimal: {
            return value_.decimal == other.value_.decimal;
        }
        case kDate: {
            return value_.date == other.value_.date;
        }
        case kPoint: {
            return value_.point == other.value_.point;
        }
        case kLine: {
            return value_.line == other.value_.line;
        }
        case kLineSeg: {
            return value_.line_segment == other.value_.line_segment;
        }
        case kBox: {
            return value_.box == other.value_.box;
        }
        case kCircle: {
            return value_.circle == other.value_.circle;
        }
        case kUuid: {
            return value_.uuid == other.value_.uuid;
        }
        case kRowID: {
            return value_.row == other.value_.row;
        }
        case kNull: {
            return true;
        }
        case kVarchar: {
            const String &s1 = this->value_info_->Get<StringValueInfo>().GetString();
            const String &s2 = other.value_info_->Get<StringValueInfo>().GetString();
            return s1 == s2;
        }
        case kEmbedding: {
            const Vector<char> &data1 = this->value_info_->Get<EmbeddingValueInfo>().data_;
            const Vector<char> &data2 = other.value_info_->Get<EmbeddingValueInfo>().data_;
            return std::ranges::equal(data1, data2);
            break;
        }
        default:
            return false;
    }
    return true;
}

void Value::CopyUnionValue(const Value &other) {
    this->type_ = other.type_;
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
            value_.decimal = other.value_.decimal;
            break;
        }
        case kDate: {
            value_.date = other.value_.date;
            break;
        }
        case kTime: {
            value_.time = other.value_.time;
            break;
        }
        case kDateTime: {
            value_.datetime = other.value_.datetime;
            break;
        }
        case kTimestamp: {
            value_.timestamp = other.value_.timestamp;
            break;
        }
        case kInterval: {
            value_.interval = other.value_.interval;
            break;
        }
        case kPoint: {
            value_.point = other.value_.point;
            break;
        }
        case kLine: {
            value_.line = other.value_.line;
            break;
        }
        case kLineSeg: {
            value_.line_segment = other.value_.line_segment;
            break;
        }
        case kBox: {
            value_.box = other.value_.box;
            break;
        }
        case kCircle: {
            value_.circle = other.value_.circle;
            break;
        }
        case kUuid: {
            value_.uuid = other.value_.uuid;
            break;
        }
        case kRowID: {
            value_.row = other.value_.row;
            break;
        }
        case kNull: {
            // No value for null value.
            break;
        }
        case kVarchar:
        case kEmbedding: {
            this->value_info_ = other.value_info_;
            break;
        }
        case kMissing:
        case kInvalid:
        default: {
            LOG_ERROR("Unexpected error!");
            break;
        }
    }
}

void Value::MoveUnionValue(Value &&other) noexcept {
    this->type_ = std::move(other.type_);
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
        case kCircle: {
            this->value_.circle = other.value_.circle;
            break;
        }
        case kUuid: {
            this->value_.uuid = std::move(other.value_.uuid);
            break;
        }
        case kRowID: {
            value_.row = other.value_.row;
            break;
        }
        case kNull: {
            // No value for null type
            break;
        }
        case kVarchar:
        case kEmbedding: {
            this->value_info_ = std::move(other.value_info_);
            break;
        }
        case kMissing:
        case kInvalid:
        default: {
            LOG_ERROR("Unexpected error!");
            break;
        }
    }
}

void Value::Reset() {
    this->value_info_.reset();
    this->type_.Reset();
}

String Value::ToString() const {
    switch (type_.type()) {
        case kBoolean: {
            return value_.boolean ? "true" : "false";
        }
        case kTinyInt: {
            return std::to_string(value_.tiny_int);
        }
        case kSmallInt: {
            return std::to_string(value_.small_int);
        }
        case kInteger: {
            return std::to_string(value_.integer);
        }
        case kBigInt: {
            return std::to_string(value_.big_int);
        }
        case kHugeInt: {
            return value_.huge_int.ToString();
        }
        case kFloat: {
            return std::to_string(value_.float32);
        }
        case kDouble: {
            return std::to_string(value_.float64);
        }
        case kDate: {
            return value_.date.ToString();
        }
        case kTime: {
            return value_.time.ToString();
        }
        case kDateTime: {
            return value_.datetime.ToString();
        }
        case kTimestamp: {
            return value_.timestamp.ToString();
        }
        case kInterval: {
            return value_.interval.ToString();
        }
        case kRowID: {
            return value_.row.ToString();
        }
        case kVarchar: {
            return value_info_->Get<StringValueInfo>().GetString();
        }
        default: {
            UnrecoverableError(fmt::format("Value::ToString() not implemented for type {}", type_.ToString()));
            return {};
        }
    }
    return "";
}

} // namespace infinity
