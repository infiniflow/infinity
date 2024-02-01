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
import parser;
import default_values;
import file_system;
import file_system_type;
import infinity_exception;
import column_vector;
import third_party;
import catalog;
import secondary_index_pgm;
import logical_type;

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

template <typename T>
    requires KeepOrderedSelf<T> or ConvertToOrderedI32<T> or ConvertToOrderedI64<T>
using ConvertToOrderedType = ConvertToOrdered<T>::type;

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
    virtual void AppendColumnVector(const void *append_ptr, u32 append_num) = 0;
    virtual void StartOutput() = 0;
    virtual void EndOutput() = 0;
    virtual void OutputToHeader(SecondaryIndexDataHead *index_head) = 0;
    virtual void OutputToPart(SecondaryIndexDataPart *index_part) = 0;
};

// usage:
//  1. AppendColumnVector(): merge sort, collect all values of the column in the segment
//  2.1. OutputToPart(): copy sorted (key, offset) pairs into several SecondaryIndexDataPart structures.
//  2.2. OutputToHeader(): create PGM index in SecondaryIndexDataHead.
template <typename RawValueType>
class SecondaryIndexDataBuilder final : public SecondaryIndexDataBuilderBase {
public:
    using KeyType = ConvertToOrderedType<RawValueType>;
    using OffsetType = u32;
    using KeyOffsetPair = Pair<KeyType, OffsetType>;

    explicit SecondaryIndexDataBuilder(u32 data_num, u32 input_block_max_row, u32 part_capacity)
        : input_block_max_row_(input_block_max_row), data_num_(data_num), output_part_capacity_(part_capacity) {
        output_part_num_ = (data_num_ + output_part_capacity_ - 1) / output_part_capacity_;
        index_key_type_ = GetLogicalType<KeyType>;
        output_offset_type_ = LogicalType::kInteger; // used to store u32 offsets
        sorted_key_offset_pair_ = MakeUniqueForOverwrite<KeyOffsetPair[]>(data_num_);
        append_key_offset_pair_ = MakeUniqueForOverwrite<KeyOffsetPair[]>(input_block_max_row_);
    }

    ~SecondaryIndexDataBuilder() override final = default;

    void AppendColumnVector(const void *append_ptr, u32 append_num) override final;

    void StartOutput() override final;

    void EndOutput() override final;

    void OutputToHeader(SecondaryIndexDataHead *index_head) override final;

    void OutputToPart(SecondaryIndexDataPart *index_part) override final;

private:
    u32 size_{};                                         // number of pairs in sorted_key_offset_pair_
    u32 input_block_max_row_{};                          // number of rows in each full input block
    const u32 data_num_{};                               // number of rows in the segment
    LogicalType index_key_type_ = LogicalType::kInvalid; // type of ordered keys stored in the raw index
    UniquePtr<KeyOffsetPair[]> sorted_key_offset_pair_;  // size: data_num_. Will be destroyed in OutputToHeader().
    UniquePtr<KeyOffsetPair[]> append_key_offset_pair_;  // size: part_capacity_. Will be destroyed in StartOutput().
private:
    // record output progress
    u32 output_part_capacity_{};                             // number of rows in each full output part
    u32 output_part_num_{};                                  // number of output parts
    u32 output_row_progress_{};                              // record output progress
    u32 output_part_progress_{};                             // record output progress
    LogicalType output_offset_type_ = LogicalType::kInvalid; // type of offset stored in the output part
    UniquePtr<KeyType[]> sorted_keys_;                       // for pgm. Will be created in StartOutput().
};

// create a secondary index on each segment
// now only support index for single column
// now only support create index for POD type with size <= sizeof(i64)
// need to convert values in column into ordered number type
export UniquePtr<SecondaryIndexDataBuilderBase>
GetSecondaryIndexDataBuilder(const SharedPtr<DataType> &data_type, u32 data_num, u32 input_block_max_row, u32 part_capacity);

// includes: metadata and PGM index
class SecondaryIndexDataHead {
    friend class SecondaryIndexDataBuilderBase;
    template <typename ValueT>
    friend class SecondaryIndexDataBuilder;

private:
    bool loaded_{false};  // whether data of this part is in memory
    u32 part_capacity_{}; // number of rows in each full part
    u32 part_num_{};      // number of parts
    u32 data_num_{};      // number of rows in the segment
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
    explicit SecondaryIndexDataHead(u32 part_capacity, u32 data_num, const SharedPtr<DataType> &data_type)
        : part_capacity_(part_capacity), data_num_(data_num) {
        part_num_ = (data_num_ + part_capacity_ - 1) / part_capacity_;
        data_type_raw_ = data_type->type();
    }

    ~SecondaryIndexDataHead() = default;

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

    void SaveIndexInner(FileHandler &file_handler) const;

    void ReadIndexInner(FileHandler &file_handler);
};

} // namespace infinity