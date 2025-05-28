//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

#include <algorithm>
#include <string_view>
module build_fast_rough_filter_task;

import stl;
import block_entry;
import segment_entry;
import segment_iter;
import infinity_exception;
import internal_types;
import logical_type;
import logger;
import buffer_manager;
import block_column_entry;
import column_vector;
import value;
import third_party;
import block_column_iter;
import probabilistic_data_filter;
import min_max_data_filter;
import fast_rough_filter;
import filter_value_type_classification;
import new_catalog;
import segment_meta;
import block_meta;
import column_meta;
import table_meeta;
import status;
import default_values;

template <>
class std::numeric_limits<infinity::InnerMinMaxDataFilterVarcharType> {
public:
    static constexpr infinity::InnerMinMaxDataFilterVarcharType lowest() { return infinity::InnerMinMaxDataFilterVarcharType::lowest(); };
    static constexpr infinity::InnerMinMaxDataFilterVarcharType max() { return infinity::InnerMinMaxDataFilterVarcharType::max(); }
};

template <>
class std::numeric_limits<infinity::HugeIntT> {
public:
    static constexpr infinity::HugeIntT lowest() {
        return infinity::HugeIntT(std::numeric_limits<infinity::i64>::lowest(), std::numeric_limits<infinity::i64>::lowest());
    };
    static constexpr infinity::HugeIntT max() {
        return infinity::HugeIntT(std::numeric_limits<infinity::i64>::max(), std::numeric_limits<infinity::i64>::max());
    }
};

template <>
class std::numeric_limits<infinity::DateT> {
public:
    static constexpr infinity::DateT lowest() { return infinity::DateT(std::numeric_limits<infinity::i32>::lowest()); };
    static constexpr infinity::DateT max() { return infinity::DateT(std::numeric_limits<infinity::i32>::max()); }
};

template <>
class std::numeric_limits<infinity::TimeT> {
public:
    static constexpr infinity::TimeT lowest() { return infinity::TimeT(std::numeric_limits<infinity::i32>::lowest()); };
    static constexpr infinity::TimeT max() { return infinity::TimeT(std::numeric_limits<infinity::i32>::max()); }
};

template <>
class std::numeric_limits<infinity::DateTimeT> {
public:
    static constexpr infinity::DateTimeT lowest() {
        return infinity::DateTimeT(std::numeric_limits<infinity::i32>::lowest(), std::numeric_limits<infinity::i32>::lowest());
    };
    static constexpr infinity::DateTimeT max() {
        return infinity::DateTimeT(std::numeric_limits<infinity::i32>::max(), std::numeric_limits<infinity::i32>::max());
    }
};

template <>
class std::numeric_limits<infinity::TimestampT> {
public:
    static constexpr infinity::TimestampT lowest() {
        return infinity::TimestampT(std::numeric_limits<infinity::i32>::lowest(), std::numeric_limits<infinity::i32>::lowest());
    };
    static constexpr infinity::TimestampT max() {
        return infinity::TimestampT(std::numeric_limits<infinity::i32>::max(), std::numeric_limits<infinity::i32>::max());
    }
};

