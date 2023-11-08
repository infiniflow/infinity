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

import std;
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

struct OperatorInformation {
    OperatorInformation() = default;

    OperatorInformation(const OperatorInformation& other)
        : name_(other.name_), time_(other.time_), input_rows_(other.input_rows_), input_data_size_(other.input_data_size_), output_rows_(other.output_rows_) {

    }

    OperatorInformation(OperatorInformation&& other)
        : name_(Move(other.name_)), time_(other.time_), input_rows_(other.input_rows_), input_data_size_(other.input_data_size_), output_rows_(other.output_rows_) {
    }

    OperatorInformation(String name, i64 time, u16 input_rows, i32 input_data_size, u16 output_rows)
        : name_(Move(name)), time_(time), input_rows_(input_rows), input_data_size_(input_data_size), output_rows_(output_rows) {
    }

    OperatorInformation& operator=(OperatorInformation&& other) {
        if (this != &other) {
            name_ = Move(other.name_);
            time_ = other.time_;
            input_rows_ = other.input_rows_;
            output_rows_ = other.output_rows_;
            input_data_size_ = other.input_data_size_;
        }
        return *this;
    }

    i64 time_ = 0;
    u16 input_rows_ = 0;
    u16 output_rows_ = 0;
    i32 input_data_size_ = 0;

    String name_;
};

export struct TaskBinding {
    u64 fragment_id_ {};
    i64 task_id_ {};
    Vector<u64> child_fragment_id_ {};
};

export class OptimizerProfiler {
public:
    void StartRule(const String &rule_name);

    void StopRule();

    [[nodiscard]] String ToString(SizeT intent = 0) const;

private:
    Vector<BaseProfiler> profilers_;
};

class PhysicalOperator;
class PlanFragment;
class InputState;
class OutputState;

export class TaskProfiler {
public:
    TaskProfiler(TaskBinding binding, bool enable, SizeT operators_len)
        : enable_(enable), binding_(binding) {

        timings_.reserve(operators_len);
    }

    void StartOperator(const PhysicalOperator *op);

    void StopOperator(const InputState *input_state, const OutputState *output_state);


    TaskBinding binding_;
    Vector<OperatorInformation> timings_{};
private:
    bool enable_ = false;

    BaseProfiler profiler_;
    const PhysicalOperator *active_operator_ = nullptr;
};

export class QueryProfiler {
public:
    QueryProfiler() {};

    QueryProfiler(bool enable) : enable_(enable) {};

    void StartPhase(QueryPhase phase);

    void StopPhase(QueryPhase phase);

    void Flush(TaskProfiler &&profiler);

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] String ToString() const;

    static String QueryPhaseToString(QueryPhase phase);

private:
    bool enable_ = false;

    Mutex flush_lock_{};
    HashMap<u64, HashMap<i64, Vector<TaskProfiler>>> records_{};
    Vector<BaseProfiler> profilers_{EnumInteger(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};

    void ExecuteRender(std::stringstream &ss) const;
};

} // namespace infinity
