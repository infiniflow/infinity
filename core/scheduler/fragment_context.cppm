//
// Created by jinhai on 23-9-8.
//

//#pragma once
//
////#include "executor/operator/physical_sink.h"
////#include "executor/operator/physical_source.h"
////#include "executor/physical_operator.h"
////#include "main/query_context.h"
//#include "scheduler/fragment_task.h"
//
//#include "common/types/alias/primitives.h"
//#include "common/types/alias/smart_ptr.h"
//#include "common/types/alias/containers.h"
//
//#include <mutex>
//#include <condition_variable>

module;

import stl;
import fragment_task;
import query_context;
import physical_operator;
import physical_source;
import physical_sink;
import table;
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

    inline SharedPtr<Table> GetResult() {
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
    virtual SharedPtr<Table> GetResultInternal() = 0;

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
    explicit inline SerialMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context) : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table> GetResultInternal() final;

protected:
    Vector<SharedPtr<DataBlock>> task_result_{};
};

export class ParallelMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit inline ParallelMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context)
        : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table> GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

export class ParallelStreamFragmentCtx : public FragmentContext {
public:
    explicit inline ParallelStreamFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context) : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table> GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

} // namespace infinity
