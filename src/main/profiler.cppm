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

import stl;
import third_party;

export module profiler;

namespace infinity {

export class BaseProfiler {
public:
    BaseProfiler() = default;

    explicit BaseProfiler(String name) : name_(Move(name)) {}

    // Start the profiler
    void Begin();

    // End the profiler
    void End();

    [[nodiscard]] String ElapsedToString() const;

    // Return the elapsed time from begin, if the profiler is ended, it will return total elapsed time.
    [[nodiscard]] inline i64 Elapsed() const { return ElapsedInternal().count(); }

    [[nodiscard]] const String &name() const { return name_; }
    void set_name(const String &name) { name_ = name; }

private:
    [[nodiscard]] static inline TimePoint<Clock> Now() {
        return Clock::now();
    }

    [[nodiscard]] NanoSeconds ElapsedInternal() const;

    TimePoint<Clock> begin_ts_{};
    TimePoint<Clock> end_ts_{};

    bool finished_ = false;
    String name_{};
};

export enum class QueryPhase : i8 {
    kParser = 0,
    kLogicalPlan,
    kOptimizer,
    kPhysicalPlan,
    kPipelineBuild,
    kExecution,
    kInvalid,
};

export class OptimizerProfiler {
public:
    void StartRule(const String &rule_name);

    void StopRule();

    [[nodiscard]] String ToString(SizeT intent = 0) const;

private:
    Vector<BaseProfiler> profilers_;
};

export class QueryProfiler {
public:
    void StartPhase(QueryPhase phase);

    void StopPhase(QueryPhase phase);

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] String ToString() const;

    static String QueryPhaseToString(QueryPhase phase);

private:
    Vector<BaseProfiler> profilers_{EnumInteger(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};
};

} // namespace infinity
