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

module infinity_core;

import :stl;

import :default_values;
import :index_base;
import :local_file_handle;
import :infinity_exception;
import :third_party;
import :secondary_index_pgm;
import :logger;
import :buffer_handle;
import :buffer_obj;
import :table_index_meeta;
import logical_type;
import internal_types;

namespace infinity {

template <typename RawValueType>
struct SecondaryIndexChunkDataReader {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    BufferHandle handle_;
    u32 row_count_ = 0;
    u32 next_offset_ = 0;
    const void *key_ptr_ = nullptr;
    const SegmentOffset *offset_ptr_ = nullptr;
    SecondaryIndexChunkDataReader(BufferObj *buffer_obj, u32 row_count) {
        handle_ = buffer_obj->Load();
        row_count_ = row_count;
        auto *index = static_cast<const SecondaryIndexDataBase<HighCardinalityTag> *>(handle_.GetData());
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
    explicit SecondaryIndexChunkMerger(const Vector<Pair<u32, BufferObj *>> &buffer_objs) {
        readers_.reserve(buffer_objs.size());
        for (const auto &[row_count, buffer_obj] : buffer_objs) {
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

// High cardinality implementation (current implementation)
template <typename RawValueType>
class SecondaryIndexDataT final : public SecondaryIndexDataBase<HighCardinalityTag> {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    UniquePtr<OrderedKeyType[]> key_;
    UniquePtr<SegmentOffset[]> offset_;

public:
    SecondaryIndexDataT(const u32 chunk_row_count, const bool allocate) : SecondaryIndexDataBase<HighCardinalityTag>(chunk_row_count) {
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

    void InsertMergeData(const Vector<Pair<u32, BufferObj *>> &old_chunks) override {
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

// Low cardinality implementation (uses unique keys with RoaringBitmap for offsets)
template <typename RawValueType>
class SecondaryIndexDataLowCardinalityT final : public SecondaryIndexDataBase<LowCardinalityTag> {
    using OrderedKeyType = ConvertToOrderedType<RawValueType>;
    Vector<OrderedKeyType> unique_keys_;
    Vector<Bitmap> offset_bitmaps_;
    u32 unique_key_count_ = 0;

public:
    SecondaryIndexDataLowCardinalityT(const u32 chunk_row_count, const bool allocate) : SecondaryIndexDataBase<LowCardinalityTag>(chunk_row_count) {
        // No PGM index needed for low cardinality
        // pgm_index_ remains nullptr
        // key_ptr_ and offset_ptr_ will be set up after data insertion
    }

    void SaveIndexInner(LocalFileHandle &file_handle) const override {
        // Save unique key count
        file_handle.Append(&unique_key_count_, sizeof(unique_key_count_));

        // Save unique keys
        if (unique_key_count_ > 0) {
            file_handle.Append(unique_keys_.data(), unique_key_count_ * sizeof(OrderedKeyType));

            // Save RoaringBitmaps
            for (const auto &bitmap : offset_bitmaps_) {
                // Use const_cast to call non-const GetSizeInBytes (needed for optimization)
                i32 bitmap_size = const_cast<Bitmap &>(bitmap).GetSizeInBytes();
                file_handle.Append(&bitmap_size, sizeof(bitmap_size));

                Vector<char> bitmap_data(bitmap_size);
                char *ptr = bitmap_data.data();
                bitmap.WriteAdv(ptr);
                file_handle.Append(bitmap_data.data(), bitmap_size);
            }
        }
    }

    void ReadIndexInner(LocalFileHandle &file_handle) override {
        // Read unique key count
        file_handle.Read(&unique_key_count_, sizeof(unique_key_count_));

        if (unique_key_count_ > 0) {
            // Read unique keys
            unique_keys_.resize(unique_key_count_);
            file_handle.Read(unique_keys_.data(), unique_key_count_ * sizeof(OrderedKeyType));

            // Read RoaringBitmaps
            offset_bitmaps_.clear();
            offset_bitmaps_.reserve(unique_key_count_);
            for (u32 i = 0; i < unique_key_count_; ++i) {
                i32 bitmap_size;
                file_handle.Read(&bitmap_size, sizeof(bitmap_size));

                Vector<char> bitmap_data(bitmap_size);
                file_handle.Read(bitmap_data.data(), bitmap_size);

                // Use static ReadAdv method to deserialize
                const char *ptr = bitmap_data.data();
                auto bitmap_ptr = Bitmap::ReadAdv(ptr, bitmap_size);
                offset_bitmaps_.emplace_back(*bitmap_ptr);
            }

            // Set up key_ptr_ and offset_ptr_ for compatibility
            SetupCompatibilityPointers();
        }
    }

    void InsertData(const void *ptr) override {
        auto map_ptr = static_cast<const MultiMap<OrderedKeyType, u32> *>(ptr);
        if (!map_ptr) {
            UnrecoverableError("InsertData(): error: map_ptr type error.");
        }
        if (map_ptr->size() != chunk_row_count_) {
            UnrecoverableError(fmt::format("InsertData(): error: map size: {} != chunk_row_count_: {}", map_ptr->size(), chunk_row_count_));
        }

        // Build unique keys and corresponding bitmaps
        Map<OrderedKeyType, Vector<u32>> key_to_offsets;
        for (const auto &[key, offset] : *map_ptr) {
            key_to_offsets[key].push_back(offset);
        }

        // Convert to vectors
        unique_key_count_ = key_to_offsets.size();
        unique_keys_.reserve(unique_key_count_);
        offset_bitmaps_.reserve(unique_key_count_);

        for (const auto &[key, offsets] : key_to_offsets) {
            unique_keys_.push_back(key);

            // Create Bitmap and add all offsets
            Bitmap bitmap(chunk_row_count_);
            for (u32 offset : offsets) {
                bitmap.SetTrue(offset);
            }
            offset_bitmaps_.emplace_back(std::move(bitmap));
        }

        // Set up compatibility pointers
        SetupCompatibilityPointers();
    }

    void InsertMergeData(const Vector<Pair<u32, BufferObj *>> &old_chunks) override {
        SecondaryIndexChunkMerger<RawValueType> merger(old_chunks);

        // Build unique keys and corresponding bitmaps from merged data
        Map<OrderedKeyType, Vector<u32>> key_to_offsets;
        OrderedKeyType key = {};
        u32 offset = 0;
        u32 total_count = 0;

        while (merger.GetNextDataPair(key, offset)) {
            key_to_offsets[key].push_back(offset);
            ++total_count;
        }

        if (total_count != chunk_row_count_) {
            UnrecoverableError(fmt::format("InsertMergeData(): error: total_count: {} != chunk_row_count_: {}", total_count, chunk_row_count_));
        }

        // Convert to vectors
        unique_key_count_ = key_to_offsets.size();
        unique_keys_.reserve(unique_key_count_);
        offset_bitmaps_.reserve(unique_key_count_);

        for (const auto &[key_val, offsets] : key_to_offsets) {
            unique_keys_.push_back(key_val);

            // Create Bitmap and add all offsets
            Bitmap bitmap(chunk_row_count_);
            for (u32 offset_val : offsets) {
                bitmap.SetTrue(offset_val);
            }
            offset_bitmaps_.emplace_back(std::move(bitmap));
        }

        // Set up compatibility pointers
        SetupCompatibilityPointers();
    }

private:
    void SetupCompatibilityPointers() {
        // For compatibility with existing code that expects key_ptr_ and offset_ptr_
        // We'll set key_ptr_ to point to unique_keys_ data
        if (!unique_keys_.empty()) {
            key_ptr_ = unique_keys_.data();
        }
        // Note: offset_ptr_ cannot be directly set since we use RoaringBitmaps
        // Code that needs offsets should use the new GetOffsetsForKey method
    }

public:
    // New method to get offsets for a specific key (for low cardinality indexes)
    const Bitmap *GetOffsetsForKey(const OrderedKeyType &key) const {
        auto it = std::lower_bound(unique_keys_.begin(), unique_keys_.end(), key);
        if (it != unique_keys_.end() && *it == key) {
            size_t index = it - unique_keys_.begin();
            return &offset_bitmaps_[index];
        }
        return nullptr;
    }

    u32 GetUniqueKeyCount() const override { return unique_key_count_; }

    const Vector<OrderedKeyType> &GetUniqueKeys() const { return unique_keys_; }

    // Virtual method implementations for base class interface
    const void *GetUniqueKeysPtr() const override { return static_cast<const void *>(unique_keys_.data()); }

    const void *GetOffsetsForKeyPtr(const void *key_ptr) const override {
        const OrderedKeyType *typed_key = static_cast<const OrderedKeyType *>(key_ptr);
        return static_cast<const void *>(GetOffsetsForKey(*typed_key));
    }
};

SecondaryIndexDataBase<HighCardinalityTag> *
GetSecondaryIndexData(const SharedPtr<DataType> &data_type, const u32 chunk_row_count, const bool allocate) {
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

// Template specialization for HighCardinalityTag
template <>
SecondaryIndexDataBase<HighCardinalityTag> *
GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(const SharedPtr<DataType> &data_type, const u32 chunk_row_count, const bool allocate) {
    return GetSecondaryIndexData(data_type, chunk_row_count, allocate);
}

// Template specialization for LowCardinalityTag
template <>
SecondaryIndexDataBase<LowCardinalityTag> *
GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(const SharedPtr<DataType> &data_type, const u32 chunk_row_count, const bool allocate) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
        return nullptr;
    }
    switch (data_type->type()) {
        case LogicalType::kTinyInt: {
            return new SecondaryIndexDataLowCardinalityT<TinyIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kSmallInt: {
            return new SecondaryIndexDataLowCardinalityT<SmallIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kInteger: {
            return new SecondaryIndexDataLowCardinalityT<IntegerT>(chunk_row_count, allocate);
        }
        case LogicalType::kBigInt: {
            return new SecondaryIndexDataLowCardinalityT<BigIntT>(chunk_row_count, allocate);
        }
        case LogicalType::kFloat: {
            return new SecondaryIndexDataLowCardinalityT<FloatT>(chunk_row_count, allocate);
        }
        case LogicalType::kDouble: {
            return new SecondaryIndexDataLowCardinalityT<DoubleT>(chunk_row_count, allocate);
        }
        case LogicalType::kDate: {
            return new SecondaryIndexDataLowCardinalityT<DateT>(chunk_row_count, allocate);
        }
        case LogicalType::kTime: {
            return new SecondaryIndexDataLowCardinalityT<TimeT>(chunk_row_count, allocate);
        }
        case LogicalType::kDateTime: {
            return new SecondaryIndexDataLowCardinalityT<DateTimeT>(chunk_row_count, allocate);
        }
        case LogicalType::kTimestamp: {
            return new SecondaryIndexDataLowCardinalityT<TimestampT>(chunk_row_count, allocate);
        }
        case LogicalType::kVarchar: {
            return new SecondaryIndexDataLowCardinalityT<VarcharT>(chunk_row_count, allocate);
        }
        default: {
            UnrecoverableError(fmt::format("Need to add secondary index support for data type: {}", data_type->ToString()));
            return nullptr;
        }
    }
}

void *GetSecondaryIndexDataWithMeeta(const SharedPtr<DataType> &data_type,
                                     const u32 chunk_row_count,
                                     const bool allocate,
                                     TableIndexMeeta *table_index_meeta) {
    if (!table_index_meeta) {
        // Default to HighCardinality if no meeta provided
        return static_cast<void *>(GetSecondaryIndexData(data_type, chunk_row_count, allocate));
    }

    auto [cardinality, status] = table_index_meeta->GetSecondaryIndexCardinality();
    if (!status.ok()) {
        // Default to HighCardinality if unable to determine
        cardinality = SecondaryIndexCardinality::kHighCardinality;
    }

    if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
        return static_cast<void *>(GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, allocate));
    } else {
        return static_cast<void *>(GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, allocate));
    }
}

} // namespace infinity
