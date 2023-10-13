//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include "magic_enum.hpp"
#include "main/profiler/base_profiler.h"
#include "common/types/alias/strings.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/containers.h"

namespace infinity {

enum class QueryPhase : i8 {
    kParser = 0,
    kLogicalPlan,
    kOptimizer,
    kPhysicalPlan,
    kPipelineBuild,
    kExecution,
    kInvalid,
};

class OptimizerProfiler {
public:
    void StartRule(const String &rule_name);

    void StopRule();

    [[nodiscard]] String ToString(size_t intent = 0) const;

private:
    Vector<BaseProfiler> profilers_;
};

class QueryProfiler {
public:
    void StartPhase(QueryPhase phase);

    void StopPhase(QueryPhase phase);

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] String ToString() const;

    static String QueryPhaseToString(QueryPhase phase);

private:
    Vector<BaseProfiler> profilers_{magic_enum::enum_integer(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};
};

} // namespace infinity
