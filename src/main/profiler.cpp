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
    // Validate current query phase.
    if (current_phase_ == QueryPhase::kInvalid) {
        Error<ExecutorException>("Query phase isn't started, yet");
    }

    current_phase_ = QueryPhase::kInvalid;
    profilers_[EnumInteger(phase)].End();
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
    }
    return ss.str();
}

} // namespace infinity
