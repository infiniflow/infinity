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

export module build_fast_rough_filter_task;

import stl;
import segment_entry;
import buffer_manager;
import infinity_exception;
import filter_value_type_classification;
import logger;

namespace infinity {

class SegmentMeta;
class FastRoughFilter;

export struct BuildingSegmentFastFilters {
    SegmentMeta *segment_meta_ = nullptr;
    SharedPtr<FastRoughFilter> segment_filter_;
    HashMap<BlockID, SharedPtr<FastRoughFilter>> block_filters_;

    static UniquePtr<BuildingSegmentFastFilters> Make(SegmentMeta *segment_meta);

    ~BuildingSegmentFastFilters();

    void ApplyToAllFastRoughFilterInSegment(std::invocable<FastRoughFilter *> auto func);

    void CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask();

    void SetSegmentBeginBuildMinMaxFilterTask(u32 column_count);

    void SetSegmentFinishBuildMinMaxFilterTask();

    void SetFilter();
};

struct TotalRowCount {
    u32 total_row_count_read_{};
    u32 total_row_count_in_segment_{};

    explicit TotalRowCount(u32 total_row_count_in_segment) : total_row_count_in_segment_(total_row_count_in_segment) {}
    bool IsAtEnd() const { return total_row_count_read_ == total_row_count_in_segment_; }
};

struct BuildFastRoughFilterArg {
    SegmentEntry *segment_entry_{};
    ColumnID column_id_{};
    UniquePtr<u64[]> &distinct_keys_;
    UniquePtr<u64[]> &distinct_keys_backup_;
    BufferManager *buffer_manager_{};
    TxnTimeStamp begin_ts_{};
    u32 distinct_count_{};
    u32 total_row_count_in_segment_{};
    TotalRowCount total_row_count_handler_{total_row_count_in_segment_};

    BuildFastRoughFilterArg(SegmentEntry *segment_entry,
                            ColumnID column_id,
                            UniquePtr<u64[]> &distinct_keys,
                            UniquePtr<u64[]> &distinct_keys_backup,
                            BufferManager *buffer_manager,
                            TxnTimeStamp begin_ts,
                            u32 total_row_count_in_segment)
        : segment_entry_(segment_entry), column_id_(column_id), distinct_keys_(distinct_keys), distinct_keys_backup_(distinct_keys_backup),
          buffer_manager_(buffer_manager), begin_ts_(begin_ts), total_row_count_in_segment_(total_row_count_in_segment) {}
};

struct NewBuildFastRoughFilterArg {
    SegmentMeta *segment_meta_{};
    ColumnID column_id_{};
    UniquePtr<u64[]> &distinct_keys_;
    UniquePtr<u64[]> &distinct_keys_backup_;
    u32 distinct_count_{};
    BuildingSegmentFastFilters *segment_filters_{};

    NewBuildFastRoughFilterArg(SegmentMeta *segment_meta,
                               ColumnID column_id,
                               UniquePtr<u64[]> &distinct_keys,
                               UniquePtr<u64[]> &distinct_keys_backup,
                               BuildingSegmentFastFilters *segment_filters);

    ~NewBuildFastRoughFilterArg();
};

export class BuildFastRoughFilterTask {
public:
    static void ExecuteOnNewSealedSegment(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts);

    static void ExecuteOnNewSealedSegment(SegmentMeta *segment_meta);

    static void ExecuteUpdateSegmentBloomFilter(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts);

private:
    static void CheckAndSetSegmentHaveStartedBuildMinMaxFilterTask(SegmentEntry *segment, TxnTimeStamp begin_ts);

    static void SetSegmentBeginBuildMinMaxFilterTask(SegmentEntry *segment, u32 column_count);

    static void SetSegmentFinishBuildMinMaxFilterTask(SegmentEntry *segment);

private:
    template <bool CheckTS>
    static void ExecuteInner(SegmentEntry *segment_entry, BufferManager *buffer_manager, TxnTimeStamp begin_ts);

    static void ExecuteInner(SegmentMeta *sement_meta, BuildingSegmentFastFilters *segment_filters);

