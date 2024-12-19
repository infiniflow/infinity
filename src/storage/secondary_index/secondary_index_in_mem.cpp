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
#include <cassert>
#include <vector>

module secondary_index_in_mem;

import stl;
import logical_type;
import internal_types;
import column_def;
import default_values;
import buffer_manager;
import block_column_entry;
import block_column_iter;
import infinity_exception;
import secondary_index_data;
import chunk_index_entry;
import segment_index_entry;
import table_index_entry;
import buffer_handle;
import logger;
import base_memindex;
import memindex_tracer;

namespace infinity {

constexpr u32 map_memory_bloat_factor = 3;

template <typename RawValueType>
class SecondaryIndexInMemT final : public SecondaryIndexInMem {
    using KeyType = ConvertToOrderedType<RawValueType>;
    const RowID begin_row_id_;
    mutable std::shared_mutex map_mutex_;
    MultiMap<KeyType, u32> in_mem_secondary_index_;

protected:
    u32 GetRowCountNoLock() const override { return in_mem_secondary_index_.size(); }
    u32 MemoryCostOfEachRow() const override { return map_memory_bloat_factor * (sizeof(KeyType) + sizeof(u32)); }
    u32 MemoryCostOfThis() const override { return sizeof(*this); }

public:
    SecondaryIndexInMemT(SegmentIndexEntry *segment_index_entry, const RowID begin_row_id)
        : SecondaryIndexInMem(segment_index_entry), begin_row_id_(begin_row_id) {
        IncreaseMemoryUsageBase(MemoryCostOfThis());
    }
    ~SecondaryIndexInMemT() override {
        DecreaseMemoryUsageBase(MemoryCostOfThis() + GetRowCount() * MemoryCostOfEachRow());
    }
    u32 GetRowCount() const override {
        std::shared_lock lock(map_mutex_);
        return in_mem_secondary_index_.size();
    }
    void InsertBlockData(const SegmentOffset block_offset,
                         BlockColumnEntry *block_column_entry,
                         BufferManager *buffer_manager,
                         const u32 row_offset,
                         const u32 row_count) override {
        MemIndexInserterIter<RawValueType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
        const auto inserted_rows = InsertInner(iter);
        assert(inserted_rows == row_count);
        IncreaseMemoryUsageBase(inserted_rows * MemoryCostOfEachRow());
    }
    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) const override {
        assert(segment_index_entry == segment_index_entry_);
        std::shared_lock lock(map_mutex_);
        const u32 row_count = GetRowCountNoLock();
        auto new_chunk_index_entry = segment_index_entry->CreateSecondaryIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr);
        BufferHandle handle = new_chunk_index_entry->GetIndex();
        auto data_ptr = static_cast<SecondaryIndexData *>(handle.GetDataMut());
        data_ptr->InsertData(&in_mem_secondary_index_);
        return new_chunk_index_entry;
    }
    Pair<u32, Bitmask> RangeQuery(const void *input) const override {
        const auto &[segment_row_count, b, e] = *static_cast<const std::tuple<u32, KeyType, KeyType> *>(input);
        return RangeQueryInner(segment_row_count, b, e);
    }

private:
    u32 InsertInner(auto &iter) {
        u32 inserted_count = 0;
        std::unique_lock lock(map_mutex_);
        while (true) {
            auto opt = iter.Next();
            if (!opt.has_value()) {
                break;
            }
            const auto &[v_ptr, offset] = opt.value();
            if constexpr (std::is_same_v<RawValueType, VarcharT>) {
                auto column_vector = iter.column_vector();
                Span<const char> data = column_vector->GetVarcharInner(*v_ptr);
                const KeyType key = ConvertToOrderedKeyValue(std::string_view{data.data(), data.size()});
                in_mem_secondary_index_.emplace(key, offset);
            } else {
                const KeyType key = ConvertToOrderedKeyValue(*v_ptr);
                in_mem_secondary_index_.emplace(key, offset);
            }
            ++inserted_count;
        }
        return inserted_count;
    }

    Pair<u32, Bitmask> RangeQueryInner(const u32 segment_row_count, const KeyType b, const KeyType e) const {
        std::shared_lock lock(map_mutex_);
        const auto begin = in_mem_secondary_index_.lower_bound(b);
        const auto end = in_mem_secondary_index_.upper_bound(e);
        const u32 result_size = std::distance(begin, end);
        Pair<u32, Bitmask> result_var(result_size, Bitmask(segment_row_count));
        result_var.second.SetAllFalse();
        for (auto it = begin; it != end; ++it) {
            if (const auto offset = it->second; offset < segment_row_count) {
                result_var.second.SetTrue(offset);
            }
        }
        result_var.second.RunOptimize();
        return result_var;
    }
};

MemIndexTracerInfo SecondaryIndexInMem::GetInfo() const {
    auto *table_index_entry = segment_index_entry_->table_index_entry();
    SharedPtr<String> index_name = table_index_entry->GetIndexName();
    auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
    SharedPtr<String> table_name = table_entry->GetTableName();
    SharedPtr<String> db_name = table_entry->GetDBName();
    const auto row_cnt = GetRowCount();
    const auto mem = MemoryCostOfThis() + row_cnt * MemoryCostOfEachRow();
    return MemIndexTracerInfo(std::move(index_name), std::move(table_name), std::move(db_name), mem, row_cnt);
}

TableIndexEntry *SecondaryIndexInMem::table_index_entry() const { return segment_index_entry_->table_index_entry(); }

SharedPtr<SecondaryIndexInMem> SecondaryIndexInMem::NewSecondaryIndexInMem(const SharedPtr<ColumnDef> &column_def,
                                                                           SegmentIndexEntry *segment_index_entry,
                                                                           RowID begin_row_id) {
    if (!column_def->type()->CanBuildSecondaryIndex()) {
        UnrecoverableError("Column type can't build secondary index");
    }
    switch (column_def->type()->type()) {
        case LogicalType::kTinyInt: {
            return MakeShared<SecondaryIndexInMemT<TinyIntT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kSmallInt: {
            return MakeShared<SecondaryIndexInMemT<SmallIntT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kInteger: {
            return MakeShared<SecondaryIndexInMemT<IntegerT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kBigInt: {
            return MakeShared<SecondaryIndexInMemT<BigIntT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kFloat: {
            return MakeShared<SecondaryIndexInMemT<FloatT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kDouble: {
            return MakeShared<SecondaryIndexInMemT<DoubleT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kDate: {
            return MakeShared<SecondaryIndexInMemT<DateT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kTime: {
            return MakeShared<SecondaryIndexInMemT<TimeT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kDateTime: {
            return MakeShared<SecondaryIndexInMemT<DateTimeT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kTimestamp: {
            return MakeShared<SecondaryIndexInMemT<TimestampT> >(segment_index_entry, begin_row_id);
        }
        case LogicalType::kVarchar: {
            return MakeShared<SecondaryIndexInMemT<VarcharT> >(segment_index_entry, begin_row_id);
        }
        default: {
            return nullptr;
        }
    }
}

} // namespace infinity