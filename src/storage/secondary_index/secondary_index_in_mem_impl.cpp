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

template <typename RawValueType, typename CardinalityTag>
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
    explicit SecondaryIndexInMemT(const RowID begin_row_id, SecondaryIndexCardinality cardinality)
        : SecondaryIndexInMem(cardinality), begin_row_id_(begin_row_id) {
        IncreaseMemoryUsageBase(MemoryCostOfThis());
    }
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

        // Use template specialization based on CardinalityTag
        if constexpr (std::is_same_v<CardinalityTag, HighCardinalityTag>) {
            auto data_ptr = static_cast<SecondaryIndexDataBase<HighCardinalityTag> *>(handle.GetDataMut());
            std::multimap<KeyType, u32> temp_map;
            const_cast<RcuMultiMap<KeyType, u32> &>(in_mem_secondary_index_).GetMergedMultiMap(temp_map);
            data_ptr->InsertData(&temp_map);
        } else if constexpr (std::is_same_v<CardinalityTag, LowCardinalityTag>) {
            auto data_ptr = static_cast<SecondaryIndexDataBase<LowCardinalityTag> *>(handle.GetDataMut());
            std::multimap<KeyType, u32> temp_map;
            const_cast<RcuMultiMap<KeyType, u32> &>(in_mem_secondary_index_).GetMergedMultiMap(temp_map);
            data_ptr->InsertData(&temp_map);
        } else {
            UnrecoverableError("Unsupported cardinality tag type");
        }
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

const ChunkIndexMetaInfo SecondaryIndexInMem::GetChunkIndexMetaInfo() const { return ChunkIndexMetaInfo{"", GetBeginRowID(), GetRowCount(), 0, 0}; }

std::shared_ptr<SecondaryIndexInMem>
SecondaryIndexInMem::NewSecondaryIndexInMem(const std::shared_ptr<ColumnDef> &column_def, RowID begin_row_id, SecondaryIndexCardinality cardinality) {
    if (!column_def->type()->CanBuildSecondaryIndex()) {
        UnrecoverableError("Column type can't build secondary index");
    }

    // Select template specialization based on cardinality
    if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
        switch (column_def->type()->type()) {
            case LogicalType::kTinyInt: {
                return std::make_shared<SecondaryIndexInMemT<TinyIntT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kSmallInt: {
                return std::make_shared<SecondaryIndexInMemT<SmallIntT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kInteger: {
                return std::make_shared<SecondaryIndexInMemT<IntegerT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kBigInt: {
                return std::make_shared<SecondaryIndexInMemT<BigIntT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kFloat: {
                return std::make_shared<SecondaryIndexInMemT<FloatT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDouble: {
                return std::make_shared<SecondaryIndexInMemT<DoubleT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDate: {
                return std::make_shared<SecondaryIndexInMemT<DateT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kTime: {
                return std::make_shared<SecondaryIndexInMemT<TimeT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDateTime: {
                return std::make_shared<SecondaryIndexInMemT<DateTimeT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kTimestamp: {
                return std::make_shared<SecondaryIndexInMemT<TimestampT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kVarchar: {
                return std::make_shared<SecondaryIndexInMemT<VarcharT, HighCardinalityTag>>(begin_row_id, cardinality);
            }
            default: {
                return nullptr;
            }
        }
    } else if (cardinality == SecondaryIndexCardinality::kLowCardinality) {
        switch (column_def->type()->type()) {
            case LogicalType::kBoolean: {
                return std::make_shared<SecondaryIndexInMemT<BooleanT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kTinyInt: {
                return std::make_shared<SecondaryIndexInMemT<TinyIntT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kSmallInt: {
                return std::make_shared<SecondaryIndexInMemT<SmallIntT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kInteger: {
                return std::make_shared<SecondaryIndexInMemT<IntegerT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kBigInt: {
                return std::make_shared<SecondaryIndexInMemT<BigIntT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kFloat: {
                return std::make_shared<SecondaryIndexInMemT<FloatT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDouble: {
                return std::make_shared<SecondaryIndexInMemT<DoubleT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDate: {
                return std::make_shared<SecondaryIndexInMemT<DateT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kTime: {
                return std::make_shared<SecondaryIndexInMemT<TimeT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kDateTime: {
                return std::make_shared<SecondaryIndexInMemT<DateTimeT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kTimestamp: {
                return std::make_shared<SecondaryIndexInMemT<TimestampT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            case LogicalType::kVarchar: {
                return std::make_shared<SecondaryIndexInMemT<VarcharT, LowCardinalityTag>>(begin_row_id, cardinality);
            }
            default: {
                return nullptr;
            }
        }
    } else {
        UnrecoverableError("Invalid secondary index cardinality");
        return nullptr;
    }
}

} // namespace infinity