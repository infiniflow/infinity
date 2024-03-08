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
import local_file_system;
import infinity_exception;
import filter_expression_push_down_helper;

namespace infinity {

class BuildFastRoughFilterTask;
export class FastRoughFilterEvaluator;

// used in block_entry and segment_entry
// status: empty -> (when segment change from Unsealed to Sealing) set is_in_bg_task_queue_ ->
//         (in Sealing task) set build_time_ -> set have_started_bg_task_ -> build -> set finished_build_filter_
// query: 1. check finished_build_filter_ 2. check query_ts 3. recursive check inner filters
export class FastRoughFilter {
private:
    friend class BuildFastRoughFilterTask;
    friend class FastRoughFilterEvaluator;
    static constexpr std::string_view JsonTagBuildTime = "fast_rough_filter_build_time";

    // in build task, first set build_time_ to be the begin_ts of the task txn
    // if set to valid time, we know one job has started
    mutable std::mutex mutex_check_task_start_;
    TxnTimeStamp build_time_{UNCOMMIT_TS};

    // in build task, after finish build probabilistic_data_filter_ and min_max_data_filter_, set finished_build_filter_ to be true
    atomic_flag finished_build_filter_; // will be properly initialized since C++20
    UniquePtr<ProbabilisticDataFilter> probabilistic_data_filter_;
    UniquePtr<MinMaxDataFilter> min_max_data_filter_;

public:
    inline bool MayContain(ColumnID column_id, const Value &value) const { return probabilistic_data_filter_->MayContain(column_id, value); }

    inline bool MayInRange(ColumnID column_id, const Value &value, FilterCompareType compare_type) const {
        return min_max_data_filter_->MayInRange(column_id, value, compare_type);
    }

    inline bool HaveFilter() const { return finished_build_filter_.test(std::memory_order_acquire); }

    String SerializeToString() const;

    void DeserializeFromString(const String &str);

    void SaveToJsonFile(nlohmann::json &entry_json) const;

    bool LoadFromJsonFile(const nlohmann::json &entry_json);

private:
    // call after check finished_build_filter_, thus no need to lock
    inline TxnTimeStamp GetBuildTime() const { return build_time_; }

    void SetHaveStartedBuildTask(TxnTimeStamp begin_ts);

    bool HaveStartedBuildTask() const;

    void BeginBuildFastRoughFilterTask(u32 column_count) {
        probabilistic_data_filter_ = MakeUnique<ProbabilisticDataFilter>(column_count);
        min_max_data_filter_ = MakeUnique<MinMaxDataFilter>(column_count);
    }

    void FinishBuildFastRoughFilterTask() { finished_build_filter_.test_and_set(std::memory_order_release); }

    void BuildProbabilisticDataFilter(ColumnID column_id, u64 *data, u32 count) { probabilistic_data_filter_->Build(column_id, data, count); }

    // used in build_fast_rough_filter_task
    template <typename OriginalValueType, typename MinMaxInnerValT>
    void BuildMinMaxDataFilter(ColumnID column_id, MinMaxInnerValT &&min, MinMaxInnerValT &&max) {
        min_max_data_filter_->Build<OriginalValueType>(column_id, std::forward<MinMaxInnerValT>(min), std::forward<MinMaxInnerValT>(max));
    }
};

class FastRoughFilterEvaluator {
public:
    virtual ~FastRoughFilterEvaluator() = default;

    inline bool Evaluate(TxnTimeStamp query_ts, const FastRoughFilter &filter) const {
        // check filter and query_ts here
        if (!filter.HaveFilter()) {
            LOG_TRACE("FastRoughFilterEvaluator: filter not finished build, cannot apply, return true.");
            return true;
        }
        if (query_ts < filter.GetBuildTime()) [[unlikely]] {
            LOG_TRACE("FastRoughFilterEvaluator: query timestamp earlier than filter build timestamp, cannot apply, return true.");
            return true;
        }
        return EvaluateInner(filter);
    }

    virtual bool EvaluateInner(const FastRoughFilter &filter) const = 0;
};

} // namespace infinity