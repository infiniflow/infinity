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

#include <sstream>
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

    static String ElapsedToString(NanoSeconds duration);

    // Return the elapsed time from begin, if the profiler is ended, it will return total elapsed time.
    [[nodiscard]] inline i64 Elapsed() const {
        if(name_.empty()) {
            return 0;
        }
        return ElapsedInternal().count();
    }

    [[nodiscard]] inline i64 GetBegin() const { return begin_ts_.time_since_epoch().count(); }

    [[nodiscard]] inline i64 GetEnd() const { return end_ts_.time_since_epoch().count(); }

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
    kTaskBuild,
    kExecution,
    kCommit,
    kRollback,
    kInvalid,
};

struct OperatorInformation {
    OperatorInformation() = default;

    OperatorInformation(const OperatorInformation& other)
        : name_(other.name_), start_(other.start_), end_(other.end_), elapsed_(other.elapsed_), input_rows_(other.input_rows_),
          output_data_size_(other.output_data_size_), output_rows_(other.output_rows_) {

    }

    OperatorInformation(OperatorInformation&& other)
        : name_(Move(other.name_)), start_(other.start_), end_(other.end_), elapsed_(other.elapsed_), input_rows_(other.input_rows_),
          output_data_size_(other.output_data_size_), output_rows_(other.output_rows_) {
    }

    OperatorInformation(String name, i64 start, i64 end, i64 elapsed, u16 input_rows, i32 output_data_size, u16 output_rows)
        : name_(Move(name)), start_(start), end_(end), elapsed_(elapsed), input_rows_(input_rows), output_data_size_(output_data_size), output_rows_(output_rows) {
    }

    OperatorInformation& operator=(OperatorInformation&& other) {
        if (this != &other) {
            name_ = Move(other.name_);
            start_ = Move(other.start_);
            end_ = Move(other.end_);
            elapsed_ = other.elapsed_;
            input_rows_ = other.input_rows_;
            output_rows_ = other.output_rows_;
            output_data_size_ = other.output_data_size_;
        }
        return *this;
    }

    String name_ {};

    i64 start_ {};
    i64 end_ {};
    i64 elapsed_{};
    u16 input_rows_ {};
    i32 output_data_size_ {};
    u16 output_rows_ {};
};

export struct TaskBinding {
    u64 fragment_id_ {};
    i64 task_id_ {};
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
class OperatorState;

export class TaskProfiler {
public:
    TaskProfiler(TaskBinding binding, bool enable, SizeT operators_len)
        : binding_(binding), enable_(enable){
        if(!enable_) {
            return;
        }
        timings_.reserve(operators_len);
    }

    void Begin() {
        if(enable_) {
            task_profiler_.Begin();
        }
    }

    void End() {
        if(enable_) {
            task_profiler_.End();
        }
    }

    void StartOperator(const PhysicalOperator *op);

    void StopOperator(const OperatorState *output_state);


    TaskBinding binding_;
    Vector<OperatorInformation> timings_{};
    BaseProfiler task_profiler_;
private:
    bool enable_ {};

    BaseProfiler profiler_;
    const PhysicalOperator *active_operator_ = nullptr;
};

export class QueryProfiler {
public:
    QueryProfiler() = default;

    explicit QueryProfiler(bool enable) : enable_(enable) {};

    void StartPhase(QueryPhase phase);

    void StopPhase(QueryPhase phase);

    void Stop();

    void Flush(TaskProfiler &&profiler);

    i64 ElapsedAt(SizeT index) {
        return profilers_[index].Elapsed();
    }

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] String ToString() const;

    static String QueryPhaseToString(QueryPhase phase);

    static Json Serialize(const QueryProfiler *profiler);

private:
    bool enable_ {};

    Mutex flush_lock_{};
    HashMap<u64, HashMap<i64, Vector<TaskProfiler>>> records_{};
    Vector<BaseProfiler> profilers_{EnumInteger(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};

    void ExecuteRender(std::stringstream &ss) const;
};

} // namespace infinity
