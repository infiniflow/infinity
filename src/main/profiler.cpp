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

#include "magic_enum.hpp"

import std;
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

String BaseProfiler::ElapsedToString() const {
    auto duration = this->ElapsedInternal();
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
    if (active_operator_) {
        Error<ProfilerException>("Attempting to call StartOperator while another operator is active.", __FILE_NAME__, __LINE__);
    }
    active_operator_ = op;
    profiler_.Begin();
}
void TaskProfiler::StopOperator(const InputState *input_state, const OutputState *output_state) {
    if (!enable_) {
        return;
    }
    if (!active_operator_) {
        Error<ProfilerException>("Attempting to call StopOperator while another operator is active.", __FILE_NAME__, __LINE__);
    }
    profiler_.End();
    auto input_block = input_state->input_data_block_;
    auto output_block = output_state->data_block_;
    auto input_rows = input_block ? input_block->row_count() : 0;
    auto input_data_size = input_block ? input_block->GetSizeInBytes() : 0;
    auto output_rows = output_block ? output_block->row_count() : 0;

    OperatorInformation info(active_operator_->GetName(), profiler_.Elapsed(), input_rows, input_data_size, output_rows);

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

    profilers_[phase_idx].set_name(QueryPhaseToString(phase));
    profilers_[phase_idx].Begin();
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
    if (!enable_) {
        return;
    }
    for (const auto &fragment : records_) {
        ss << "Fragment #" << fragment.first << std::endl;
        for (const auto &task : fragment.second) {
            ss << "|- Task #" << task.first << std::endl;
            SizeT times = 0;
            for (const auto &operators : task.second) {
                ss << "  |- Times: " << times << std::endl;
                for (const auto &op : operators.timings_) {
                    ss << "    -> " << op.name_
                       << ": ConsumingTime: " << op.time_
                       << ", InputRows: " << op.input_rows_
                       << ", OutputRows: " << op.output_rows_
                       << ", InputDataSize: " << op.input_data_size_
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
            ss << "--------------------------------" << std::endl;
            ExecuteRender(ss);
        }
    }
    return ss.str();
}

} // namespace infinity