namespace infinity {

auto operator<=>(const InnerMinMaxDataFilterVarcharType &lhs, const InnerMinMaxDataFilterVarcharType &rhs) {
    return lhs.GetStringView() <=> rhs.GetStringView();
}

auto operator==(const InnerMinMaxDataFilterVarcharType &lhs, const InnerMinMaxDataFilterVarcharType &rhs) {
    return lhs.GetStringView() == rhs.GetStringView();
}

template <typename InnerValueType>
void UpdateMin(InnerValueType &min, const InnerValueType &value) {
    if (value < min) {
        min = value;
    }
}
template <typename InnerValueType>
void UpdateMax(InnerValueType &max, const InnerValueType &value) {
    if (value > max) {
        max = value;
    }
}

void UpdateMin(InnerMinMaxDataFilterVarcharType &min, const String &input_str) {
    std::string_view input_str_view(input_str.begin(), input_str.end());
    if (input_str_view < min.GetStringView()) {
        // update min, truncate if necessary
        min.SetToTruncate(input_str_view);
    }
}

void UpdateMax(InnerMinMaxDataFilterVarcharType &max, const String &input_str) {
    std::string_view input_str_view(input_str.begin(), input_str.end());
    if (input_str_view > max.GetStringView()) {
        // update max, truncate if necessary
        max.SetToTruncate(input_str_view);
    }
}

inline void Advance(TotalRowCount &total_row_count_handler) {
    if (++total_row_count_handler.total_row_count_read_ > total_row_count_handler.total_row_count_in_segment_) {
        String error_message = "BuildFastRoughFilterArg: total_row_count overflow";
        UnrecoverableError(error_message);
    }
}

UniquePtr<BuildingSegmentFastFilters> BuildingSegmentFastFilters::Make(SegmentMeta *segment_meta) {
    auto [block_ids_ptr, status] = segment_meta->GetBlockIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    auto segment_filters = MakeUnique<BuildingSegmentFastFilters>();
    segment_filters->segment_meta_ = segment_meta;
    segment_filters->segment_filter_ = MakeShared<FastRoughFilter>();
    for (BlockID block_id : *block_ids_ptr) {
        segment_filters->block_filters_.emplace(block_id, MakeShared<FastRoughFilter>());
    }
    return segment_filters;
}

BuildingSegmentFastFilters::~BuildingSegmentFastFilters() = default;

void BuildingSegmentFastFilters::ApplyToAllFastRoughFilterInSegment(std::invocable<FastRoughFilter *> auto func) {
    for (auto &[block_id, block_filter] : block_filters_) {
        func(block_filter.get());
    }
    func(segment_filter_.get());
}

void BuildingSegmentFastFilters::CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask() {
    TxnTimeStamp begin_ts = segment_meta_->begin_ts();
    ApplyToAllFastRoughFilterInSegment([begin_ts](FastRoughFilter *filter) { filter->SetHaveStartedMinMaxFilterBuildTask(begin_ts); });
}

void BuildingSegmentFastFilters::SetSegmentBeginBuildMinMaxFilterTask(u32 column_count) {
    ApplyToAllFastRoughFilterInSegment([column_count](FastRoughFilter *filter) { filter->BeginBuildMinMaxFilterTask(column_count); });
}

void BuildingSegmentFastFilters::SetSegmentFinishBuildMinMaxFilterTask() {
    ApplyToAllFastRoughFilterInSegment([](FastRoughFilter *filter) { filter->FinishBuildMinMaxFilterTask(); });
}

void BuildingSegmentFastFilters::SetFilter() {
    for (auto &[block_id, block_filter] : block_filters_) {
        BlockMeta block_meta(block_id, *segment_meta_);
        Status status = block_meta.SetFastRoughFilter(block_filter);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
    }
    Status status = segment_meta_->SetFastRoughFilter(segment_filter_);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

NewBuildFastRoughFilterArg::NewBuildFastRoughFilterArg(SegmentMeta *segment_meta,
                                                       ColumnID column_id,
                                                       UniquePtr<u64[]> &distinct_keys,
                                                       UniquePtr<u64[]> &distinct_keys_backup,
                                                       BuildingSegmentFastFilters *segment_filters)
    : segment_meta_(segment_meta), column_id_(column_id), distinct_keys_(distinct_keys), distinct_keys_backup_(distinct_keys_backup),
      segment_filters_(segment_filters) {}

NewBuildFastRoughFilterArg::~NewBuildFastRoughFilterArg() = default;

template <CanBuildBloomFilter ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildOnlyBloomFilter(BuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyBloomFilter job begin for column: {}", arg.column_id_));
    if (!arg.distinct_keys_) {
        arg.distinct_keys_ = MakeUniqueForOverwrite<u64[]>(arg.total_row_count_in_segment_);
        arg.distinct_keys_backup_ = MakeUniqueForOverwrite<u64[]>(arg.total_row_count_in_segment_);
    }
    auto iter = BlockEntryIter(arg.segment_entry_);
    Vector<u64> input_data; // for reuse
    for (auto *block_entry = iter.Next(); block_entry != nullptr; block_entry = iter.Next()) {
        // check row count
        u32 block_row_cnt = block_entry->row_count();
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        // clear
        input_data.clear();
        // prepare column_iter
        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(arg.column_id_);
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_, block_row_cnt);
        // step 1. swap arg.distinct_keys_ and arg.distinct_keys_backup_
        std::swap(arg.distinct_keys_, arg.distinct_keys_backup_);
        // step 2. collect data in row
        if constexpr (std::is_same_v<ValueType, BooleanT>) {
            // for boolean, only 0 and 1
            bool have_0 = false;
            bool have_1 = false;
            auto *u8_ptr = reinterpret_cast<const u8 *>(column_iter.data());
            for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
                Advance(arg.total_row_count_handler_); // need to count actual row count
                if (!have_0 or !have_1) {
                    // need to access data
                    auto &[_, offset] = next_pair.value();
                    auto [byte_cnt, remain_cnt] = std::div(offset, 8);
                    bool val = u8_ptr[byte_cnt] & (u8(1) << remain_cnt);
                    if (val) {
                        have_1 = true;
                    } else {
                        have_0 = true;
                    }
                }
            }
            if (have_0) {
                BooleanT false_val(false);
                input_data.push_back(ConvertValueToU64(false_val));
            }
            if (have_1) {
                BooleanT true_val(true);
                input_data.push_back(ConvertValueToU64(true_val));
            }
        } else {
            for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
                Advance(arg.total_row_count_handler_);
                auto &[ptr, offset] = next_pair.value();
                if constexpr (std::is_same_v<ValueType, VarcharT>) {
                    Value val = column_iter.column_vector()->GetValue(offset);
                    const String &str = val.GetVarchar();
                    input_data.push_back(ConvertValueToU64(str));
                } else {
                    const auto &val = *static_cast<const ValueType *>(ptr);
                    input_data.push_back(ConvertValueToU64(val));
                }
            }
        }
        // step 3. sort data and remove duplicate
        std::sort(input_data.begin(), input_data.end());
        u32 input_distinct_count = std::unique(input_data.begin(), input_data.end()) - input_data.begin();
        // step 4. build probabilistic_data_filter for block
        block_entry->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.begin_ts_, arg.column_id_, input_data.data(), input_distinct_count);
        // step 5. merge u64 distinct key array
        arg.distinct_count_ = std::set_union(arg.distinct_keys_backup_.get(),
                                             arg.distinct_keys_backup_.get() + arg.distinct_count_,
                                             input_data.begin(),
                                             input_data.begin() + input_distinct_count,
                                             arg.distinct_keys_.get()) -
                              arg.distinct_keys_.get();
    }
    // finally, build probabilistic_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.begin_ts_,
                                                                           arg.column_id_,
                                                                           arg.distinct_keys_.get(),
                                                                           arg.distinct_count_);
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyBloomFilter job end for column: {}", arg.column_id_));
}

