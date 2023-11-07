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
import fragment_task;
import query_context;
import physical_operator;
import physical_source;
import physical_sink;
import data_table;
import data_block;

export module fragment_context;

namespace infinity {

class PlanFragment;

// enum class FragmentStatus {
//     kNotStart,
//     k
//     kStart,
//     kFinish,
// };
export enum class FragmentType {
    kInvalid,
    kSerialMaterialize,
    kParallelMaterialize,
    kParallelStream,
};

export class FragmentContext {
public:
    static void
    MakeFragmentContext(QueryContext *query_context, FragmentContext *parent_context, PlanFragment *fragment_ptr, Vector<FragmentTask *> &tasks);

public:
    explicit FragmentContext(PlanFragment *fragment_ptr, QueryContext *query_context);

    virtual ~FragmentContext() = default;

    inline void IncreaseTask() { task_n_.fetch_add(1); }

    inline void FinishTask() {
        u64 unfinished_task = task_n_.fetch_sub(1);
        auto sink_op = GetSinkOperator();

        if (unfinished_task == 1 && sink_op->sink_type() == SinkType::kResult) {
            Complete();
        }
    }

    Vector<PhysicalOperator *> &GetOperators();

    [[nodiscard]] PhysicalSink *GetSinkOperator() const;

    [[nodiscard]] PhysicalSource *GetSourceOperator() const;

    void CreateTasks(i64 parallel_count, i64 operator_count);

    inline Vector<UniquePtr<FragmentTask>> &Tasks() { return tasks_; }

    inline SharedPtr<DataTable> GetResult() {
        UniqueLock<Mutex> lk(locker_);
        cv_.wait(lk, [&] { return completed_; });

        return GetResultInternal();
    }

    inline void Complete() {
        UniqueLock<Mutex> lk(locker_);
        completed_ = true;
        cv_.notify_one();
    }

    inline QueryContext *query_context() { return query_context_; }

    [[nodiscard]] inline FragmentType ContextType() const { return fragment_type_; }

protected:
    virtual SharedPtr<DataTable> GetResultInternal() = 0;

protected:
    au64 task_n_{0};

    Mutex locker_{};
    CondVar cv_{};

    PlanFragment *fragment_ptr_{};
    //    HashMap<u64, UniquePtr<FragmentTask>> tasks_;
    Vector<UniquePtr<FragmentTask>> tasks_{};

    bool finish_building_{false};
    bool completed_{false};
    i64 finished_task_count_{};
    Vector<SharedPtr<DataBlock>> data_array_{};

    FragmentType fragment_type_{FragmentType::kInvalid};
    QueryContext *query_context_{};
};

export class SerialMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit inline SerialMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context)
        : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<DataTable> GetResultInternal() final;

protected:
    Vector<SharedPtr<DataBlock>> task_result_{};
};

export class ParallelMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit inline ParallelMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context)
        : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<DataTable> GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

export class ParallelStreamFragmentCtx : public FragmentContext {
public:
    explicit inline ParallelStreamFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context)
        : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<DataTable> GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

} // namespace infinity
