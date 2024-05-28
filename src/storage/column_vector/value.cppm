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

export module value;

import stl;
import type_info;
import logical_type;
import infinity_exception;
import internal_types;
import embedding_info;
import data_type;
import knn_expr;
import third_party;
import logger;

namespace infinity {

enum class ExtraValueInfoType : u8 {
    INVALID_TYPE_INFO = 0,
    STRING_VALUE_INFO = 1,
    EMBEDDING_VALUE_INFO = 2,
    TENSORARRAY_VALUE_INFO = 3,
};

//===--------------------------------------------------------------------===//
// Extra Value Info
//===--------------------------------------------------------------------===//
struct ExtraValueInfo {
    explicit ExtraValueInfo(ExtraValueInfoType type) : type_(type) {}
    virtual ~ExtraValueInfo() {}

    ExtraValueInfoType type_;

public:
    bool Equals(ExtraValueInfo *other_p) const {
        if (!other_p) {
            return false;
        }
        if (type_ != other_p->type_) {
            return false;
        }
        return EqualsInternal(other_p);
    }

    template <class T>
    T &Get() {
        if (type_ != T::TYPE) {
            String error_message = "ExtraValueInfo type mismatch";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        return (T &)*this;
    }

protected:
    virtual bool EqualsInternal(ExtraValueInfo *) const { return true; }
};

//===--------------------------------------------------------------------===//
// String Value Info
//===--------------------------------------------------------------------===//
struct StringValueInfo : public ExtraValueInfo {
    static constexpr ExtraValueInfoType TYPE = ExtraValueInfoType::STRING_VALUE_INFO;

public:
    explicit StringValueInfo(const String &str_p) : ExtraValueInfo(ExtraValueInfoType::STRING_VALUE_INFO), str_(str_p) {}
    explicit StringValueInfo(const std::string_view &str_view) : ExtraValueInfo(ExtraValueInfoType::STRING_VALUE_INFO), str_(str_view) {}
    explicit StringValueInfo(String &&str_p) : ExtraValueInfo(ExtraValueInfoType::STRING_VALUE_INFO), str_(std::move(str_p)) {}

    const String &GetString() { return str_; }

protected:
    bool EqualsInternal(ExtraValueInfo *other_p) const override { return IsEqual(str_, other_p->Get<StringValueInfo>().str_); }

    String str_;
};

//===--------------------------------------------------------------------===//
// Embedding Value Info
//===--------------------------------------------------------------------===//
export struct EmbeddingValueInfo : public ExtraValueInfo {
    static constexpr ExtraValueInfoType TYPE = ExtraValueInfoType::EMBEDDING_VALUE_INFO;
    friend struct Value;

public:
    EmbeddingValueInfo() : ExtraValueInfo(ExtraValueInfoType::EMBEDDING_VALUE_INFO) {}

    template <typename T>
    explicit EmbeddingValueInfo(const Vector<T> &values_p) : ExtraValueInfo(ExtraValueInfoType::EMBEDDING_VALUE_INFO) {
        SizeT len = values_p.size() * sizeof(T);
        data_.resize(len);
        std::memcpy(data_.data(), values_p.data(), len);
    }

    template <>
    explicit EmbeddingValueInfo(const Vector<bool> &values_p) : ExtraValueInfo(ExtraValueInfoType::EMBEDDING_VALUE_INFO) {
        SizeT len = values_p.size() / 8;
        data_.resize(len);
        auto *data_ptr = reinterpret_cast<u8 *>(data_.data());
        for (SizeT i = 0; i < values_p.size(); i++) {
            if (values_p[i]) {
                data_ptr[i / 8] |= (1u << (i % 8));
            }
        }
    }

    // Also used for tensor info
    static SharedPtr<EmbeddingValueInfo> MakeTensorValueInfo(const_ptr_t ptr, SizeT bytes);

    Pair<const_ptr_t, SizeT> GetData() const { return MakePair<const_ptr_t, SizeT>(data_.data(), data_.size()); }

protected:
    Vector<char> data_;
};

//===--------------------------------------------------------------------===//
// TensorArray Value Info
//===--------------------------------------------------------------------===//
export struct TensorArrayValueInfo : public ExtraValueInfo {
    static constexpr ExtraValueInfoType TYPE = ExtraValueInfoType::TENSORARRAY_VALUE_INFO;
    friend struct Value;
    TensorArrayValueInfo() : ExtraValueInfo(ExtraValueInfoType::TENSORARRAY_VALUE_INFO) {}
    void AppendTensor(const_ptr_t ptr, SizeT bytes) { member_tensor_data_.emplace_back(EmbeddingValueInfo::MakeTensorValueInfo(ptr, bytes)); }
    Vector<SharedPtr<EmbeddingValueInfo>> member_tensor_data_;
};

export struct Value {
    // class member
public:
    // Value creator from different type of input
    static Value MakeValue(DataType type);

    static Value MakeNull();

    static Value MakeInvalid();

    static Value MakeBool(BooleanT input);

    static Value MakeTinyInt(TinyIntT input);

    static Value MakeSmallInt(SmallIntT input);

    static Value MakeInt(IntegerT input);

    static Value MakeBigInt(BigIntT input);

