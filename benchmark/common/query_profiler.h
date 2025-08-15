//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include "base_profiler.h"
#include <magic_enum/magic_enum.hpp>
#include <vector>

namespace infinity {

enum class QueryPhase : int8_t {
    kParser = 0,
    kLogicalPlan,
    kOptimizer,
    kPhysicalPlan,
    kPipelineBuild,
    kTaskBuild,
    kExecution,
    kInvalid,
};

class OptimizerProfiler {
public:
    void StartRule(const std::string &rule_name);

    void StopRule();

    [[nodiscard]] std::string ToString(size_t intent = 0) const;

private:
    std::vector<BaseProfiler> profilers_;
};

class QueryProfiler {
public:
    void StartPhase(QueryPhase phase);

    void StopPhase(QueryPhase phase);

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] std::string ToString() const;

    static std::string QueryPhaseToString(QueryPhase phase);

private:
    std::vector<BaseProfiler> profilers_{magic_enum::enum_integer(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};
};

} // namespace infinity