    template <CanBuildBloomFilter ValueType, bool CheckTS>
    static void BuildOnlyBloomFilter(BuildFastRoughFilterArg &arg);

    template <CanBuildBloomFilter ValueType>
    static void BuildOnlyBloomFilter(NewBuildFastRoughFilterArg &arg);

    template <CanBuildMinMaxFilter ValueType, bool CheckTS>
    static void BuildOnlyMinMaxFilter(BuildFastRoughFilterArg &arg);

    template <CanBuildMinMaxFilter ValueType>
    static void BuildOnlyMinMaxFilter(NewBuildFastRoughFilterArg &arg);

    template <CanBuildMinMaxFilterAndBloomFilter ValueType, bool CheckTS>
    static void BuildMinMaxAndBloomFilter(BuildFastRoughFilterArg &arg);

    template <CanBuildMinMaxFilterAndBloomFilter ValueType>
    static void BuildMinMaxAndBloomFilter(NewBuildFastRoughFilterArg &arg);

    template <typename ValueType, bool CheckTS>
    static void BuildFilter(BuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter);

    template <CanBuildMinMaxFilterAndBloomFilter ValueType, bool CheckTS>
    static void BuildFilter(BuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_min_max_filter and build_bloom_filter) {
            BuildMinMaxAndBloomFilter<ValueType, CheckTS>(arg);
        } else if (build_min_max_filter) {
            // TODO: now only use this branch
            BuildOnlyMinMaxFilter<ValueType, CheckTS>(arg);
        } else if (build_bloom_filter) {
            BuildOnlyBloomFilter<ValueType, CheckTS>(arg);
        }
    }

    template <CanOnlyBuildMinMaxFilter ValueType, bool CheckTS>
    static void BuildFilter(BuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_bloom_filter) [[unlikely]] {
            String error_message = "BuildFilter: build_bloom_filter is true, but ValueType can only build min-max filter";
            UnrecoverableError(error_message);
        }
        if (build_min_max_filter) {
            // TODO: now only use this branch
            BuildOnlyMinMaxFilter<ValueType, CheckTS>(arg);
        }
    }

    template <CanOnlyBuildBloomFilter ValueType, bool CheckTS>
    static void BuildFilter(BuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_min_max_filter) [[unlikely]] {
            String error_message = "BuildFilter: build_min_max_filter is true, but ValueType can only build bloom filter";
            UnrecoverableError(error_message);
        }
        if (build_bloom_filter) {
            BuildOnlyBloomFilter<ValueType, CheckTS>(arg);
        }
    }

    template <typename ValueType>
    static void BuildFilter(NewBuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter);

    template <CanBuildMinMaxFilterAndBloomFilter ValueType>
    static void BuildFilter(NewBuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_min_max_filter and build_bloom_filter) {
            BuildMinMaxAndBloomFilter<ValueType>(arg);
        } else if (build_min_max_filter) {
            // TODO: now only use this branch
            BuildOnlyMinMaxFilter<ValueType>(arg);
        } else if (build_bloom_filter) {
            BuildOnlyBloomFilter<ValueType>(arg);
        }
    }

    template <CanOnlyBuildMinMaxFilter ValueType>
    static void BuildFilter(NewBuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_bloom_filter) [[unlikely]] {
            String error_message = "BuildFilter: build_bloom_filter is true, but ValueType can only build min-max filter";
            UnrecoverableError(error_message);
        }
        if (build_min_max_filter) {
            // TODO: now only use this branch
            BuildOnlyMinMaxFilter<ValueType>(arg);
        }
    }

    template <CanOnlyBuildBloomFilter ValueType>
    static void BuildFilter(NewBuildFastRoughFilterArg &arg, bool build_min_max_filter, bool build_bloom_filter) {
        if (build_min_max_filter) [[unlikely]] {
            String error_message = "BuildFilter: build_min_max_filter is true, but ValueType can only build bloom filter";
            UnrecoverableError(error_message);
        }
        if (build_bloom_filter) {
            BuildOnlyBloomFilter<ValueType>(arg);
        }
    }
};

} // namespace infinity