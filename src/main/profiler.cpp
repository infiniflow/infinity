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

#include <iostream>
#include "magic_enum.hpp"

import stl;
import third_party;
import physical_operator;
import plan_fragment;
import operator_state;

import infinity_exception;

module profiler;

namespace infinity {

void BaseProfiler::Begin() {
    finished_ = false;
    begin_ts_ = Now();
}

void BaseProfiler::End() {
    if (finished_)
        return;
    end_ts_ = Now();
    finished_ = true;
}

NanoSeconds BaseProfiler::ElapsedInternal() const {
    auto now = finished_ ? end_ts_ : Now();
    return ElapsedFromStart(now, begin_ts_);
}

String BaseProfiler::ElapsedToString(NanoSeconds duration) {
    String result;
    if (duration.count() <= 1000) {
        result.append(Format("{}ns", duration.count()));
    } else if (duration.count() <= 1000 * 1000) {
        result.append(Format("{}us", ChronoCast<MicroSeconds>(duration).count()));
    } else if (duration.count() <= 1000 * 1000 * 1000) {
        result.append(Format("{}ms", ChronoCast<MilliSeconds>(duration).count()));
    } else {
        result.append(Format("{}s", ChronoCast<Seconds>(duration).count()));
    }
    return result;
}

String BaseProfiler::ElapsedToString() const {
    return ElapsedToString(this->ElapsedInternal());
}

void OptimizerProfiler::StartRule(const String &rule_name) {
    profilers_.emplace_back(rule_name);
    profilers_.back().Begin();
}

void OptimizerProfiler::StopRule() { profilers_.back().End(); }

String OptimizerProfiler::ToString(SizeT intent) const {
    String result;
    String space(intent, ' ');

    SizeT profiler_count = profilers_.size();
    for (SizeT idx = 0; idx < profiler_count; ++idx) {
        const auto &profiler = profilers_[idx];
        result.append(Format("{}{}: {}", space, profiler.name(), profiler.ElapsedToString()));
    }

    return result;
}

void TaskProfiler::StartOperator(const PhysicalOperator *op) {
    if (!enable_) {
        return;
    }
    if (active_operator_ != nullptr) {
        Error<ProfilerException>("Attempting to call StartOperator while another operator is active.", __FILE_NAME__, __LINE__);
    }
    active_operator_ = op;
    profiler_.Begin();
}
void TaskProfiler::StopOperator(const OperatorState *operator_state) {
    if (!enable_) {
        return;
    }
    if (active_operator_ == nullptr) {
        Error<ProfilerException>("Attempting to call StopOperator while another operator is active.", __FILE_NAME__, __LINE__);
    }
    profiler_.End();

    auto output_block = operator_state->data_block_;
    auto input_rows = operator_state->prev_op_state_ ? operator_state->prev_op_state_->data_block_->row_count() : 0;
    auto output_data_size = output_block && output_block->Finalized() ? output_block->GetSizeInBytes() : 0;
    auto output_rows = output_block ? output_block->row_count() : 0;

    OperatorInformation info(active_operator_->GetName(), profiler_.GetBegin(), profiler_.GetEnd(), profiler_.Elapsed(), input_rows, output_data_size, output_rows);

    timings_.push_back(Move(info));
    active_operator_ = nullptr;
}

String QueryProfiler::QueryPhaseToString(QueryPhase phase) {
    switch (phase) {
        case QueryPhase::kParser: {
            return "Parser";
        }
        case QueryPhase::kLogicalPlan: {
            return "LogicalPlan";
        }
        case QueryPhase::kOptimizer: {
            return "Optimizer";
        }
        case QueryPhase::kPhysicalPlan: {
            return "PhysicalPlan";
        }
        case QueryPhase::kPipelineBuild: {
            return "PipelineBuild";
        }
        case QueryPhase::kExecution: {
            return "Execution";
        }
        default: {
            Error<ExecutorException>("Invalid query phase in query profiler");
        }
    }
    return {};
}

void QueryProfiler::StartPhase(QueryPhase phase) {
    if (!enable_) {
        return;
    }
    SizeT phase_idx = EnumInteger(phase);

    // Validate current query phase.
    if (current_phase_ == QueryPhase::kInvalid) {
        current_phase_ = phase;
    } else {
        Error<ExecutorException>(Format("Can't start new query phase before current phase({}) is finished", QueryPhaseToString(current_phase_)));
    }

    BaseProfiler& phase_profiler = profilers_[phase_idx];
    phase_profiler.set_name(QueryPhaseToString(phase));
    phase_profiler.Begin();
}

void QueryProfiler::StopPhase(QueryPhase phase) {
    if (!enable_) {
        return;
    }

    // Validate current query phase.
    if (current_phase_ == QueryPhase::kInvalid) {
        Error<ExecutorException>("Query phase isn't started, yet");
    }

    current_phase_ = QueryPhase::kInvalid;
    profilers_[EnumInteger(phase)].End();
}

void QueryProfiler::Flush(TaskProfiler &&profiler) {
    if (!enable_) {
        return;
    }

    UniqueLock<Mutex> lk(flush_lock_);
    records_[profiler.binding_.fragment_id_][profiler.binding_.task_id_].push_back(profiler);
}

void QueryProfiler::ExecuteRender(std::stringstream &ss) const {
    for (const auto &fragment : records_) {
        ss << "Fragment #" << fragment.first << std::endl;
        for (const auto &task : fragment.second) {
            ss << "|- Task #" << task.first << std::endl;
            SizeT times = 0;
            for (const auto &operators : task.second) {
                ss << "  |- Times: " << times << std::endl;
                for (const auto &op : operators.timings_) {
                    ss << "    -> " << op.name_
                       << ": BeginTime: " << op.start_
                       << ": EndTime: " << op.end_
                       << ": ElapsedTime: " << op.elapsed_
                       << ", InputRows: " << op.input_rows_
                       << ", OutputRows: " << op.output_rows_
                       << ", OutputDataSize: " << op.output_data_size_
                       << std::endl;
                }
                times ++;
            }
        }
    }
}

String QueryProfiler::ToString() const {
    std::stringstream ss;
    constexpr SizeT profilers_count = magic_enum::enum_integer(QueryPhase::kInvalid);

    double cost_sum = 0;
    for (SizeT idx = 0; idx < profilers_count; ++idx) {
        const BaseProfiler &profiler = profilers_[idx];
        cost_sum += static_cast<double>(profiler.Elapsed());
    }

    ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ss.setf(std::ios_base::showpoint);
    ss.precision(2);
    for (SizeT idx = 0; idx < profilers_count; ++idx) {
        const BaseProfiler &profiler = profilers_[idx];
        ss << profiler.name() << ": " << profiler.ElapsedToString() << "(" << static_cast<double>(profiler.Elapsed() * 100) / cost_sum << "%)"
           << std::endl;
        if (magic_enum::enum_value<QueryPhase>(idx) == QueryPhase::kOptimizer) {
            ss << optimizer_.ToString(4) << std::endl;
        }
        if (magic_enum::enum_value<QueryPhase>(idx) == QueryPhase::kExecution) {
            ExecuteRender(ss);
        }
    }
    return ss.str();
}

Json QueryProfiler::Serialize(const QueryProfiler *profiler) {
    Json json;

    i64 start = std::numeric_limits<i64>::max();
    i64 end = 0;
    for (const auto &fragment : profiler->records_) {
        Json json_fragments;
        json_fragments["fragment_id"] = fragment.first;

        i64 fragment_start = std::numeric_limits<i64>::max();
        i64 fragment_end = 0;
        for (const auto &task : fragment.second) {
            Json json_tasks;
            SizeT times = 0;
            json_tasks["task_id"] = task.first;

            i64 task_start = std::numeric_limits<i64>::max();
            i64 task_end = 0;
            for (const auto &operators : task.second) {
                task_start = Min(task_start, operators.task_profiler_.GetBegin());
                task_end = Max(task_end, operators.task_profiler_.GetEnd());

                Json json_operators;
                json_operators["times"] = times;
                for (const auto &op : operators.timings_) {
                    Json json_info;
                    json_info["name"] = op.name_;
                    json_info["start"] = op.start_;
                    json_info["end"] = op.end_;
                    json_info["elapsed"] = op.elapsed_;
                    json_info["input_rows"] = op.input_rows_;
                    json_info["output_rows"] = op.output_rows_;
                    json_info["output_data_size"] = op.output_data_size_;
                    json_operators["infos"].push_back(json_info);
                }
                times ++;
                json_tasks["operators"].push_back(json_operators);
            }
            json_tasks["task_start"] = task_start;
            json_tasks["task_end"] = task_end;
            json_tasks["task_total"] = task_end - task_start;

            fragment_start = Min(fragment_start, task_start);
            fragment_end = Max(fragment_end, task_end);

            json_fragments["tasks"].push_back(json_tasks);
        }
        i64 fragment_total = fragment_end - fragment_start;

        json_fragments["fragment_start"] = fragment_start;
        json_fragments["fragment_end"] = fragment_end;
        json_fragments["fragment_total"] = fragment_total;

        start = Min(start, fragment_start);
        end = Max(end, fragment_end);

        json["fragments"].push_back(json_fragments);
    }
    json["total"] = end - start;
    json["time_unit"] = "ns";

    return json;
}

} // namespace infinity