    static Value MakeHugeInt(HugeIntT input);

    static Value MakeFloat(FloatT input);

    static Value MakeDouble(DoubleT input);

    static Value MakeDecimal(DecimalT input, SharedPtr<TypeInfo> type_info_ptr);

    static Value MakeDate(DateT input);

    static Value MakeTime(TimeT input);

    static Value MakeDateTime(DateTimeT input);

    static Value MakeTimestamp(TimestampT input);

    static Value MakeInterval(IntervalT input);

    static Value MakePoint(PointT input);

    static Value MakeLine(LineT input);

    static Value MakeLineSegment(LineSegT input);

    static Value MakeBox(BoxT input);

    //    static Value MakePath(PathT input);
    //
    //    static Value MakePolygon(PolygonT input);

    static Value MakeCircle(CircleT input);

    //    static Value MakeBitmap(BitmapT input);

    static Value MakeUuid(UuidT input);

    //    static Value MakeBlob(BlobT input);

    static Value MakeRow(RowID input);

    // static Value MakeVarchar(const String &str);

    static Value MakeVarchar(const std::string_view str_view);

    static Value MakeVarchar(const char *ptr, SizeT len);

    static Value MakeVarchar(const VarcharT &input_ref);

    template <class T>
    static Value MakeEmbedding(const Vector<T> &vec) {
        auto embedding_info_ptr = EmbeddingInfo::Make(ToEmbeddingDataType<T>(), vec.size());
        Value value(LogicalType::kEmbedding, embedding_info_ptr);
        value.value_info_ = MakeShared<EmbeddingValueInfo>(vec);
        if constexpr (std::is_same_v<T, bool>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemBit, vec.size()));
        } else if constexpr (std::is_same_v<T, i8>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemInt8, vec.size()));
        } else if constexpr (std::is_same_v<T, i16>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemInt16, vec.size()));
        } else if constexpr (std::is_same_v<T, i32>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemInt32, vec.size()));
        } else if constexpr (std::is_same_v<T, i64>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, vec.size()));
        } else if constexpr (std::is_same_v<T, float>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, vec.size()));
        } else if constexpr (std::is_same_v<T, double>) {
            value.type_ = DataType(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemDouble, vec.size()));
        } else {
            UnrecoverableError("Not supported embedding data type.");
        }

        return value;
    }

    static Value MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> type_info_ptr);

    static Value MakeTensor(const_ptr_t ptr, SizeT bytes, SharedPtr<TypeInfo> type_info_ptr);

    static Value MakeTensorArray(SharedPtr<TypeInfo> type_info_ptr);

    void AppendToTensorArray(const_ptr_t ptr, SizeT bytes);

    // Object member
public:
    // Value getter template for all types in union
    template <class T>
    T GetValue() const {
        UnrecoverableError("Not implemented value getter.");
        return T();
    }

    // Value getter for each type outside union
    const String &GetVarchar() const { return this->value_info_->Get<StringValueInfo>().GetString(); }

    Pair<const_ptr_t, SizeT> GetEmbedding() const { return this->value_info_->Get<EmbeddingValueInfo>().GetData(); }

    const Vector<SharedPtr<EmbeddingValueInfo>> &GetTensorArray() const { return this->value_info_->Get<TensorArrayValueInfo>().member_tensor_data_; }

    [[nodiscard]] const DataType &type() const { return type_; }

    [[nodiscard]] String ToString() const;

    void Reset();

    void AppendToJson(const String& name, nlohmann::json& json);

    // Member method
public:
    explicit Value(const DataType &type);
    explicit Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr = nullptr);
    Value(const Value &other);
    Value(Value &&other) noexcept;
    ~Value();

    Value &operator=(const Value &other);
    Value &operator=(Value &&other) noexcept;
    bool operator==(const Value &other) const;
    bool operator!=(const Value &other) const { return (*this) == other; }

private:
    void CopyUnionValue(const Value &other);
    void MoveUnionValue(Value &&other) noexcept;

public:
    DataType type_;
    union UnionValue {
        BooleanT boolean;
        TinyIntT tiny_int;
        SmallIntT small_int;
        IntegerT integer;
        BigIntT big_int;
        HugeIntT huge_int;
        FloatT float32;
        DoubleT float64;
        DecimalT decimal;
        DateT date;
        TimeT time;
        DateTimeT datetime;
        TimestampT timestamp;
        IntervalT interval;
        PointT point;
        LineT line;
        LineSegT line_segment;
        BoxT box;
        CircleT circle;
        UuidT uuid;
        RowID row;
    } value_ = {};
    SharedPtr<ExtraValueInfo> value_info_ = {}; // NOLINT
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
PointT Value::GetValue() const;

template <>
LineT Value::GetValue() const;

template <>
LineSegT Value::GetValue() const;

template <>
BoxT Value::GetValue() const;

// template <>
// PathT Value::GetValue() const;
//
// template <>
// PolygonT Value::GetValue() const;

template <>
CircleT Value::GetValue() const;

// template <>
// BitmapT Value::GetValue() const;

template <>
UuidT Value::GetValue() const;

// template <>
// BlobT Value::GetValue() const;

template <>
RowID Value::GetValue() const;

} // namespace infinity