template <CanBuildBloomFilter ValueType>
void BuildFastRoughFilterTask::BuildOnlyBloomFilter(NewBuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyBloomFilter job begin for column: {}", arg.column_id_));
    TxnTimeStamp begin_ts = arg.segment_meta_->begin_ts();
    auto [block_ids_ptr, status] = arg.segment_meta_->GetBlockIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    if (!arg.distinct_keys_) {
        SizeT total_row_count_in_segment = block_ids_ptr->size() * DEFAULT_BLOCK_CAPACITY;
        arg.distinct_keys_ = MakeUniqueForOverwrite<u64[]>(total_row_count_in_segment);
        arg.distinct_keys_backup_ = MakeUniqueForOverwrite<u64[]>(total_row_count_in_segment);
    }
    Vector<u64> input_data; // for reuse
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, *arg.segment_meta_);
        // check row count
        auto [block_row_cnt, status] = block_meta.GetRowCnt1();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        ColumnMeta column_meta(arg.column_id_, block_meta);
        ColumnVector column_vector;
        status = NewCatalog::GetColumnVector(column_meta, block_row_cnt, ColumnVectorTipe::kReadOnly, column_vector);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        // step 1. swap arg.distinct_keys_ and arg.distinct_keys_backup_
        std::swap(arg.distinct_keys_, arg.distinct_keys_backup_);
        // step 2. collect data in row
        NewTxnGetVisibleRangeState visit_state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, visit_state);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        NewTxnBlockVisitor block_visitor(&visit_state);

        if constexpr (std::is_same_v<ValueType, BooleanT>) {
            // for boolean, only 0 and 1
            bool have_0 = false;
            bool have_1 = false;
            auto *u8_ptr = reinterpret_cast<const u8 *>(column_vector.data());
            for (auto block_off_opt = block_visitor.Next(); block_off_opt; block_off_opt = block_visitor.Next()) {
                BlockOffset block_off = *block_off_opt;
                if (!have_0 or !have_1) {
                    // need to access data
                    auto [byte_cnt, remain_cnt] = std::div(block_off, 8);
                    bool val = u8_ptr[byte_cnt] & (u8(1) << remain_cnt);
                    if (val) {
                        have_1 = true;
                    } else {
                        have_0 = true;
                    }
                }
            }
            if (have_0) {
                BooleanT false_val(false);
                input_data.push_back(ConvertValueToU64(false_val));
            }
            if (have_1) {
                BooleanT true_val(true);
                input_data.push_back(ConvertValueToU64(true_val));
            }
        } else {
            for (SizeT block_off = 0; block_off < block_row_cnt; ++block_off) {
                if constexpr (std::is_same_v<ValueType, VarcharT>) {
                    Value val = column_vector.GetValue(block_off);
                    const String &str = val.GetVarchar();
                    input_data.push_back(ConvertValueToU64(str));
                } else {
                    const auto &val = *reinterpret_cast<const ValueType *>(column_vector.GetRawPtr(block_off));
                    input_data.push_back(ConvertValueToU64(val));
                }
            }
        }
        // step 3. sort data and remove duplicate
        std::sort(input_data.begin(), input_data.end());
        u32 input_distinct_count = std::unique(input_data.begin(), input_data.end()) - input_data.begin();
        // step 4. build probabilistic_data_filter for block
        auto block_filter = arg.segment_filters_->block_filters_[block_id];
        block_filter->BuildProbabilisticDataFilter(begin_ts, arg.column_id_, input_data.data(), input_distinct_count);
        // step 5. merge u64 distinct key array
        arg.distinct_count_ = std::set_union(arg.distinct_keys_backup_.get(),
                                             arg.distinct_keys_backup_.get() + arg.distinct_count_,
                                             input_data.begin(),
                                             input_data.begin() + input_distinct_count,
                                             arg.distinct_keys_.get()) -
                              arg.distinct_keys_.get();
    }
    // finally, build probabilistic_data_filter for segment
    auto segment_filter = arg.segment_filters_->segment_filter_;
    segment_filter->BuildProbabilisticDataFilter(begin_ts, arg.column_id_, arg.distinct_keys_.get(), arg.distinct_count_);
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyBloomFilter job end for column: {}", arg.column_id_));
}

