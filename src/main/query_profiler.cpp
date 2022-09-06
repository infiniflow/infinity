//
// Created by JinHai on 2022/9/5.
//

#include "query_profiler.h"

#include <sstream>

namespace infinity {


void
OptimizerProfiler::StartRule(const std::string &rule_name) {
    profilers_.emplace_back(rule_name);
    profilers_.back().Begin();
}

void
OptimizerProfiler::StopRule() {
    profilers_.back().End();
}

std::string
OptimizerProfiler::ToString() const {
    std::stringstream ss;
    for(auto& profiler: profilers_) {
        ss << profiler.name() << ": " << profiler.ElapsedToString() << std::endl;
    }

    return ss.str();
}

std::string
QueryProfiler::QueryPhaseToString(QueryPhase phase) {
    switch (phase) {
        case QueryPhase::kParser: return "Parser";
        case QueryPhase::kLogicalPlan: return "LogicalPlan";
        case QueryPhase::kOptimizer: return "Optimizer";
        case QueryPhase::kPhysicalPlan: return "PhysicalPlan";
        case QueryPhase::kPipelineBuild: return "PipelineBuild";
        case QueryPhase::kExecution: return "Execution";
    }
}

void
QueryProfiler::StartPhase(QueryPhase phase) {
    size_t phase_idx = magic_enum::enum_integer(phase);
    profilers_[phase_idx].set_name(QueryPhaseToString(phase));
    profilers_[phase_idx].Begin();
}

void
QueryProfiler::StopPhase(QueryPhase phase) {
    profilers_[magic_enum::enum_integer(phase)].End();
}

std::string
QueryProfiler::ToString() const {
    std::stringstream ss;
    constexpr size_t profilers_count = magic_enum::enum_count<QueryPhase>();

    for(size_t idx = 0; idx < profilers_count; ++ idx) {
        const BaseProfiler& profiler = profilers_[idx];
        ss << profiler.name() << ": " << profiler.ElapsedToString() << std::endl;
        if(magic_enum::enum_value<QueryPhase>(idx) == QueryPhase::kOptimizer) {
            ss << optimizer_.ToString() << std::endl;
        }
    }
    return ss.str();
}

}