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

namespace {
template <typename T>
void Embedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json& embedding_json) {
    for (size_t i = 0; i < dimension; ++i) {
        embedding_json.push_back(((T *)(embedding.ptr))[i]);
    }
}

void BitmapEmbedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json& embedding_json) {
    if(dimension % 8 != 0) {
        Status status = Status::SyntaxError("Binary embedding dimension should be the times of 8.");
        RecoverableError(status);
    }

    auto *array = reinterpret_cast<const u8*>(embedding.ptr);
    for (size_t i = 0; i < dimension / 8; ++i) {
        const u8 byte = array[i];
        for (size_t j = 0; j < 8; ++j) {
            int8_t elem = (byte & (1 << j)) ? 1 : 0;
            embedding_json.push_back(elem);
        }
    }
}

void Embedding2Json(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension, nlohmann::json& embedding_json) {
    switch (type) {
        case kElemBit: {
            BitmapEmbedding2JsonInternal(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt8: {
            Embedding2JsonInternal<int8_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt16: {
            Embedding2JsonInternal<int16_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt32: {
            Embedding2JsonInternal<int32_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt64: {
            Embedding2JsonInternal<int64_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemFloat: {
            Embedding2JsonInternal<float>(embedding, dimension, embedding_json);
            break;
        }
        case kElemDouble: {
            Embedding2JsonInternal<double>(embedding, dimension, embedding_json);
            break;
        }
        default: {
            Status status = Status::SyntaxError("Unexpected embedding type");
            RecoverableError(status);
        }
    }
}

}

// Value maker
Value Value::MakeValue(DataType type) {
    Value value(type.type(), type.type_info());
    return value;
}

Value Value::MakeNull() {
    Value value(LogicalType::kNull);
    return value;
}

Value Value::MakeEmptyArray() {
    Value value(LogicalType::kEmptyArray);
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

//Value Value::MakeVarchar(const String &str) {
//    Value value(LogicalType::kVarchar);
//    value.value_info_ = MakeShared<StringValueInfo>(str);
//    return value;
//}

Value Value::MakeVarchar(const std::string_view str_view) {
    Value value(LogicalType::kVarchar);
    value.value_info_ = MakeShared<StringValueInfo>(str_view);
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
    } else {
        String error_message = "Value::MakeVarchar(VectorVarchar) is unsupported!";
        UnrecoverableError(error_message);
    }
    return value;
}

Value Value::MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeEmbedding(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_info_ptr.get());
    SizeT len = embedding_info->Size();
    SharedPtr<EmbeddingValueInfo> embedding_value_info = MakeShared<EmbeddingValueInfo>(ptr, len);
    Value value(LogicalType::kEmbedding, type_info_ptr);
    value.value_info_ = embedding_value_info;
    return value;
}

Value Value::MakeTensor(const_ptr_t ptr, SizeT bytes, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeTensor(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_info_ptr.get());
    if (const SizeT len = embedding_info->Size(); len == 0 or bytes % len != 0) {
        Status status = Status::SyntaxError(fmt::format("Value::MakeTensor(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
        RecoverableError(status);
    }
    Value value(LogicalType::kTensor, std::move(type_info_ptr));
    value.value_info_ = EmbeddingValueInfo::MakeTensorValueInfo(ptr, bytes);
    return value;
}

Value Value::MakeTensorArray(SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeTensorArray(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_info_ptr.get());
    if (const SizeT len = embedding_info->Size(); len == 0) {
        Status status = Status::SyntaxError(fmt::format("Value::MakeTensorArray(unit embedding bytes = {}) is invalid", len));
        RecoverableError(std::move(status));
    }
    Value value(LogicalType::kTensorArray, std::move(type_info_ptr));
    value.value_info_ = MakeShared<TensorArrayValueInfo>();
    return value;
}

Value Value::MakeSparse(const char *raw_ptr, SizeT nnz, const SharedPtr<TypeInfo> type_info) {
    const auto *sparse_info = static_cast<const SparseInfo *>(type_info.get());

    const char *raw_indice_ptr = raw_ptr;
    SizeT raw_indice_len = sparse_info->IndiceSize(nnz);
    const char *raw_data_ptr = raw_ptr + raw_indice_len;
    SizeT raw_data_len = sparse_info->DataSize(nnz);
    Value value(LogicalType::kSparse, type_info);
    value.value_info_ = MakeShared<SparseValueInfo>(nnz, raw_indice_ptr, raw_indice_len, raw_data_ptr, raw_data_len);
    return value;
}

Value Value::MakeSparse(SizeT nnz, UniquePtr<char[]> indice_ptr, UniquePtr<char[]> data_ptr, const SharedPtr<TypeInfo> type_info) {
    Value value(LogicalType::kSparse, type_info);
    const auto *sparse_info = static_cast<const SparseInfo *>(type_info.get());
    SizeT indice_len = sparse_info->IndiceSize(nnz);
    SizeT data_len = sparse_info->DataSize(nnz);
    value.value_info_ = MakeShared<SparseValueInfo>(nnz, std::move(indice_ptr), indice_len, std::move(data_ptr), data_len);
    return value;
}

void Value::AppendToTensorArray(const_ptr_t ptr, SizeT bytes) {
    if (type_.type() != LogicalType::kTensorArray) {
        String error_message = fmt::format("Value::AppendToTensorArray() is not supported for type {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    const EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_.type_info().get());
    if (const SizeT len = embedding_info->Size(); len == 0 or bytes % len != 0) {
        Status status =
            Status::SyntaxError(fmt::format("Value::AppendToTensorArray(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
        RecoverableError(std::move(status));
    }
    auto &tensor_array_info = value_info_->Get<TensorArrayValueInfo>();
    tensor_array_info.AppendTensor(ptr, bytes);
}

// Value getter
template <>
BooleanT Value::GetValue() const {
    if (type_.type() != LogicalType::kBoolean) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.boolean;
}

template <>
TinyIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kTinyInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.tiny_int;
}

template <>
SmallIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kSmallInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.small_int;
}

template <>
IntegerT Value::GetValue() const {
    if (type_.type() != LogicalType::kInteger) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.integer;
}

template <>
BigIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kBigInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.big_int;
}

template <>
HugeIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kHugeInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.huge_int;
}

template <>
FloatT Value::GetValue() const {
    if (type_.type() != LogicalType::kFloat) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.float32;
}

template <>
DoubleT Value::GetValue() const {
    if (type_.type() != LogicalType::kDouble) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.float64;
}

template <>
DecimalT Value::GetValue() const {
    if (type_.type() != LogicalType::kDecimal) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.decimal;
}

template <>
DateT Value::GetValue() const {
    if (type_.type() != LogicalType::kDate) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.date;
}

template <>
TimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kTime) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.time;
}

template <>
DateTimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kDateTime) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.datetime;
}

template <>
TimestampT Value::GetValue() const {
    if (type_.type() != LogicalType::kTimestamp) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.timestamp;
}

template <>
IntervalT Value::GetValue() const {
    if (type_.type() != LogicalType::kInterval) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.interval;
}

template <>
PointT Value::GetValue() const {
    if (type_.type() != LogicalType::kPoint) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.point;
}

template <>
LineT Value::GetValue() const {
    if (type_.type() != LogicalType::kLine) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.line;
}

template <>
LineSegT Value::GetValue() const {
    if (type_.type() != LogicalType::kLineSeg) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.line_segment;
}

template <>
BoxT Value::GetValue() const {
    if (type_.type() != LogicalType::kBox) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.box;
}

template <>
CircleT Value::GetValue() const {
    if (type_.type() != LogicalType::kCircle) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.circle;
}

template <>
UuidT Value::GetValue() const {
    if (type_.type() != LogicalType::kUuid) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.uuid;
}

template <>
RowID Value::GetValue() const {
    if (type_.type() != LogicalType::kRowID) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
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
        case kTime: {
            return value_.time == other.value_.time;
        }
        case kDateTime: {
            return value_.datetime == other.value_.datetime;
        }
        case kTimestamp: {
            return value_.timestamp == other.value_.timestamp;
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
        case kEmptyArray:
        case kNull: {
            return true;
        }
        case kVarchar: {
            const String &s1 = this->value_info_->Get<StringValueInfo>().GetString();
            const String &s2 = other.value_info_->Get<StringValueInfo>().GetString();
            return s1 == s2;
        }
        case kEmbedding:
        case kTensor: {
            const Span<char> &data1 = this->GetEmbedding();
            const Span<char> &data2 = other.GetEmbedding();
            return std::ranges::equal(data1, data2);
        }
        case kTensorArray: {
            // TODO
            String error_message = "Not implemented yet.";
            UnrecoverableError(error_message);
            break;
        }
        case kSparse: {
            // TODO
            String error_message = "Not implemented yet.";
            UnrecoverableError(error_message);
            break;
        }
        case kInterval:
        case kArray:
        case kTuple:
        case kMixed:
        case kMissing:
        case kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
            return false;
        }
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
        case kEmptyArray:
        case kNull: {
            // No value for null value.
            break;
        }
        case kVarchar:
        case kTensor:
        case kTensorArray:
        case kEmbedding:
        case kSparse: {
            this->value_info_ = other.value_info_;
            break;
        }
        case kArray:
        case kTuple:
        case kMixed:
        case kMissing:
        case kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
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
        case kEmptyArray:
        case kNull: {
            // No value for null type
            break;
        }
        case kVarchar:
        case kTensor:
        case kTensorArray:
        case kEmbedding:
        case kSparse: {
            this->value_info_ = std::move(other.value_info_);
            break;
        }
        case kArray:
        case kTuple:
        case kMixed:
        case kMissing:
        case kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
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
        case LogicalType::kBoolean: {
            return value_.boolean ? "true" : "false";
        }
        case LogicalType::kTinyInt: {
            return std::to_string(value_.tiny_int);
        }
        case LogicalType::kSmallInt: {
            return std::to_string(value_.small_int);
        }
        case LogicalType::kInteger: {
            return std::to_string(value_.integer);
        }
        case LogicalType::kBigInt: {
            return std::to_string(value_.big_int);
        }
        case LogicalType::kHugeInt: {
            return value_.huge_int.ToString();
        }
        case LogicalType::kFloat: {
            return fmt::format("{}", value_.float32);
        }
        case LogicalType::kDouble: {
            return fmt::format("{}", value_.float64);
        }
        case LogicalType::kDate: {
            return value_.date.ToString();
        }
        case LogicalType::kTime: {
            return value_.time.ToString();
        }
        case LogicalType::kDateTime: {
            return value_.datetime.ToString();
        }
        case LogicalType::kTimestamp: {
            return value_.timestamp.ToString();
        }
        case LogicalType::kInterval: {
            return value_.interval.ToString();
        }
        case LogicalType::kRowID: {
            return value_.row.ToString();
        }
        case LogicalType::kVarchar: {
            return value_info_->Get<StringValueInfo>().GetString();
        }
        case LogicalType::kEmbedding: {
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            if (data_span.size() != embedding_info->Size()) {
                String error_message = "Embedding data size mismatch.";
                UnrecoverableError(error_message);
            }
            const EmbeddingT embedding(const_cast<char *>(data_span.data()), false);
            return EmbeddingT::Embedding2String(embedding, embedding_info->Type(), embedding_info->Dimension());
        }
        case LogicalType::kTensor: {
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            SizeT data_bytes = data_span.size();
            const auto basic_embedding_bytes = embedding_info->Size();
            if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
                String error_message = "Tensor data size mismatch.";
                UnrecoverableError(error_message);
            }
            const auto embedding_num = data_bytes / basic_embedding_bytes;
            return TensorT::Tensor2String(const_cast<char *>(data_span.data()), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
        }
        case LogicalType::kTensorArray: {
            // TODO
            String error_message = "Not implemented yet.";
            UnrecoverableError(error_message);
            return {};
        }
        case LogicalType::kSparse: {
            auto *sparse_info = static_cast<SparseInfo *>(type_.type_info().get());
            auto [nnz, indice_span, data_span] = this->GetSparse();
            return SparseT::Sparse2String(data_span.data(), indice_span.data(), sparse_info->DataType(), sparse_info->IndexType(), nnz);
        }
        case LogicalType::kEmptyArray: {
            return "[]";
        }
        case LogicalType::kDecimal:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = fmt::format("Value::ToString() not implemented for type {}", type_.ToString());
            UnrecoverableError(error_message);
            return {};
        }
    }
    return {};
}