template <CanBuildMinMaxFilter ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildOnlyMinMaxFilter(BuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyMinMaxFilter job begin for column: {}", arg.column_id_));
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
    // step 0. prepare min and max value
    MinMaxInnerValueType segment_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
    MinMaxInnerValueType segment_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
    auto iter = BlockEntryIter(arg.segment_entry_);
    for (auto *block_entry = iter.Next(); block_entry != nullptr; block_entry = iter.Next()) {
        // check row count
        u32 block_row_cnt = block_entry->row_count();
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        // step 1. update min and max value for block
        MinMaxInnerValueType block_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
        MinMaxInnerValueType block_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(arg.column_id_);
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_, block_row_cnt);
        for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
            Advance(arg.total_row_count_handler_);
            auto &[ptr, offset] = next_pair.value();
            if constexpr (std::is_same_v<ValueType, VarcharT>) {
                Value val = column_iter.column_vector()->GetValue(offset);
                const String &str = val.GetVarchar();
                UpdateMin(block_min_value, str);
                UpdateMax(block_max_value, str);
            } else {
                const auto &val = *static_cast<const ValueType *>(ptr);
                UpdateMin(block_min_value, val);
                UpdateMax(block_max_value, val);
            }
        }
        // step 2. merge min, max for segment
        UpdateMin(segment_min_value, block_min_value);
        UpdateMax(segment_max_value, block_max_value);
        // step 3. build min_max_data_filter for block
        block_entry->GetFastRoughFilter()->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(block_min_value), std::move(block_max_value));
    }
    // finally, build min_max_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildMinMaxDataFilter<ValueType>(arg.column_id_,
                                                                               std::move(segment_min_value),
                                                                               std::move(segment_max_value));
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyMinMaxFilter job end for column: {}", arg.column_id_));
}

template <CanBuildMinMaxFilter ValueType>
void BuildFastRoughFilterTask::BuildOnlyMinMaxFilter(NewBuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildOnlyMinMaxFilter job begin for column: {}", arg.column_id_));
    TxnTimeStamp begin_ts = arg.segment_meta_->begin_ts();
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
    // step 0. prepare min and max value
    MinMaxInnerValueType segment_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
    MinMaxInnerValueType segment_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
    auto [block_ids_ptr, status] = arg.segment_meta_->GetBlockIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, *arg.segment_meta_);
        // check row count
        auto [block_row_cnt, status] = block_meta.GetRowCnt1();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        // step 1. update min and max value for block
        MinMaxInnerValueType block_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
        MinMaxInnerValueType block_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();

        ColumnMeta column_meta(arg.column_id_, block_meta);
        ColumnVector column_vector;
        status = NewCatalog::GetColumnVector(column_meta, block_row_cnt, ColumnVectorTipe::kReadOnly, column_vector);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        NewTxnGetVisibleRangeState visit_state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, visit_state);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        NewTxnBlockVisitor block_visitor(&visit_state);
        for (auto block_off_opt = block_visitor.Next(); block_off_opt; block_off_opt = block_visitor.Next()) {
            BlockOffset block_off = *block_off_opt;
            if constexpr (std::is_same_v<ValueType, VarcharT>) {
                Value val = column_vector.GetValue(block_off);
                const String &str = val.GetVarchar();
                UpdateMin(block_min_value, str);
                UpdateMax(block_max_value, str);
            } else {
                const auto &val = *reinterpret_cast<const ValueType *>(column_vector.GetRawPtr(block_off));
                UpdateMin(block_min_value, val);
                UpdateMax(block_max_value, val);
            }
        }
        // step 2. merge min, max for segment
        UpdateMin(segment_min_value, block_min_value);
        UpdateMax(segment_max_value, block_max_value);
        // step 3. build min_max_data_filter for block
        auto block_filter = arg.segment_filters_->block_filters_[block_id];
        block_filter->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(block_min_value), std::move(block_max_value));
    }
    // finally, build min_max_data_filter for segment
    auto segment_filter = arg.segment_filters_->segment_filter_;
    segment_filter->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(segment_min_value), std::move(segment_max_value));
}

