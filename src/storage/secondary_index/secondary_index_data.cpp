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

#include <cassert>
#include <concepts>
#include <vector>

module secondary_index_data;

import stl;

import default_values;
import index_base;
import local_file_handle;
import infinity_exception;
import third_party;
import secondary_index_pgm;
import logger;
import chunk_index_entry;
import buffer_handle;
import buffer_obj;

namespace infinity {

template <typename RawValueType>
struct SecondaryIndexChunkDataReader {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    // ChunkIndexEntry *chunk_index_;
    BufferHandle handle_;
    u32 row_count_ = 0;
    u32 next_offset_ = 0;
    const void *key_ptr_ = nullptr;
    const SegmentOffset *offset_ptr_ = nullptr;
    SecondaryIndexChunkDataReader(ChunkIndexEntry *chunk_index) {
        handle_ = chunk_index->GetIndex();
        row_count_ = chunk_index->GetRowCount();
        auto *index = static_cast<const SecondaryIndexData *>(handle_.GetData());
        std::tie(key_ptr_, offset_ptr_) = index->GetKeyOffsetPointer();
        assert(index->GetChunkRowCount() == row_count_);
    }
    SecondaryIndexChunkDataReader(BufferObj *buffer_obj, u32 row_count) {
        handle_ = buffer_obj->Load();
        row_count_ = row_count;
        auto *index = static_cast<const SecondaryIndexData *>(handle_.GetData());
        std::tie(key_ptr_, offset_ptr_) = index->GetKeyOffsetPointer();
        assert(index->GetChunkRowCount() == row_count_);
    }
    bool GetNextDataPair(OrderedKeyType &key, u32 &offset) {
        if (next_offset_ >= row_count_) {
            return false;
        }
        std::memcpy(&key, static_cast<const char *>(key_ptr_) + next_offset_ * sizeof(OrderedKeyType), sizeof(key));
        std::memcpy(&offset, offset_ptr_ + next_offset_, sizeof(offset));
        ++next_offset_;
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
    explicit SecondaryIndexChunkMerger(const Vector<Pair<BufferObj *, u32>> &buffer_objs) {
        readers_.reserve(buffer_objs.size());
        for (const auto &[buffer_obj, row_count] : buffer_objs) {
            readers_.emplace_back(buffer_obj, row_count);
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
    UniquePtr<OrderedKeyType[]> key_;
    UniquePtr<SegmentOffset[]> offset_;

public:
    SecondaryIndexDataT(const u32 chunk_row_count, const bool allocate) : SecondaryIndexData(chunk_row_count) {
        pgm_index_ = GenerateSecondaryPGMIndex<OrderedKeyType>();
        key_ = MakeUnique<OrderedKeyType[]>(chunk_row_count_);
        offset_ = MakeUnique<SegmentOffset[]>(chunk_row_count_);
        key_ptr_ = key_.get();
        offset_ptr_ = offset_.get();
    }

    void SaveIndexInner(LocalFileHandle &file_handle) const override {
        file_handle.Append(key_ptr_, chunk_row_count_ * sizeof(OrderedKeyType));
        file_handle.Append(offset_ptr_, chunk_row_count_ * sizeof(SegmentOffset));
        pgm_index_->SaveIndex(file_handle);
    }

    void ReadIndexInner(LocalFileHandle &file_handle) override {
        file_handle.Read(key_ptr_, chunk_row_count_ * sizeof(OrderedKeyType));
        file_handle.Read(offset_ptr_, chunk_row_count_ * sizeof(SegmentOffset));
        pgm_index_->LoadIndex(file_handle);
    }

    void InsertData(const void *ptr) override {
        auto map_ptr = static_cast<const MultiMap<OrderedKeyType, u32> *>(ptr);
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
        pgm_index_->BuildIndex(chunk_row_count_, key_.get());
    }

    void InsertMergeData(Vector<ChunkIndexEntry *> &old_chunks) override {
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
            UnrecoverableError(fmt::format("InsertMergeData(): error: i: {} != chunk_row_count_: {}", i, chunk_row_count_));
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
        case LogicalType::kVarchar: {
            return new SecondaryIndexDataT<VarcharT>(chunk_row_count, allocate);
        }
        default: {
            UnrecoverableError(fmt::format("Need to add secondary index support for data type: {}", data_type->ToString()));
            return nullptr;
        }
    }
}

} // namespace infinity
