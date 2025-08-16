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

module infinity_core:secondary_index_in_mem.impl;

import :secondary_index_in_mem;
import :default_values;
import :buffer_manager;
import :block_column_iter;
import :infinity_exception;
import :secondary_index_data;
import :buffer_handle;
import :logger;
import :base_memindex;
import :memindex_tracer;
import :column_vector;
import :buffer_obj;
import :rcu_multimap;

import std;

import logical_type;
import internal_types;
import column_def;

namespace infinity {

constexpr u32 map_memory_bloat_factor = 3;

// No wrapper needed anymore - RcuMultiMap now works with POD types directly

template <typename RawValueType>
class SecondaryIndexInMemT final : public SecondaryIndexInMem {
    using KeyType = ConvertToOrderedType<RawValueType>;
    const RowID begin_row_id_;
    // Replaced std::multimap + mutex with RcuMultiMap for better concurrent performance
    RcuMultiMap<KeyType, u32> in_mem_secondary_index_;

protected:
    u32 GetRowCountNoLock() const override { return in_mem_secondary_index_.size(); }
    u32 MemoryCostOfEachRow() const override { return map_memory_bloat_factor * (sizeof(KeyType) + sizeof(u32)); }
    u32 MemoryCostOfThis() const override { return sizeof(*this); }

public:
    explicit SecondaryIndexInMemT(const RowID begin_row_id) : begin_row_id_(begin_row_id) { IncreaseMemoryUsageBase(MemoryCostOfThis()); }
    ~SecondaryIndexInMemT() override { DecreaseMemoryUsageBase(MemoryCostOfThis() + GetRowCount() * MemoryCostOfEachRow()); }
    virtual RowID GetBeginRowID() const override { return begin_row_id_; }
    u32 GetRowCount() const override {
        // RcuMultiMap is thread-safe, no lock needed
        return in_mem_secondary_index_.size();
    }

    void InsertBlockData(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) override {
        MemIndexInserterIter1<RawValueType> iter(block_offset, col, offset, row_count);
        const auto inserted_rows = InsertInner(iter);
        assert(inserted_rows == row_count);
        IncreaseMemoryUsageBase(inserted_rows * MemoryCostOfEachRow());
    }

    void Dump(BufferObj *buffer_obj) const override {
        BufferHandle handle = buffer_obj->Load();
        auto data_ptr = static_cast<SecondaryIndexData *>(handle.GetDataMut());

        std::multimap<KeyType, u32> temp_map;
        const_cast<RcuMultiMap<KeyType, u32> &>(in_mem_secondary_index_).GetMergedMultiMap(temp_map);

        data_ptr->InsertData(&temp_map);
    }
    std::pair<u32, Bitmask> RangeQuery(const void *input) const override {
        const auto &[segment_row_count, b, e] = *static_cast<const std::tuple<u32, KeyType, KeyType> *>(input);
        return RangeQueryInner(segment_row_count, b, e);
    }

private:
    u32 InsertInner(auto &iter) {
        u32 inserted_count = 0;
        // No lock needed - RcuMultiMap handles concurrency internally
        while (true) {
            auto opt = iter.Next();
            if (!opt.has_value()) {
                break;
            }
            const auto &[v_ptr, offset] = opt.value();
            if constexpr (std::is_same_v<RawValueType, VarcharT>) {
                auto column_vector = iter.column_vector();
                std::span<const char> data = column_vector->GetVarcharInner(*v_ptr);
                const KeyType key = ConvertToOrderedKeyValue(std::string_view{data.data(), data.size()});
                // Insert u32 offset directly
                in_mem_secondary_index_.Insert(key, offset);
            } else {
                const KeyType key = ConvertToOrderedKeyValue(*v_ptr);
                // Insert u32 offset directly
                in_mem_secondary_index_.Insert(key, offset);
            }
            ++inserted_count;
        }
        return inserted_count;
    }

    std::pair<u32, Bitmask> RangeQueryInner(const u32 segment_row_count, const KeyType b, const KeyType e) const {
        std::vector<u32> result;
        const u32 result_size = in_mem_secondary_index_.range(b, e, result);
        std::pair<u32, Bitmask> result_var(result_size, Bitmask(segment_row_count));
        result_var.second.SetAllFalse();

        for (const u32 offset : result) {
            if (offset < segment_row_count) {
                result_var.second.SetTrue(offset);
            }
        }

        result_var.second.RunOptimize();
        return result_var;
    }
};

MemIndexTracerInfo SecondaryIndexInMem::GetInfo() const {
    const auto row_cnt = GetRowCount();
    const auto mem = MemoryCostOfThis() + row_cnt * MemoryCostOfEachRow();
    return MemIndexTracerInfo(nullptr, nullptr, nullptr, mem, row_cnt);
}

const ChunkIndexMetaInfo SecondaryIndexInMem::GetChunkIndexMetaInfo() const { return ChunkIndexMetaInfo{"", GetBeginRowID(), GetRowCount(), 0}; }

std::shared_ptr<SecondaryIndexInMem> SecondaryIndexInMem::NewSecondaryIndexInMem(const std::shared_ptr<ColumnDef> &column_def, RowID begin_row_id) {
    if (!column_def->type()->CanBuildSecondaryIndex()) {
        UnrecoverableError("Column type can't build secondary index");
    }
    switch (column_def->type()->type()) {
        case LogicalType::kTinyInt: {
            return std::make_shared<SecondaryIndexInMemT<TinyIntT>>(begin_row_id);
        }
        case LogicalType::kSmallInt: {
            return std::make_shared<SecondaryIndexInMemT<SmallIntT>>(begin_row_id);
        }
        case LogicalType::kInteger: {
            return std::make_shared<SecondaryIndexInMemT<IntegerT>>(begin_row_id);
        }
        case LogicalType::kBigInt: {
            return std::make_shared<SecondaryIndexInMemT<BigIntT>>(begin_row_id);
        }
        case LogicalType::kFloat: {
            return std::make_shared<SecondaryIndexInMemT<FloatT>>(begin_row_id);
        }
        case LogicalType::kDouble: {
            return std::make_shared<SecondaryIndexInMemT<DoubleT>>(begin_row_id);
        }
        case LogicalType::kDate: {
            return std::make_shared<SecondaryIndexInMemT<DateT>>(begin_row_id);
        }
        case LogicalType::kTime: {
            return std::make_shared<SecondaryIndexInMemT<TimeT>>(begin_row_id);
        }
        case LogicalType::kDateTime: {
            return std::make_shared<SecondaryIndexInMemT<DateTimeT>>(begin_row_id);
        }
        case LogicalType::kTimestamp: {
            return std::make_shared<SecondaryIndexInMemT<TimestampT>>(begin_row_id);
        }
        case LogicalType::kVarchar: {
            return std::make_shared<SecondaryIndexInMemT<VarcharT>>(begin_row_id);
        }
        default: {
            return nullptr;
        }
    }
}

} // namespace infinity