template <CanBuildMinMaxFilterAndBloomFilter ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildMinMaxAndBloomFilter(BuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildMinMaxAndBloomFilter job begin for column: {}", arg.column_id_));
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
    if (!arg.distinct_keys_) {
        arg.distinct_keys_ = MakeUniqueForOverwrite<u64[]>(arg.total_row_count_in_segment_);
        arg.distinct_keys_backup_ = MakeUniqueForOverwrite<u64[]>(arg.total_row_count_in_segment_);
    }
    // step 0. prepare min and max value
    MinMaxInnerValueType segment_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
    MinMaxInnerValueType segment_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
    auto iter = BlockEntryIter(arg.segment_entry_);
    Vector<u64> input_data; // for reuse
    for (auto *block_entry = iter.Next(); block_entry != nullptr; block_entry = iter.Next()) {
        // check row count
        u32 block_row_cnt = block_entry->row_count();
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        // clear
        input_data.clear();
        // prepare column_iter
        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(arg.column_id_);
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_, block_row_cnt);
        // step 1. swap arg.distinct_keys_ and arg.distinct_keys_backup_
        std::swap(arg.distinct_keys_, arg.distinct_keys_backup_);
        // step 2. collect data in row, get and update min and max value
        MinMaxInnerValueType block_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
        MinMaxInnerValueType block_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
        for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
            Advance(arg.total_row_count_handler_);
            auto &[ptr, offset] = next_pair.value();
            if constexpr (std::is_same_v<ValueType, VarcharT>) {
                Value val = column_iter.column_vector()->GetValue(offset);
                const String &str = val.GetVarchar();
                UpdateMin(block_min_value, str);
                UpdateMax(block_max_value, str);
                input_data.push_back(ConvertValueToU64(str));
            } else {
                const auto &val = *static_cast<const ValueType *>(ptr);
                UpdateMin(block_min_value, val);
                UpdateMax(block_max_value, val);
                input_data.push_back(ConvertValueToU64(val));
            }
        }
        UpdateMin(segment_min_value, block_min_value);
        UpdateMax(segment_max_value, block_max_value);
        // step 3. sort data and remove duplicate
        std::sort(input_data.begin(), input_data.end());
        u32 input_distinct_count = std::unique(input_data.begin(), input_data.end()) - input_data.begin();
        // step 4. build probabilistic_data_filter and min_max_data_filter for block
        block_entry->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.begin_ts_, arg.column_id_, input_data.data(), input_distinct_count);
        block_entry->GetFastRoughFilter()->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(block_min_value), std::move(block_max_value));
        // step 5. merge u64 distinct key array
        arg.distinct_count_ = std::set_union(arg.distinct_keys_backup_.get(),
                                             arg.distinct_keys_backup_.get() + arg.distinct_count_,
                                             input_data.begin(),
                                             input_data.begin() + input_distinct_count,
                                             arg.distinct_keys_.get()) -
                              arg.distinct_keys_.get();
    }
    // finally, build probabilistic_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.begin_ts_,
                                                                           arg.column_id_,
                                                                           arg.distinct_keys_.get(),
                                                                           arg.distinct_count_);
    // finally, build min_max_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildMinMaxDataFilter<ValueType>(arg.column_id_,
                                                                               std::move(segment_min_value),
                                                                               std::move(segment_max_value));
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildMinMaxAndBloomFilter job end for column: {}", arg.column_id_));
}