void Value::AppendToJson(const String &name, nlohmann::json &json) {
    switch (type_.type()) {
        case LogicalType::kBoolean: {
            json[name] = value_.boolean;
            return;
        }
        case LogicalType::kTinyInt: {
            json[name] = value_.tiny_int;
            return;
        }
        case LogicalType::kSmallInt: {
            json[name] = value_.small_int;
            return;
        }
        case LogicalType::kInteger: {
            json[name] = value_.integer;
            return;
        }
        case LogicalType::kBigInt: {
            json[name] = value_.big_int;
            return;
        }
        case LogicalType::kFloat: {
            json[name] = value_.float32;
            return;
        }
        case LogicalType::kDouble: {
            json[name] = value_.float64;
            return;
        }
        case LogicalType::kDate: {
            json[name] = value_.date.ToString();
            return;
        }
        case LogicalType::kTime: {
            json[name] = value_.time.ToString();
            return;
        }
        case LogicalType::kDateTime: {
            json[name] = value_.datetime.ToString();
            return;
        }
        case LogicalType::kTimestamp: {
            json[name] = value_.timestamp.ToString();
            return;
        }
        case LogicalType::kInterval: {
            json[name] = value_.interval.ToString();
            return;
        }
        case LogicalType::kRowID: {
            json[name] = value_.row.ToString();
            return;
        }
        case LogicalType::kVarchar: {
            json[name] = value_info_->Get<StringValueInfo>().GetString();
            return;
        }
        case LogicalType::kEmbedding: {
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            if (data_span.size() != embedding_info->Size()) {
                String error_message = "Embedding data size mismatch.";
                UnrecoverableError(error_message);
            }
            const EmbeddingT embedding(const_cast<char *>(data_span.data()), false);
            Embedding2Json(embedding, embedding_info->Type(), embedding_info->Dimension(), json[name]);
            return;
        }
        case LogicalType::kTensor: {
            EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            SizeT data_bytes = data_span.size();
            const auto basic_embedding_bytes = embedding_info->Size();
            if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
                String error_message = "Tensor data size mismatch.";
                UnrecoverableError(error_message);
            }
            const auto embedding_num = data_bytes / basic_embedding_bytes;
            json[name] =
                TensorT::Tensor2String(const_cast<char *>(data_span.data()), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
            return;
        }
        case LogicalType::kTensorArray: {
            // TODO
            String error_message = "Not implemented yet.";
            UnrecoverableError(error_message);
        }
        case LogicalType::kEmptyArray: {
            json[name] = "[]";
            return;
        }
        case LogicalType::kSparse: {
            json[name] = "[]";
            return;
        }
        default: {
            String error_message = fmt::format("Value::AppendToJson() not implemented for type {}", type_.ToString());
            UnrecoverableError(error_message);
        }
    }
}

SharedPtr<EmbeddingValueInfo> EmbeddingValueInfo::MakeTensorValueInfo(const_ptr_t ptr, SizeT bytes) {
    if (bytes == 0) {
        String error_message = "EmbeddingValueInfo::MakeTensorValueInfo(bytes=0) is invalid.";
        UnrecoverableError(error_message);
    }
    if (bytes > DEFAULT_FIXLEN_TENSOR_CHUNK_SIZE) {
        Status status = Status::SyntaxError(fmt::format("EmbeddingValueInfo::MakeTensorValueInfo(bytes={}) is larger than the maximum tensor size={}",
                                                        bytes,
                                                        DEFAULT_FIXLEN_TENSOR_CHUNK_SIZE));
        RecoverableError(status);
    }
    return MakeShared<EmbeddingValueInfo>(ptr, bytes);
}

} // namespace infinity
