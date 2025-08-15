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

export module infinity_core:secondary_index_data;

import :default_values;
import :local_file_handle;
import :infinity_exception;
import :column_vector;
import :secondary_index_pgm;
import :buffer_handle;
import :logger;

import third_party;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

class BufferObj;
class TableIndexMeeta;

template <typename T>
concept KeepOrderedSelf = IsAnyOf<T, TinyIntT, SmallIntT, IntegerT, BigIntT, FloatT, DoubleT>;

template <typename T>
concept ConvertToOrderedI32 = IsAnyOf<T, DateT, TimeT>;

template <typename T>
concept ConvertToOrderedI64 = IsAnyOf<T, DateTimeT, TimestampT>;

template <typename T>
concept ConvertToHashU64 = IsAnyOf<T, VarcharT, std::string_view>;

template <typename ValueT>
struct ConvertToOrdered;

template <KeepOrderedSelf T>
struct ConvertToOrdered<T> {
    using type = T;
};

template <ConvertToOrderedI32 T>
struct ConvertToOrdered<T> {
    using type = i32;
};

template <ConvertToOrderedI64 T>
struct ConvertToOrdered<T> {
    using type = i64;
};

template <ConvertToHashU64 T>
struct ConvertToOrdered<T> {
    using type = u64;
};

export template <typename T>
    requires KeepOrderedSelf<T> or ConvertToOrderedI32<T> or ConvertToOrderedI64<T> or ConvertToHashU64<T>
using ConvertToOrderedType = typename ConvertToOrdered<T>::type;

export template <typename RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    static_assert(false, "type not supported");
}

export template <KeepOrderedSelf RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value;
}

// DateT, TimeT
export template <ConvertToOrderedI32 RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value.GetValue();
}

// DateTimeT, TimestampT
export template <ConvertToOrderedI64 RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value.GetEpochTime();
}

// for VarcharT
export template <>
ConvertToOrderedType<std::string_view> ConvertToOrderedKeyValue(std::string_view value) {
    return std::hash<std::string_view>{}(value);
}

export template <typename T>
constexpr LogicalType GetLogicalType = LogicalType::kInvalid;

template <>
constexpr LogicalType GetLogicalType<TinyIntT> = LogicalType::kTinyInt;

template <>
constexpr LogicalType GetLogicalType<SmallIntT> = LogicalType::kSmallInt;

template <>
constexpr LogicalType GetLogicalType<IntegerT> = LogicalType::kInteger;

template <>
constexpr LogicalType GetLogicalType<BigIntT> = LogicalType::kBigInt;

template <>
constexpr LogicalType GetLogicalType<FloatT> = LogicalType::kFloat;

template <>
constexpr LogicalType GetLogicalType<DoubleT> = LogicalType::kDouble;

template <>
constexpr LogicalType GetLogicalType<DateT> = LogicalType::kDate;

template <>
constexpr LogicalType GetLogicalType<TimeT> = LogicalType::kTime;

template <>
constexpr LogicalType GetLogicalType<DateTimeT> = LogicalType::kDateTime;

template <>
constexpr LogicalType GetLogicalType<TimestampT> = LogicalType::kTimestamp;

template <>
constexpr LogicalType GetLogicalType<VarcharT> = LogicalType::kVarchar;

// Cardinality tag types for template specialization
export struct HighCardinalityTag {};
export struct LowCardinalityTag {};

// Template base class for SecondaryIndexData with cardinality parameter
export template <typename CardinalityTag>
class SecondaryIndexDataBase {
protected:
    u32 chunk_row_count_ = 0;
    // pgm index
    std::unique_ptr<SecondaryPGMIndex> pgm_index_;
    // k-v data
    void *key_ptr_ = nullptr;
    SegmentOffset *offset_ptr_ = nullptr;

public:
    explicit SecondaryIndexDataBase(u32 chunk_row_count) : chunk_row_count_(chunk_row_count) {}

    virtual ~SecondaryIndexDataBase() = default;

    [[nodiscard]] std::pair<const void *, const SegmentOffset *> GetKeyOffsetPointer() const { return {key_ptr_, offset_ptr_}; }

    [[nodiscard]] inline auto SearchPGM(const void *val_ptr) const {
        if (!pgm_index_) {
            UnrecoverableError("Not initialized yet.");
        }
        return pgm_index_->SearchIndex(val_ptr);
    }

    [[nodiscard]] inline u32 GetChunkRowCount() const { return chunk_row_count_; }

    virtual void SaveIndexInner(LocalFileHandle &file_handle) const = 0;

    virtual void ReadIndexInner(LocalFileHandle &file_handle) = 0;

    virtual void InsertData(const void *ptr) = 0;

    virtual void InsertMergeData(const std::vector<std::pair<u32, BufferObj *>> &old_chunks) = 0;

    // Virtual methods for low cardinality access (default implementations for high cardinality)
    virtual u32 GetUniqueKeyCount() const { return 0; }
    virtual const void *GetUniqueKeysPtr() const { return nullptr; }
    virtual const void *GetOffsetsForKeyPtr(const void *key_ptr) const { return nullptr; }
};

// Type aliases for backward compatibility
export using SecondaryIndexData = SecondaryIndexDataBase<HighCardinalityTag>;

export SecondaryIndexDataBase<HighCardinalityTag> *GetSecondaryIndexData(const std::shared_ptr<DataType> &data_type, u32 chunk_row_count, bool allocate);

// New factory function that can create both cardinality variants
export template <typename CardinalityTag>
SecondaryIndexDataBase<CardinalityTag> *
GetSecondaryIndexDataWithCardinality(const std::shared_ptr<DataType> &data_type, u32 chunk_row_count, bool allocate);

// Factory function that determines cardinality from TableIndexMeeta
export void *
GetSecondaryIndexDataWithMeeta(const std::shared_ptr<DataType> &data_type, u32 chunk_row_count, bool allocate, TableIndexMeeta *table_index_meeta);

} // namespace infinity
