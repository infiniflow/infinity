//
// Created by JinHai on 2022/9/5.
//

#include "query_profiler.h"
#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"

#include <iomanip>
#include <sstream>

namespace infinity {

void OptimizerProfiler::StartRule(const String &rule_name) {
    profilers_.emplace_back(rule_name);
    profilers_.back().Begin();
}

void OptimizerProfiler::StopRule() { profilers_.back().End(); }

String OptimizerProfiler::ToString(SizeT intent) const {
    std::stringstream ss;
    String space(intent, ' ');
    for (auto &profiler : profilers_) {
        ss << space << profiler.name() << ": " << profiler.ElapsedToString() << std::endl;
    }

    return ss.str();
}

String QueryProfiler::QueryPhaseToString(QueryPhase phase) {
    switch (phase) {
        case QueryPhase::kParser:
            return "Parser";
        case QueryPhase::kLogicalPlan:
            return "LogicalPlan";
        case QueryPhase::kOptimizer:
            return "Optimizer";
        case QueryPhase::kPhysicalPlan:
            return "PhysicalPlan";
        case QueryPhase::kPipelineBuild:
            return "PipelineBuild";
        case QueryPhase::kExecution:
            return "Execution";
        default:
            GeneralError("Invalid query phase in query profiler");
    }
}

void QueryProfiler::StartPhase(QueryPhase phase) {
    SizeT phase_idx = magic_enum::enum_integer(phase);

    // Validate current query phase.
    if (current_phase_ == QueryPhase::kInvalid) {
        current_phase_ = phase;
    } else {
        GeneralError("Can't start new query phase before current phase(" + QueryPhaseToString(current_phase_) + ") is finished");
    }

    profilers_[phase_idx].set_name(QueryPhaseToString(phase));
    profilers_[phase_idx].Begin();
}

void QueryProfiler::StopPhase(QueryPhase phase) {
    // Validate current query phase.
    if (current_phase_ == QueryPhase::kInvalid) {
        GeneralError("Query phase isn't started, yet");
    }

    current_phase_ = QueryPhase::kInvalid;
    profilers_[magic_enum::enum_integer(phase)].End();
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