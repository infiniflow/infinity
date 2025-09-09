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

export module infinity_core:profiler;

import :infinity_type;

import std.compat;
import third_party;

namespace infinity {

export class BaseProfiler {
public:
    BaseProfiler() = default;

    explicit BaseProfiler(std::string name) : name_(std::move(name)) {}

    // Start the profiler
    void Begin();

    // End the profiler
    void End();

    std::string BeginTime();

    std::string EndTime();

    [[nodiscard]] std::string ElapsedToString(i64 scale = 1) const;

    static std::string ElapsedToString(std::chrono::nanoseconds duration, i64 scale = 1);

    // Return the elapsed time from begin, if the profiler is ended, it will return total elapsed time.
    [[nodiscard]] inline i64 Elapsed() const { return ElapsedInternal().count(); }

    [[nodiscard]] inline i64 GetBegin() const { return begin_ts_.time_since_epoch().count(); }

    [[nodiscard]] inline i64 GetEnd() const { return end_ts_.time_since_epoch().count(); }

    [[nodiscard]] const std::string &name() const { return name_; }
    void set_name(const std::string &name) { name_ = name; }

private:
    [[nodiscard]] static inline auto Now() { return std::chrono::high_resolution_clock::now(); }

    [[nodiscard]] std::chrono::nanoseconds ElapsedInternal() const;

    std::chrono::time_point<std::chrono::system_clock> begin_ts_{};
    std::chrono::time_point<std::chrono::system_clock> end_ts_{};
    // std::chrono::system_clock<std::chrono::high_resolution_clock, std::chrono::nanoseconds> begin_ts_{};
    // std::chrono::system_clock<std::chrono::high_resolution_clock, std::chrono::nanoseconds> end_ts_{};
    // TimePoint<std::chrono::high_resolution_clock> begin_ts_{};
    // TimePoint<std::chrono::high_resolution_clock> end_ts_{};

    bool finished_ = false;
    std::string name_{};
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

    OperatorInformation(const OperatorInformation &other)
        : name_(other.name_), start_(other.start_), end_(other.end_), elapsed_(other.elapsed_), input_rows_(other.input_rows_),
          output_data_size_(other.output_data_size_), output_rows_(other.output_rows_) {}

    OperatorInformation(OperatorInformation &&other)
        : name_(std::move(other.name_)), start_(other.start_), end_(other.end_), elapsed_(other.elapsed_), input_rows_(other.input_rows_),
          output_data_size_(other.output_data_size_), output_rows_(other.output_rows_) {}

    OperatorInformation(std::string name, i64 start, i64 end, i64 elapsed, u16 input_rows, i32 output_data_size, u16 output_rows)
        : name_(std::move(name)), start_(start), end_(end), elapsed_(elapsed), input_rows_(input_rows), output_data_size_(output_data_size),
          output_rows_(output_rows) {}

    OperatorInformation &operator=(OperatorInformation &&other) {
        if (this != &other) {
            name_ = std::move(other.name_);
            start_ = std::move(other.start_);
            end_ = std::move(other.end_);
            elapsed_ = other.elapsed_;
            input_rows_ = other.input_rows_;
            output_rows_ = other.output_rows_;
            output_data_size_ = other.output_data_size_;
        }
        return *this;
    }

    std::string name_{};

    i64 start_{};
    i64 end_{};
    i64 elapsed_{};
    u16 input_rows_{};
    i32 output_data_size_{};
    u16 output_rows_{};
};

export struct TaskBinding {
    u64 fragment_id_{};
    i64 task_id_{};
};

export class OptimizerProfiler {
public:
    void StartRule(const std::string &rule_name);

    void StopRule();

    [[nodiscard]] std::string ToString(size_t intent = 0) const;

private:
    std::vector<BaseProfiler> profilers_;
};

class PhysicalOperator;
export class PlanFragment;
class OperatorState;

export class TaskProfiler {
public:
    TaskProfiler(TaskBinding binding, bool enable, size_t operators_len) : binding_(binding), enable_(enable) {
        if (!enable_) {
            return;
        }
        timings_.reserve(operators_len);
    }

    void Begin() {
        if (enable_) {
            task_profiler_.Begin();
        }
    }

    void End() {
        if (enable_) {
            task_profiler_.End();
        }
    }

    bool Enable() const { return enable_; }

    void StartOperator(const PhysicalOperator *op);

    void StopOperator(const OperatorState *output_state);

    TaskBinding binding_;
    std::vector<OperatorInformation> timings_{};
    BaseProfiler task_profiler_;

private:
    bool enable_{};

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

    i64 ElapsedAt(size_t index) { return profilers_[index].Elapsed(); }

    OptimizerProfiler &optimizer() { return optimizer_; }

    [[nodiscard]] std::string ToString() const;

    static std::string QueryPhaseToString(QueryPhase phase);

    static nlohmann::json Serialize(const QueryProfiler *profiler);

    std::vector<TaskProfiler> &GetTaskProfile(u64 fragment_id, i64 task_id);

private:
    bool enable_{};

    std::mutex flush_lock_{};
    std::unordered_map<u64, std::unordered_map<i64, std::vector<TaskProfiler>>> records_{};
    std::vector<BaseProfiler> profilers_{static_cast<magic_enum::underlying_type_t<QueryPhase>>(QueryPhase::kInvalid)};
    OptimizerProfiler optimizer_;
    QueryPhase current_phase_{QueryPhase::kInvalid};

    void ExecuteRender(std::stringstream &ss) const;
};

export class ProfileHistory {
private:
    mutable std::mutex lock_{};
    std::deque<std::shared_ptr<QueryProfiler>> deque_{};
    size_t max_size_{};

public:
    explicit ProfileHistory(size_t size) {
        std::unique_lock<std::mutex> lk(lock_);
        max_size_ = size;
    }

    size_t HistoryCapacity() const {
        std::unique_lock<std::mutex> lk(lock_);
        return max_size_;
    }

    void Resize(size_t new_size);

    void Enqueue(std::shared_ptr<QueryProfiler> &&profiler) {
        std::unique_lock<std::mutex> lk(lock_);
        if (deque_.size() >= max_size_) {
            deque_.pop_back();
        }

        deque_.emplace_front(profiler);
    }

    QueryProfiler *GetElement(size_t index);

    std::vector<std::shared_ptr<QueryProfiler>> GetElements();
};

} // namespace infinity
