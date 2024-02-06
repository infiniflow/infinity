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

#include <type_traits>

export module secondary_index_data;

import stl;

import default_values;
import file_system;
import file_system_type;
import infinity_exception;
import column_vector;
import third_party;
import buffer_manager;
import secondary_index_pgm;
import logical_type;
import internal_types;
import data_type;
import segment_entry;

namespace infinity {

template <typename T>
concept KeepOrderedSelf = IsAnyOf<T, TinyIntT, SmallIntT, IntegerT, BigIntT, FloatT, DoubleT>;

template <typename T>
concept ConvertToOrderedI32 = IsAnyOf<T, DateT, TimeT>;

template <typename T>
concept ConvertToOrderedI64 = IsAnyOf<T, DateTimeT, TimestampT>;

template <typename ValueT>
struct ConvertToOrdered {
    static_assert(false, "type not supported");
};

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

export template <typename T>
    requires KeepOrderedSelf<T> or ConvertToOrderedI32<T> or ConvertToOrderedI64<T>
using ConvertToOrderedType = ConvertToOrdered<T>::type;

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

template <typename T>
LogicalType GetLogicalType = kInvalid;

template <>
LogicalType GetLogicalType<FloatT> = LogicalType::kFloat;

template <>
LogicalType GetLogicalType<DoubleT> = LogicalType::kDouble;

template <>
LogicalType GetLogicalType<TinyIntT> = LogicalType::kTinyInt;

template <>
LogicalType GetLogicalType<SmallIntT> = LogicalType::kSmallInt;

template <>
LogicalType GetLogicalType<IntegerT> = LogicalType::kInteger;

template <>
LogicalType GetLogicalType<BigIntT> = LogicalType::kBigInt;

export class SecondaryIndexDataHead;

export class SecondaryIndexDataPart;

class SecondaryIndexDataBuilderBase {
public:
    SecondaryIndexDataBuilderBase() = default;
    virtual ~SecondaryIndexDataBuilderBase() = default;
    virtual void
    LoadSegmentData(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp begin_ts, bool check_ts) = 0;
    virtual void StartOutput() = 0;
    virtual void EndOutput() = 0;
    virtual void OutputToHeader(SecondaryIndexDataHead *index_head) = 0;
    virtual void OutputToPart(SecondaryIndexDataPart *index_part) = 0;
};

// create a secondary index on each segment
// now only support index for single column
// now only support create index for POD type with size <= sizeof(i64)
// need to convert values in column into ordered number type
// data_num : number of rows in the segment, except those deleted
export UniquePtr<SecondaryIndexDataBuilderBase>
GetSecondaryIndexDataBuilder(const SharedPtr<DataType> &data_type, u32 full_data_num, u32 data_num, u32 part_capacity);

// includes: metadata and PGM index
class SecondaryIndexDataHead {
    friend class SecondaryIndexDataBuilderBase;
    template <typename ValueT>
    friend class SecondaryIndexDataBuilder;

private:
    bool loaded_{false};  // whether data of this part is in memory
    u32 part_capacity_{}; // number of rows in each full part
    u32 part_num_{};      // number of parts
    u32 full_data_num_{}; // number of rows in the segment (include those deleted)
    u32 data_num_{};      // number of rows in the segment (except those deleted) when the index is created
    // sorted values in segment
    LogicalType data_type_raw_ = LogicalType::kInvalid; // type of original data
    LogicalType data_type_key_ = LogicalType::kInvalid; // type of data stored in the raw index
    // offset of each value in segment
    // type: IntegerT (its size matches with the type u32 of the segment_offset_)
    LogicalType data_type_offset_ = LogicalType::kInvalid;
    // pgm index
    UniquePtr<SecondaryPGMIndex> pgm_index_;

public:
    // will be called when an old index is loaded
    // used in SecondaryIndexFileWorker::ReadFromFileImpl()
    SecondaryIndexDataHead() = default;

    // will be called when a new index is created
    // used in SecondaryIndexFileWorker::AllocateInMemory()
    explicit SecondaryIndexDataHead(u32 part_capacity, u32 full_data_num, u32 data_num, const SharedPtr<DataType> &data_type)
        : part_capacity_(part_capacity), full_data_num_(full_data_num), data_num_(data_num) {
        part_num_ = (full_data_num + part_capacity_ - 1) / part_capacity_;
        data_type_raw_ = data_type->type();
    }

    ~SecondaryIndexDataHead() = default;

    [[nodiscard]] u32 GetPartCapacity() const { return part_capacity_; }
    [[nodiscard]] u32 GetPartNum() const { return part_num_; }
    [[nodiscard]] u32 GetDataNum() const { return data_num_; }

    [[nodiscard]] auto SearchPGM(const void *val_ptr) const {
        if (!pgm_index_) {
            UnrecoverableError("Not initialized yet.");
        }
        return pgm_index_->SearchIndex(val_ptr);
    }

    void SaveIndexInner(FileHandler &file_handler) const;

    void ReadIndexInner(FileHandler &file_handler);
};

// an index may include several parts
// includes: a part of keys and corresponding offsets in the segment
class SecondaryIndexDataPart {
    friend class SecondaryIndexDataBuilderBase;
    template <typename ValueT>
    friend class SecondaryIndexDataBuilder;

private:
    bool loaded_{false}; // whether data of this part is in memory
    u32 part_id_{};      // id of this part
    u32 part_size_{};    // number of rows in this part
    // data type
    LogicalType data_type_key_ = LogicalType::kInvalid;
    LogicalType data_type_offset_ = LogicalType::kInvalid;
    // key-offset pairs
    UniquePtr<ColumnVector> column_key_;
    UniquePtr<ColumnVector> column_offset_;

public:
    // will be called when an old index is loaded
    // used in SecondaryIndexFileWorker::ReadFromFileImpl()
    SecondaryIndexDataPart() = default;

    // will be called when a new index is created
    // used in SecondaryIndexFileWorker::AllocateInMemory()
    explicit SecondaryIndexDataPart(u32 part_id, u32 part_size) : part_id_(part_id), part_size_(part_size) {}

    ~SecondaryIndexDataPart() = default;

    [[nodiscard]] u32 GetPartId() const { return part_id_; }

    [[nodiscard]] u32 GetPartSize() const { return part_size_; }

    [[nodiscard]] const void *GetColumnKeyData() const { return column_key_->data(); }

    [[nodiscard]] const void *GetColumnOffsetData() const { return column_offset_->data(); }

    void SaveIndexInner(FileHandler &file_handler) const;

    void ReadIndexInner(FileHandler &file_handler);
};

} // namespace infinity