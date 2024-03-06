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
        UnrecoverableError("BUG: BuildFastRoughFilterArg: total_row_count overflow");
    }
}

template <IsOnlyProbabilisticFilterAcceptedTypes ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildOnlyProbabilisticDataFilter(BuildFastRoughFilterArg &arg) {
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
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_);
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
        } else if constexpr (std::is_same_v<ValueType, DecimalT>) {
            for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
                Advance(arg.total_row_count_handler_);
                auto &[ptr, _] = next_pair.value(); // unnecessary offset here
                auto v_ptr = static_cast<const ValueType *>(ptr);
                input_data.push_back(ConvertValueToU64(*v_ptr));
            }
        } else {
            static_assert(false, "BUG: BuildOnlyProbabilisticDataFilter: unsupported data type");
        }
        // step 3. sort data and remove duplicate
        std::sort(input_data.begin(), input_data.end());
        u32 input_distinct_count = std::unique(input_data.begin(), input_data.end()) - input_data.begin();
        // step 4. build probabilistic_data_filter for block
        block_entry->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.column_id_, input_data.data(), input_distinct_count);
        // step 5. merge u64 distinct key array
        arg.distinct_count_ = std::set_union(arg.distinct_keys_backup_.get(),
                                             arg.distinct_keys_backup_.get() + arg.distinct_count_,
                                             input_data.begin(),
                                             input_data.begin() + input_distinct_count,
                                             arg.distinct_keys_.get()) -
                              arg.distinct_keys_.get();
    }
    // finally, build probabilistic_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.column_id_, arg.distinct_keys_.get(), arg.distinct_count_);
}

template <IsOnlyMinMaxFilterAcceptedTypes ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildOnlyMinMaxDataFilter(BuildFastRoughFilterArg &arg) {
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
    static_assert(std::is_same_v<ValueType, MinMaxInnerValueType>, "BUG: BuildOnlyMinMaxDataFilter: ValueType != MinMaxInnerValueType");
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
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_);
        for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
            Advance(arg.total_row_count_handler_);
            auto &[ptr, _] = next_pair.value();
            const auto &val = *static_cast<const ValueType *>(ptr);
            UpdateMin(block_min_value, val);
            UpdateMax(block_max_value, val);
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
}