template <CanBuildMinMaxFilterAndBloomFilter ValueType>
void BuildFastRoughFilterTask::BuildMinMaxAndBloomFilter(NewBuildFastRoughFilterArg &arg) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildMinMaxAndBloomFilter job begin for column: {}", arg.column_id_));
    TxnTimeStamp begin_ts = arg.segment_meta_->begin_ts();
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
    // step 0. prepare min and max value
    MinMaxInnerValueType segment_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
    MinMaxInnerValueType segment_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
    auto [block_ids_ptr, status] = arg.segment_meta_->GetBlockIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    if (!arg.distinct_keys_) {
        SizeT total_row_count_in_segment = block_ids_ptr->size() * DEFAULT_BLOCK_CAPACITY;
        arg.distinct_keys_ = MakeUniqueForOverwrite<u64[]>(total_row_count_in_segment);
        arg.distinct_keys_backup_ = MakeUniqueForOverwrite<u64[]>(total_row_count_in_segment);
    }
    Vector<u64> input_data; // for reuse
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, *arg.segment_meta_);
        // check row count
        auto [block_row_cnt, status] = block_meta.GetRowCnt1();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        if (block_row_cnt == 0) {
            // skip empty block
            continue;
        }
        // clear
        input_data.clear();
        ColumnMeta column_meta(arg.column_id_, block_meta);
        ColumnVector column_vector;
        status = NewCatalog::GetColumnVector(column_meta, block_row_cnt, ColumnVectorTipe::kReadOnly, column_vector);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        NewTxnGetVisibleRangeState visit_state;
        status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, visit_state);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        NewTxnBlockVisitor block_visitor(&visit_state);

        // step 1. swap arg.distinct_keys_ and arg.distinct_keys_backup_
        std::swap(arg.distinct_keys_, arg.distinct_keys_backup_);
        // step 2. collect data in row, get and update min and max value
        MinMaxInnerValueType block_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
        MinMaxInnerValueType block_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
        for (auto block_off_opt = block_visitor.Next(); block_off_opt; block_visitor.Next()) {
            BlockOffset block_off = *block_off_opt;
            Value val = column_vector.GetValue(block_off);
            if constexpr (std::is_same_v<ValueType, VarcharT>) {
                const String &str = val.GetVarchar();
                UpdateMin(block_min_value, str);
                UpdateMax(block_max_value, str);
            } else {
                const auto &val = *reinterpret_cast<const ValueType *>(column_vector.GetRawPtr(block_off));
                UpdateMin(block_min_value, val);
                UpdateMax(block_max_value, val);
                input_data.push_back(ConvertValueToU64(val));
            }
        }
        UpdateMin(segment_min_value, block_min_value);
        UpdateMax(segment_max_value, block_max_value);
        // step 3. sort data and remove duplicate
        std::sort(input_data.begin(), input_data.end());
        u32 input_distinct_count = std::unique(input_data.begin(), input_data.end()) - input_data.begin();
        // step 4. build probabilistic_data_filter and min_max_data_filter for block
        auto block_filter = arg.segment_filters_->block_filters_[block_id];
        block_filter->BuildProbabilisticDataFilter(begin_ts, arg.column_id_, input_data.data(), input_distinct_count);
        block_filter->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(block_min_value), std::move(block_max_value));
        // step 5. merge u64 distinct key array
        arg.distinct_count_ = std::set_union(arg.distinct_keys_backup_.get(),
                                             arg.distinct_keys_backup_.get() + arg.distinct_count_,
                                             input_data.begin(),
                                             input_data.begin() + input_distinct_count,
                                             arg.distinct_keys_.get()) -
                              arg.distinct_keys_.get();
    }
    // finally, build probabilistic_data_filter and min_max_data_filter for segment
    auto segment_filter = arg.segment_filters_->segment_filter_;
    segment_filter->BuildProbabilisticDataFilter(begin_ts, arg.column_id_, arg.distinct_keys_.get(), arg.distinct_count_);
    segment_filter->BuildMinMaxDataFilter<ValueType>(arg.column_id_, std::move(segment_min_value), std::move(segment_max_value));
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: BuildMinMaxAndBloomFilter job end for column: {}", arg.column_id_));
}

void ApplyToAllFastRoughFilterInSegment(SegmentEntry *segment_entry, std::invocable<FastRoughFilter *> auto func) {
    // first, apply to block_entry
    BlockEntryIter block_entry_iter{segment_entry};
    for (auto *block_entry = block_entry_iter.Next(); block_entry; block_entry = block_entry_iter.Next()) {
        func(block_entry->GetFastRoughFilter());
    }
    // then, apply to segment_entry
    func(segment_entry->GetFastRoughFilter());
}

void BuildFastRoughFilterTask::CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask(SegmentEntry *segment, TxnTimeStamp begin_ts) {
    ApplyToAllFastRoughFilterInSegment(segment, [begin_ts](FastRoughFilter *filter) { filter->SetHaveStartedMinMaxFilterBuildTask(begin_ts); });
}

