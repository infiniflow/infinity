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

#include <concepts>
#include <vector>

module secondary_index_data;

import stl;

import default_values;
import index_base;
import file_system;
import file_system_type;
import infinity_exception;
import third_party;
import secondary_index_pgm;
import logger;
import chunk_index_entry;
import buffer_handle;

namespace infinity {

template <typename RawValueType>
class SecondaryIndexDataT final : public SecondaryIndexData {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    // sorted values in chunk
    // only for build and save
    // should not be loaded from file
    bool need_save_ = false;
    UniquePtr<OrderedKeyType[]> key_;
    UniquePtr<SegmentOffset[]> offset_;

public:
    static constexpr u32 PairSize = sizeof(OrderedKeyType) + sizeof(SegmentOffset);

    SecondaryIndexDataT(const u32 chunk_row_count, const bool allocate) : SecondaryIndexData(chunk_row_count) {
        pgm_index_ = GenerateSecondaryPGMIndex<OrderedKeyType>();
        if (allocate) {
            need_save_ = true;
            LOG_TRACE(fmt::format("SecondaryIndexDataT(): Allocate space for chunk_row_count_: {}", chunk_row_count_));
            key_ = MakeUnique<OrderedKeyType[]>(chunk_row_count_);
            offset_ = MakeUnique<SegmentOffset[]>(chunk_row_count_);
        }
    }

    void SaveIndexInner(FileHandler &file_handler) const override {
        if (!need_save_) {
            UnrecoverableError("SaveIndexInner(): error: SecondaryIndexDataT is not allocated.");
        }
        pgm_index_->SaveIndex(file_handler);
    }

    void ReadIndexInner(FileHandler &file_handler) override { pgm_index_->LoadIndex(file_handler); }

    void InsertData(void *ptr, SharedPtr<ChunkIndexEntry> &chunk_index) override {
        if (!need_save_) {
            UnrecoverableError("InsertData(): error: SecondaryIndexDataT is not allocated.");
        }
        auto map_ptr = static_cast<MultiMap<OrderedKeyType, u32> *>(ptr);
        if (!map_ptr) {
            UnrecoverableError("InsertData(): error: map_ptr type error.");
        }
        if (map_ptr->size() != chunk_row_count_) {
            UnrecoverableError(fmt::format("InsertData(): error: map size: {} != chunk_row_count_: {}", map_ptr->size(), chunk_row_count_));
        }
        u32 i = 0;
        for (const auto &[key, offset] : *map_ptr) {
            key_[i] = key;
            offset_[i] = offset;
            ++i;
        }
        if (i != chunk_row_count_) {
            UnrecoverableError(fmt::format("InsertData(): error: i: {} != chunk_row_count_: {}", i, chunk_row_count_));
        }
        const u32 part_num = chunk_index->GetPartNum();
        for (u32 part_id = 0; part_id < part_num; ++part_id) {
            const u32 part_row_count = chunk_index->GetPartRowCount(part_id);
            const u32 part_offset = part_id * 8192;
            BufferHandle handle = chunk_index->GetIndexPartAt(part_id);
            auto data_ptr = static_cast<char *>(handle.GetDataMut());
            for (u32 j = 0; j < part_row_count; ++j) {
                const u32 index = part_offset + j;
                const OrderedKeyType key = key_[index];
                const u32 offset = offset_[index];
                std::memcpy(data_ptr + j * PairSize, &key, sizeof(OrderedKeyType));
                std::memcpy(data_ptr + j * PairSize + sizeof(OrderedKeyType), &offset, sizeof(SegmentOffset));
            }
        }
        pgm_index_->BuildIndex(chunk_row_count_, key_.get());
    }
};

SecondaryIndexData *GetSecondaryIndexData(const SharedPtr<DataType> &data_type, const u32 chunk_row_count, const bool allocate) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
        return nullptr;
    }
    switch (data_type->type()) {
        case LogicalType::kTinyInt: {
            return new SecondaryIndexDataT<TinyIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kSmallInt: {
            return new SecondaryIndexDataT<SmallIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kInteger: {
            return new SecondaryIndexDataT<IntegerT>(chunk_row_count, allocate);
        }
        case LogicalType::kBigInt: {
            return new SecondaryIndexDataT<BigIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kFloat: {
            return new SecondaryIndexDataT<FloatT>(chunk_row_count, allocate);
        }
        case LogicalType::kDouble: {
            return new SecondaryIndexDataT<DoubleT>(chunk_row_count, allocate);
        }
        case LogicalType::kDate: {
            return new SecondaryIndexDataT<DateT>(chunk_row_count, allocate);
        }
        case LogicalType::kTime: {
            return new SecondaryIndexDataT<TimeT>(chunk_row_count, allocate);
        }
        case LogicalType::kDateTime: {
            return new SecondaryIndexDataT<DateTimeT>(chunk_row_count, allocate);
        }
        case LogicalType::kTimestamp: {
            return new SecondaryIndexDataT<TimestampT>(chunk_row_count, allocate);
        }
        default: {
            UnrecoverableError(fmt::format("Need to add secondary index support for data type: {}", data_type->ToString()));
            return nullptr;
        }
    }
}

u32 GetSecondaryIndexDataPairSize(const SharedPtr<DataType> &data_type) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
        return 0;
    }
    switch (data_type->type()) {
        case LogicalType::kTinyInt: {
            return SecondaryIndexDataT<TinyIntT>::PairSize;
        }
        case LogicalType::kSmallInt: {
            return SecondaryIndexDataT<SmallIntT>::PairSize;
        }
        case LogicalType::kInteger: {
            return SecondaryIndexDataT<IntegerT>::PairSize;
        }
        case LogicalType::kBigInt: {
            return SecondaryIndexDataT<BigIntT>::PairSize;
        }
        case LogicalType::kFloat: {
            return SecondaryIndexDataT<FloatT>::PairSize;
        }
        case LogicalType::kDouble: {
            return SecondaryIndexDataT<DoubleT>::PairSize;
        }
        case LogicalType::kDate: {
            return SecondaryIndexDataT<DateT>::PairSize;
        }
        case LogicalType::kTime: {
            return SecondaryIndexDataT<TimeT>::PairSize;
        }
        case LogicalType::kDateTime: {
            return SecondaryIndexDataT<DateTimeT>::PairSize;
        }
        case LogicalType::kTimestamp: {
            return SecondaryIndexDataT<TimestampT>::PairSize;
        }
        default: {
            UnrecoverableError(fmt::format("Need to add secondary index support for data type: {}", data_type->ToString()));
            return 0;
        }
    }
}

} // namespace infinity