template <IsProbabilisticAndMinMaxFilterAcceptedTypes ValueType, bool CheckTS>
void BuildFastRoughFilterTask::BuildProbabilisticAndMinMaxDataFilter(BuildFastRoughFilterArg &arg) {
    using MinMaxHelper = InnerMinMaxDataFilterInfo<ValueType>;
    using MinMaxInnerValueType = MinMaxHelper::InnerValueType;
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
        BlockColumnIter<CheckTS> column_iter(block_column_entry, arg.buffer_manager_, arg.begin_ts_);
        // step 1. swap arg.distinct_keys_ and arg.distinct_keys_backup_
        std::swap(arg.distinct_keys_, arg.distinct_keys_backup_);
        // step 2. collect data in row, get and update min and max value
        MinMaxInnerValueType block_min_value = std::numeric_limits<MinMaxInnerValueType>::max();
        MinMaxInnerValueType block_max_value = std::numeric_limits<MinMaxInnerValueType>::lowest();
        for (auto next_pair = column_iter.Next(); next_pair; next_pair = column_iter.Next()) {
            Advance(arg.total_row_count_handler_);
            auto &[ptr, offset] = next_pair.value();
            const auto &val = *static_cast<const ValueType *>(ptr);
            if constexpr (std::is_same_v<ValueType, VarcharT>) {
                Value val = column_iter.column_vector()->GetValue(offset);
                const String &str = val.GetVarchar();
                UpdateMin(block_min_value, str);
                UpdateMax(block_max_value, str);
                input_data.push_back(ConvertValueToU64(str));
            } else {
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
        block_entry->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.column_id_, input_data.data(), input_distinct_count);
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
    arg.segment_entry_->GetFastRoughFilter()->BuildProbabilisticDataFilter(arg.column_id_, arg.distinct_keys_.get(), arg.distinct_count_);
    // finally, build min_max_data_filter for segment
    arg.segment_entry_->GetFastRoughFilter()->BuildMinMaxDataFilter<ValueType>(arg.column_id_,
                                                                               std::move(segment_min_value),
                                                                               std::move(segment_max_value));
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

void BuildFastRoughFilterTask::CheckAndSetSegmentHaveStartedBuildTask(SegmentEntry *segment, TxnTimeStamp begin_ts) {
    ApplyToAllFastRoughFilterInSegment(segment, [begin_ts](FastRoughFilter *filter) { filter->SetHaveStartedBuildTask(begin_ts); });
}

void BuildFastRoughFilterTask::SetSegmentBeginBuildFastRoughFilterTask(SegmentEntry *segment, u32 column_count) {
    ApplyToAllFastRoughFilterInSegment(segment, [column_count](FastRoughFilter *filter) { filter->BeginBuildFastRoughFilterTask(column_count); });
}

void BuildFastRoughFilterTask::SetSegmentFinishBuildFastRoughFilterTask(SegmentEntry *segment) {
    ApplyToAllFastRoughFilterInSegment(segment, [](FastRoughFilter *filter) { filter->FinishBuildFastRoughFilterTask(); });
}

void BuildFastRoughFilterTask::Execute(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts, SegmentStatus status) {
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job begin.", segment_entry->segment_id()));
    // set task status
    CheckAndSetSegmentHaveStartedBuildTask(segment_entry, begin_ts);
    // the fast_rough_filter will only be built once here
    if (segment_entry->status() != status) {
        UnrecoverableError("BUG: BuildFastRoughFilterTask: segment status error");
    }
    switch (status) {
        case SegmentStatus::kSealing:
            ExecuteInner<true>(segment_entry, buffer_manager, begin_ts);
            break;
        case SegmentStatus::kSealed:
            ExecuteInner<false>(segment_entry, buffer_manager, begin_ts);
            break;
        default:
            UnrecoverableError("BUG: BuildFastRoughFilterTask: segment status error");
            break;
    }
}

template <bool CheckTS>
void BuildFastRoughFilterTask::ExecuteInner(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts) {
    const u32 column_count = segment_entry->column_count();
    const u32 segment_row_count = segment_entry->row_count();
    // total_row_count_in_segment may be greater than rows actually read,
    // because there may be deleted rows
    // step 1. begin build FastRoughFilter
    SetSegmentBeginBuildFastRoughFilterTask(segment_entry, column_count);
    // step 2. add data: for each qualified column, from blocks to segment
    {
        // TODO: now cannot get actual_row_count of a specific timestamp
        auto distinct_keys = MakeUniqueForOverwrite<u64[]>(segment_row_count);
        auto distinct_keys_backup = MakeUniqueForOverwrite<u64[]>(segment_row_count);
        const auto *table_entry = segment_entry->GetTableEntry();
        for (u32 column_id = 0; column_id < column_count; ++column_id) {
            // step 2.1. check data type
            auto &data_type_ptr = table_entry->GetColumnDefByID(column_id)->type();
            bool can_build_min_max_data_filter = data_type_ptr->SupportMinMaxFilter();
            bool can_build_probabilistic_data_filter = data_type_ptr->SupportBloomFilter();
            bool no_skip = can_build_min_max_data_filter or can_build_probabilistic_data_filter;
            if (!no_skip) {
                // skip non-support data type
                continue;
            }
            // step 2.2. collect distinct data from blocks and build probabilistic_data_filter for blocks and segment
            BuildFastRoughFilterArg arg(segment_entry, column_id, distinct_keys, distinct_keys_backup, buffer_manager, begin_ts, segment_row_count);
            switch (data_type_ptr->type()) {
                case kBoolean: {
                    BuildOnlyProbabilisticDataFilter<BooleanT, CheckTS>(arg);
                    break;
                }
                case kDecimal: {
                    // TODO: DecimalT only support "==", cannot support MinMaxDataFilter
                    BuildOnlyProbabilisticDataFilter<DecimalT, CheckTS>(arg);
                    break;
                }
                case kFloat: {
                    BuildOnlyMinMaxDataFilter<FloatT, CheckTS>(arg);
                    break;
                }
                case kDouble: {
                    BuildOnlyMinMaxDataFilter<DoubleT, CheckTS>(arg);
                    break;
                }
                case kTinyInt: {
                    BuildProbabilisticAndMinMaxDataFilter<TinyIntT, CheckTS>(arg);
                    break;
                }
                case kSmallInt: {
                    BuildProbabilisticAndMinMaxDataFilter<SmallIntT, CheckTS>(arg);
                    break;
                }
                case kInteger: {
                    BuildProbabilisticAndMinMaxDataFilter<IntegerT, CheckTS>(arg);
                    break;
                }
                case kBigInt: {
                    BuildProbabilisticAndMinMaxDataFilter<BigIntT, CheckTS>(arg);
                    break;
                }
                case kHugeInt: {
                    BuildProbabilisticAndMinMaxDataFilter<HugeIntT, CheckTS>(arg);
                    break;
                }
                case kVarchar: {
                    BuildProbabilisticAndMinMaxDataFilter<VarcharT, CheckTS>(arg);
                    break;
                }
                case kDate: {
                    BuildProbabilisticAndMinMaxDataFilter<DateT, CheckTS>(arg);
                    break;
                }
                case kTime: {
                    BuildProbabilisticAndMinMaxDataFilter<TimeT, CheckTS>(arg);
                    break;
                }
                case kDateTime: {
                    BuildProbabilisticAndMinMaxDataFilter<DateTimeT, CheckTS>(arg);
                    break;
                }
                case kTimestamp: {
                    BuildProbabilisticAndMinMaxDataFilter<TimestampT, CheckTS>(arg);
                    break;
                }
                default: {
                    UnrecoverableError("BUG: BuildFastRoughFilterTask: unsupported data type");
                }
            }
            // step 2.3. check total_row_count
            if (!arg.total_row_count_handler_.IsAtEnd()) {
                LOG_TRACE(fmt::format("BuildFastRoughFilterTask: read row count ({}) < segment_row_count ({}), maybe some rows are deleted",
                                      arg.total_row_count_handler_.total_row_count_read_,
                                      segment_row_count));
            }
        }
    }
    // step 3. finish build FastRoughFilter
    SetSegmentFinishBuildFastRoughFilterTask(segment_entry);
    // step 4. save FastRoughFilter to block / segment json file in future checkpoint
    LOG_TRACE(fmt::format("BuildFastRoughFilterTask: build fast rough filter for segment {}, job end.", segment_entry->segment_id()));
}

} // namespace infinity