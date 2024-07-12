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
struct SecondaryIndexChunkDataReader {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    static constexpr u32 PairSize = sizeof(OrderedKeyType) + sizeof(SegmentOffset);
    ChunkIndexEntry *chunk_index_;
    BufferHandle current_handle_;
    u32 part_count_ = 0;
    u32 current_offset_ = 0;
    u32 current_part_id_ = 0;
    u32 current_part_size_ = 0;
    SecondaryIndexChunkDataReader(ChunkIndexEntry *chunk_index) : chunk_index_(chunk_index) {
        part_count_ = chunk_index_->GetPartNum();
        current_part_size_ = chunk_index_->GetPartRowCount(current_part_id_);
    }
    bool GetNextDataPair(OrderedKeyType &key, u32 &offset) {
        if (current_offset_ == 0) {
            if (current_part_id_ >= part_count_) {
                return false;
            }
            current_handle_ = chunk_index_->GetIndexPartAt(current_part_id_);
        }
        const auto *data_ptr = static_cast<const char *>(current_handle_.GetData());
        std::memcpy(&key, data_ptr + current_offset_ * PairSize, sizeof(OrderedKeyType));
        std::memcpy(&offset, data_ptr + current_offset_ * PairSize + sizeof(OrderedKeyType), sizeof(SegmentOffset));
        if (++current_offset_ == current_part_size_) {
            current_offset_ = 0;
            if (++current_part_id_ < part_count_) {
                current_part_size_ = chunk_index_->GetPartRowCount(current_part_id_);
            }
        }
        return true;
    }
};

template <typename RawValueType>
struct SecondaryIndexChunkMerger {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    Vector<SecondaryIndexChunkDataReader<RawValueType>> readers_;
    std::priority_queue<Tuple<OrderedKeyType, u32, u32>, Vector<Tuple<OrderedKeyType, u32, u32>>, std::greater<Tuple<OrderedKeyType, u32, u32>>> pq_;
    explicit SecondaryIndexChunkMerger(const Vector<ChunkIndexEntry *> &old_chunks) {
        readers_.reserve(old_chunks.size());
        for (ChunkIndexEntry *chunk : old_chunks) {
            readers_.emplace_back(chunk);
        }
        OrderedKeyType key = {};
        u32 offset = 0;
        for (u32 i = 0; i < readers_.size(); ++i) {
            if (readers_[i].GetNextDataPair(key, offset)) {
                pq_.emplace(key, offset, i);
            }
        }
    }
    bool GetNextDataPair(OrderedKeyType &out_key, u32 &out_offset) {
        if (pq_.empty()) {
            return false;
        }
        const auto [key, offset, reader_id] = pq_.top();
        out_key = key;
        out_offset = offset;
        pq_.pop();
        OrderedKeyType next_key = {};
        u32 next_offset = 0;
        if (readers_[reader_id].GetNextDataPair(next_key, next_offset)) {
            pq_.emplace(next_key, next_offset, reader_id);
        }
        return true;
    }
};

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
            String error_message = "SaveIndexInner(): error: SecondaryIndexDataT is not allocated.";
            UnrecoverableError(error_message);
        }
        pgm_index_->SaveIndex(file_handler);
    }

    void ReadIndexInner(FileHandler &file_handler) override { pgm_index_->LoadIndex(file_handler); }

    void InsertData(void *ptr, SharedPtr<ChunkIndexEntry> &chunk_index) override {
        if (!need_save_) {
            String error_message = "InsertData(): error: SecondaryIndexDataT is not allocated.";
            UnrecoverableError(error_message);
        }
        auto map_ptr = static_cast<MultiMap<OrderedKeyType, u32> *>(ptr);
        if (!map_ptr) {
            String error_message = "InsertData(): error: map_ptr type error.";
            UnrecoverableError(error_message);
        }
        if (map_ptr->size() != chunk_row_count_) {
            String error_message = fmt::format("InsertData(): error: map size: {} != chunk_row_count_: {}", map_ptr->size(), chunk_row_count_);
            UnrecoverableError(error_message);
        }
        u32 i = 0;
        for (const auto &[key, offset] : *map_ptr) {
            key_[i] = key;
            offset_[i] = offset;
            ++i;
        }
        if (i != chunk_row_count_) {
            String error_message = fmt::format("InsertData(): error: i: {} != chunk_row_count_: {}", i, chunk_row_count_);
            UnrecoverableError(error_message);
        }
        OutputAndBuild(chunk_index);
    }

    void InsertMergeData(Vector<ChunkIndexEntry *> &old_chunks, SharedPtr<ChunkIndexEntry> &merged_chunk_index_entry) override {
        if (!need_save_) {
            String error_message = "InsertMergeData(): error: SecondaryIndexDataT is not allocated.";
            UnrecoverableError(error_message);
        }
        SecondaryIndexChunkMerger<RawValueType> merger(old_chunks);
        OrderedKeyType key = {};
        u32 offset = 0;
        u32 i = 0;
        while (merger.GetNextDataPair(key, offset)) {
            key_[i] = key;
            offset_[i] = offset;
            ++i;
        }
        if (i != chunk_row_count_) {
            String error_message = fmt::format("InsertMergeData(): error: i: {} != chunk_row_count_: {}", i, chunk_row_count_);
            UnrecoverableError(error_message);
        }
        OutputAndBuild(merged_chunk_index_entry);
    }

    void OutputAndBuild(SharedPtr<ChunkIndexEntry> &chunk_index) {
        const u32 part_num = chunk_index->GetPartNum();
        for (u32 part_id = 0; part_id < part_num; ++part_id) {
            const u32 part_row_count = chunk_index->GetPartRowCount(part_id);
            const u32 part_offset = part_id * 8192;
            BufferHandle handle = chunk_index->GetIndexPartAt(part_id);
            auto data_ptr = static_cast<char *>(handle.GetDataMut());
            for (u32 j = 0; j < part_row_count; ++j) {
                const u32 index = part_offset + j;
                std::memcpy(data_ptr + j * PairSize, key_.get() + index, sizeof(OrderedKeyType));
                std::memcpy(data_ptr + j * PairSize + sizeof(OrderedKeyType), offset_.get() + index, sizeof(SegmentOffset));
            }
        }
        pgm_index_->BuildIndex(chunk_row_count_, key_.get());
    }
};

SecondaryIndexData *GetSecondaryIndexData(const SharedPtr<DataType> &data_type, const u32 chunk_row_count, const bool allocate) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        String error_message = fmt::format("Cannot build secondary index on data type: {}", data_type->ToString());
        UnrecoverableError(error_message);
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
        case LogicalType::kVarchar: {
            return new SecondaryIndexDataT<VarcharT>(chunk_row_count, allocate);
        }
        default: {
            String error_message = fmt::format("Need to add secondary index support for data type: {}", data_type->ToString());
            UnrecoverableError(error_message);
            return nullptr;
        }
    }
}

u32 GetSecondaryIndexDataPairSize(const SharedPtr<DataType> &data_type) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        String error_message = fmt::format("Cannot build secondary index on data type: {}", data_type->ToString());
        UnrecoverableError(error_message);
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
        case LogicalType::kVarchar: {
            return SecondaryIndexDataT<VarcharT>::PairSize;
        }
        default: {
            String error_message = fmt::format("Need to add secondary index support for data type: {}", data_type->ToString());
            UnrecoverableError(error_message);
            return 0;
        }
    }
}

} // namespace infinity