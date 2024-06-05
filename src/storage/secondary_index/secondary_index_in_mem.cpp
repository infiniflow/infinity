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

#include <bit>
#include <vector>

module secondary_index_in_mem;

import stl;
import logical_type;
import internal_types;
import column_def;
import bitmask;
import default_values;
import buffer_manager;
import block_column_entry;
import block_column_iter;
import infinity_exception;
import secondary_index_data;
import chunk_index_entry;
import segment_index_entry;
import buffer_handle;
import logger;

namespace infinity {

template <typename RawValueType>
class SecondaryIndexInMemT final : public SecondaryIndexInMem {
    using KeyType = ConvertToOrderedType<RawValueType>;
    const RowID begin_row_id_;
    const u32 max_size_;
    std::shared_mutex map_mutex_;
    MultiMap<KeyType, u32> in_mem_secondary_index_;

public:
    explicit SecondaryIndexInMemT(const RowID begin_row_id, const u32 max_size) : begin_row_id_(begin_row_id), max_size_(max_size) {}
    u32 GetRowCount() const override { return in_mem_secondary_index_.size(); }
    void Insert(const u16 block_id, BlockColumnEntry *block_column_entry, BufferManager *buffer_manager, u32 row_offset, u32 row_count) override {
        MemIndexInserterIter<RawValueType> iter(block_id * DEFAULT_BLOCK_CAPACITY, block_column_entry, buffer_manager, row_offset, row_count);
        InsertInner(iter);
    }
    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) override {
        std::shared_lock lock(map_mutex_);
        u32 row_count = GetRowCount();
        auto new_chunk_index_entry = segment_index_entry->CreateSecondaryIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr);
        BufferHandle handle = new_chunk_index_entry->GetIndex();
        auto data_ptr = static_cast<SecondaryIndexData *>(handle.GetDataMut());
        data_ptr->InsertData(&in_mem_secondary_index_, new_chunk_index_entry);
        return new_chunk_index_entry;
    }
    Pair<u32, std::variant<Vector<u32>, Bitmask>> RangeQuery(const void *input) override {
        const auto &[segment_row_count, b, e] = *static_cast<const std::tuple<u32, KeyType, KeyType> *>(input);
        return RangeQueryInner(segment_row_count, b, e);
    }

private:
    void InsertInner(auto &iter) {
        std::unique_lock lock(map_mutex_);
        while (true) {
            auto opt = iter.Next();
            if (!opt.has_value()) {
                break;
            }
            const auto &[v_ptr, offset] = opt.value();
            const KeyType key = ConvertToOrderedKeyValue(*v_ptr);
            in_mem_secondary_index_.emplace(key, offset);
        }
    }

    Pair<u32, std::variant<Vector<u32>, Bitmask>> RangeQueryInner(const u32 segment_row_count, const KeyType b, const KeyType e) {
        std::shared_lock lock(map_mutex_);
        const auto begin = in_mem_secondary_index_.lower_bound(b);
        const auto end = in_mem_secondary_index_.upper_bound(e);
        const u32 result_size = std::distance(begin, end);
        Pair<u32, std::variant<Vector<u32>, Bitmask>> result_var;
        result_var.first = result_size;
        // use array or bitmask for result
        // use array when result_size <= 1024 or size of array (u32 type) <= size of bitmask
        if (result_size <= 1024 or result_size <= std::bit_ceil(segment_row_count) / 32) {
            auto &result = result_var.second.emplace<Vector<u32>>();
            result.reserve(result_size);
            for (auto it = begin; it != end; ++it) {
                result.push_back(it->second);
            }
        } else {
            auto &result = result_var.second.emplace<Bitmask>();
            result.Initialize(segment_row_count);
            result.SetAllFalse();
            for (auto it = begin; it != end; ++it) {
                result.SetTrue(it->second);
            }
        }
        return result_var;
    }
};

SharedPtr<SecondaryIndexInMem> SecondaryIndexInMem::NewSecondaryIndexInMem(const SharedPtr<ColumnDef> &column_def, RowID begin_row_id, u32 max_size) {
    if (!column_def->type()->CanBuildSecondaryIndex()) {
        String error_message = "Column type can't build secondary index";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    switch (column_def->type()->type()) {
        case LogicalType::kTinyInt: {
            return MakeShared<SecondaryIndexInMemT<TinyIntT>>(begin_row_id, max_size);
        }
        case LogicalType::kSmallInt: {
            return MakeShared<SecondaryIndexInMemT<SmallIntT>>(begin_row_id, max_size);
        }
        case LogicalType::kInteger: {
            return MakeShared<SecondaryIndexInMemT<IntegerT>>(begin_row_id, max_size);
        }
        case LogicalType::kBigInt: {
            return MakeShared<SecondaryIndexInMemT<BigIntT>>(begin_row_id, max_size);
        }
        case LogicalType::kFloat: {
            return MakeShared<SecondaryIndexInMemT<FloatT>>(begin_row_id, max_size);
        }
        case LogicalType::kDouble: {
            return MakeShared<SecondaryIndexInMemT<DoubleT>>(begin_row_id, max_size);
        }
        case LogicalType::kDate: {
            return MakeShared<SecondaryIndexInMemT<DateT>>(begin_row_id, max_size);
        }
        case LogicalType::kTime: {
            return MakeShared<SecondaryIndexInMemT<TimeT>>(begin_row_id, max_size);
        }
        case LogicalType::kDateTime: {
            return MakeShared<SecondaryIndexInMemT<DateTimeT>>(begin_row_id, max_size);
        }
        case LogicalType::kTimestamp: {
            return MakeShared<SecondaryIndexInMemT<TimestampT>>(begin_row_id, max_size);
        }
        default: {
            return nullptr;
        }
    }
}

} // namespace infinity