void BuildFastRoughFilterTask::SetSegmentBeginBuildMinMaxFilterTask(SegmentEntry *segment, u32 column_count) {
    ApplyToAllFastRoughFilterInSegment(segment, [column_count](FastRoughFilter *filter) { filter->BeginBuildMinMaxFilterTask(column_count); });
}

void BuildFastRoughFilterTask::SetSegmentFinishBuildMinMaxFilterTask(SegmentEntry *segment) {
    ApplyToAllFastRoughFilterInSegment(segment, [](FastRoughFilter *filter) { filter->FinishBuildMinMaxFilterTask(); });
}

// deprecate except this
void BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts) {
    bool use_block_version = false;
    switch (auto status = segment_entry->status(); status) {
        case SegmentStatus::kUnsealed: {
            // segment is set full by append, use block version
            use_block_version = true;
            LOG_TRACE(fmt::format("BuildFastRoughFilterTask: segment {} status {} is unsealed, full because of append",
                                  segment_entry->segment_id(),
                                  static_cast<std::underlying_type_t<SegmentStatus>>(status)));
            break;
        }
        case SegmentStatus::kSealed: {
            // segment is created by import / compact, does not have block version
            use_block_version = false;
            LOG_TRACE(fmt::format("BuildFastRoughFilterTask: segment {} status {} may be created by import / compact",
                                  segment_entry->segment_id(),
                                  static_cast<std::underlying_type_t<SegmentStatus>>(status)));
            break;
        }
        default: {
            String error_message = fmt::format("BuildFastRoughFilterTask: segment {} status {} cannot build filter",
                                               segment_entry->segment_id(),
                                               static_cast<std::underlying_type_t<SegmentStatus>>(status));
            UnrecoverableError(error_message);
            return;
        }
    }
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job begin.", segment_entry->segment_id()));
    // step 1. when build minmax, set timestamp
    CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask(segment_entry, begin_ts);
    // step2. when build minmax, init filters to size of column_count
    const u32 column_count = segment_entry->column_count();
    SetSegmentBeginBuildMinMaxFilterTask(segment_entry, column_count);
    // step 3. build filter
    if (use_block_version) {
        ExecuteInner<true>(segment_entry, buffer_manager, begin_ts);
    } else {
        ExecuteInner<false>(segment_entry, buffer_manager, begin_ts);
    }
    // step 4. set finish build MinMax atomic flag
    SetSegmentFinishBuildMinMaxFilterTask(segment_entry);
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job end.", segment_entry->segment_id()));
}

// deprecate except this
void BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(SegmentMeta *segment_meta) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job begin.", segment_meta->segment_id()));

    auto segment_filters = BuildingSegmentFastFilters::Make(segment_meta);
    // step 1. when building minmax, set timestamp
    segment_filters->CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask();
    // step2. when building minmax, init filters to size of column_count
    u32 column_count = 0;
    {
        auto [column_defs, status] = segment_meta->table_meta().GetColumnDefs();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        column_count = column_defs->size();
    }
    segment_filters->SetSegmentBeginBuildMinMaxFilterTask(column_count);
    // step 3. build filter
    ExecuteInner(segment_meta, segment_filters.get());
    // step 4. set finish build MinMax atomic flag
    segment_filters->SetSegmentFinishBuildMinMaxFilterTask();
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job end.", segment_meta->segment_id()));

    segment_filters->SetFilter();
}

void BuildFastRoughFilterTask::ExecuteUpdateSegmentBloomFilter(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts) {}

