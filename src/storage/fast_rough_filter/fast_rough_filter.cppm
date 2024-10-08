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

export module fast_rough_filter;
import stl;
import value;
import default_values;
import probabilistic_data_filter;
import min_max_data_filter;
import logger;
import third_party;
import infinity_exception;
import filter_expression_push_down_helper;

namespace infinity {

class BuildFastRoughFilterTask;
export class FastRoughFilterEvaluator;

// used in block_entry and segment_entry
// sealed segment will have minmax filter
// some columns may have bloom filter
export class FastRoughFilter {
private:
    friend class BuildFastRoughFilterTask;
    friend class FastRoughFilterEvaluator;
    static constexpr std::string_view JsonTagBuildTime = "fast_rough_filter_build_time";

    // in minmax build task, first set build_time_ to be the begin_ts of the task txn
    // if set to valid time, we know one job has started
    mutable std::mutex mutex_check_task_start_;
    TxnTimeStamp build_time_{UNCOMMIT_TS};     // for minmax filter
    std::atomic_flag finished_build_minmax_filter_; // for minmax filter
    UniquePtr<MinMaxDataFilter> min_max_data_filter_;

    UniquePtr<ProbabilisticDataFilter> probabilistic_data_filter_;

public:
    // bloom filter test
    inline bool MayContain(TxnTimeStamp query_ts, ColumnID column_id, const Value &value) const {
        return probabilistic_data_filter_->MayContain(query_ts, column_id, value);
    }

    // minmax filter test
    inline bool MayInRange(ColumnID column_id, const Value &value, FilterCompareType compare_type) const {
        return min_max_data_filter_->MayInRange(column_id, value, compare_type);
    }

    String SerializeToString() const;

    void DeserializeFromString(const String &str);

    void SaveToJsonFile(nlohmann::json &entry_json) const;

    bool LoadFromJsonFile(const nlohmann::json &entry_json);

private:
    inline bool HaveMinMaxFilter() const { return finished_build_minmax_filter_.test(std::memory_order_acquire); }

    // call after check finished_build_minmax_filter_, thus no need to lock
    inline TxnTimeStamp GetMinMaxBuildTime() const { return build_time_; }

    void SetHaveStartedMinMaxFilterBuildTask(TxnTimeStamp begin_ts) {
        std::lock_guard lock(mutex_check_task_start_);
        if (build_time_ != UNCOMMIT_TS) {
            String error_message = "FastRoughFilter::SetHaveStartedBuildTask(): Job already started.";
            UnrecoverableError(error_message);
        }
        build_time_ = begin_ts;
    }

    bool HaveStartedMinMaxFilterBuildTask() const {
        std::lock_guard lock(mutex_check_task_start_);
        return build_time_ != UNCOMMIT_TS;
    }

    void BeginBuildMinMaxFilterTask(u32 column_count) {
        probabilistic_data_filter_ = MakeUnique<ProbabilisticDataFilter>(column_count);
        min_max_data_filter_ = MakeUnique<MinMaxDataFilter>(column_count);
    }

    void FinishBuildMinMaxFilterTask() { finished_build_minmax_filter_.test_and_set(std::memory_order_release); }

    void BuildProbabilisticDataFilter(TxnTimeStamp begin_ts, ColumnID column_id, u64 *data, u32 count) {
        probabilistic_data_filter_->Build(begin_ts, column_id, data, count);
    }

    // used in build_fast_rough_filter_task
    template <typename OriginalValueType, typename MinMaxInnerValT>
    void BuildMinMaxDataFilter(ColumnID column_id, MinMaxInnerValT &&min, MinMaxInnerValT &&max) {
        min_max_data_filter_->Build<OriginalValueType>(column_id, std::forward<MinMaxInnerValT>(min), std::forward<MinMaxInnerValT>(max));
    }
};

export enum class FastRoughFilterEvaluatorTag : u8 {
    kAlwaysTrue,
    kAlwaysFalse,
    kCombineAnd,
    kCombineOr,
    kProbabilisticDataFilter,
    kMinMaxFilter,
};

class FastRoughFilterEvaluator {
    FastRoughFilterEvaluatorTag tag_;

protected:
    explicit FastRoughFilterEvaluator(const FastRoughFilterEvaluatorTag tag) : tag_(tag) {}

public:
    auto Tag() const { return tag_; }

    virtual ~FastRoughFilterEvaluator() = default;

    inline bool Evaluate(TxnTimeStamp query_ts, const FastRoughFilter &filter) const {
        // check filter and query_ts here
        if (!filter.HaveMinMaxFilter()) {
            LOG_TRACE("FastRoughFilterEvaluator: filter not finished build, cannot apply, return true.");
            return true;
        }
        if (query_ts < filter.GetMinMaxBuildTime()) [[unlikely]] {
            LOG_TRACE("FastRoughFilterEvaluator: query timestamp earlier than filter build timestamp, cannot apply, return true.");
            return true;
        }
        return EvaluateInner(query_ts, filter);
    }

    virtual bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const = 0;
};

} // namespace infinity