// will check every column
template <bool CheckTS>
void BuildFastRoughFilterTask::ExecuteInner(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts) {
    const u32 column_count = segment_entry->column_count();
    const u32 segment_row_count = segment_entry->row_count();
    // total_row_count_in_segment may be greater than rows actually read,
    // because there may be deleted rows
    UniquePtr<u64[]> distinct_keys = nullptr;
    UniquePtr<u64[]> distinct_keys_backup = nullptr;
    const auto *table_entry = segment_entry->GetTableEntry();
    for (u32 column_id = 0; column_id < column_count; ++column_id) {
        // step 2.1. check data type
        auto *column_def = table_entry->GetColumnDefByIdx(column_id);
        auto &data_type_ptr = column_def->type();
        bool can_build_min_max_data_filter = data_type_ptr->SupportMinMaxFilter();
        bool can_build_probabilistic_data_filter = data_type_ptr->SupportBloomFilter();
        bool build_min_max_filter = can_build_min_max_data_filter;
        // check column def
        bool build_bloom_filter = can_build_probabilistic_data_filter and column_def->build_bloom_filter_;
        bool no_skip = build_min_max_filter or build_bloom_filter;
        if (!no_skip) {
            // skip non-support data type
            continue;
        }
        // step 2.2. collect distinct data from blocks and build probabilistic_data_filter for blocks and segment
        BuildFastRoughFilterArg arg(segment_entry, column_id, distinct_keys, distinct_keys_backup, buffer_manager, begin_ts, segment_row_count);
        switch (data_type_ptr->type()) {
            case LogicalType::kBoolean: {
                BuildFilter<BooleanT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDecimal: {
                // TODO: DecimalT only support "==", cannot support MinMaxDataFilter
                BuildFilter<DecimalT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kFloat: {
                BuildFilter<FloatT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDouble: {
                BuildFilter<DoubleT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTinyInt: {
                BuildFilter<TinyIntT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kSmallInt: {
                BuildFilter<SmallIntT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kInteger: {
                BuildFilter<IntegerT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kBigInt: {
                BuildFilter<BigIntT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kHugeInt: {
                BuildFilter<HugeIntT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kVarchar: {
                BuildFilter<VarcharT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDate: {
                BuildFilter<DateT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTime: {
                BuildFilter<TimeT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDateTime: {
                BuildFilter<DateTimeT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTimestamp: {
                BuildFilter<TimestampT, CheckTS>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            default: {
                String error_message = "BuildFastRoughFilterTask: unsupported data type";
                UnrecoverableError(error_message);
            }
        }
        // step 2.3. check total_row_count
        if (!arg.total_row_count_handler_.IsAtEnd()) {
            LOG_TRACE(fmt::format("BuildFastRoughFilterTask: read row count ({}) < segment_row_count ({}), maybe some rows are deleted",
                                  arg.total_row_count_handler_.total_row_count_read_,
                                  segment_row_count));
            if (arg.total_row_count_handler_.total_row_count_read_ == 0 and segment_row_count != 0) {
                LOG_TRACE(fmt::format("BuildFastRoughFilterTask: read row count is 0, but segment is not empty!"));
            }
        }
    }
}

void BuildFastRoughFilterTask::ExecuteInner(SegmentMeta *segment_meta, BuildingSegmentFastFilters *segment_filters) {
    UniquePtr<u64[]> distinct_keys = nullptr;
    UniquePtr<u64[]> distinct_keys_backup = nullptr;
    auto [column_defs, status] = segment_meta->table_meta().GetColumnDefs();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    for (SizeT column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
        // step 2.1. check data type
        auto *column_def = (*column_defs)[column_idx].get();
        // ColumnID column_id = column_def->id();

        auto &data_type_ptr = column_def->type();
        bool can_build_min_max_data_filter = data_type_ptr->SupportMinMaxFilter();
        bool can_build_probabilistic_data_filter = data_type_ptr->SupportBloomFilter();
        bool build_min_max_filter = can_build_min_max_data_filter;
        // check column def
        bool build_bloom_filter = can_build_probabilistic_data_filter and column_def->build_bloom_filter_;
        bool no_skip = build_min_max_filter or build_bloom_filter;
        if (!no_skip) {
            // skip non-support data type
            continue;
        }
        // step 2.2. collect distinct data from blocks and build probabilistic_data_filter for blocks and segment
        NewBuildFastRoughFilterArg arg(segment_meta, column_idx, distinct_keys, distinct_keys_backup, segment_filters);
        switch (data_type_ptr->type()) {
            case LogicalType::kBoolean: {
                BuildFilter<BooleanT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDecimal: {
                // TODO: DecimalT only support "==", cannot support MinMaxDataFilter
                BuildFilter<DecimalT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kFloat: {
                BuildFilter<FloatT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDouble: {
                BuildFilter<DoubleT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTinyInt: {
                BuildFilter<TinyIntT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kSmallInt: {
                BuildFilter<SmallIntT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kInteger: {
                BuildFilter<IntegerT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kBigInt: {
                BuildFilter<BigIntT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kHugeInt: {
                BuildFilter<HugeIntT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kVarchar: {
                BuildFilter<VarcharT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDate: {
                BuildFilter<DateT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTime: {
                BuildFilter<TimeT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kDateTime: {
                BuildFilter<DateTimeT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            case LogicalType::kTimestamp: {
                BuildFilter<TimestampT>(arg, build_min_max_filter, build_bloom_filter);
                break;
            }
            default: {
                String error_message = "BuildFastRoughFilterTask: unsupported data type";
                UnrecoverableError(error_message);
            }
        }
    }
}

} // namespace infinity