//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/query_context.h"
#include "scheduler/fragment_task.h"
#include "executor/physical_operator.h"

namespace infinity {

class PlanFragment;


//enum class FragmentStatus {
//    kNotStart,
//    k
//    kStart,
//    kFinish,
//};
enum class FragmentType {
    kGlobalMaterialized,
    kLocalMaterialized,
    kStreamFragment
};

class FragmentContext {
public:
    static UniquePtr<FragmentContext>
    MakeFragmentContext(QueryContext* query_context, PlanFragment* fragment_ptr);

public:
    explicit
    FragmentContext(FragmentType fragment_type, PlanFragment* fragment_ptr, QueryContext* query_context):
            fragment_type_(fragment_type), fragment_ptr_(fragment_ptr), query_context_(query_context) {};

    virtual
    ~FragmentContext() = default;

    inline void
    IncreaseTask() {
//        std::unique_lock<RWMutex> w_locker(rw_locker_);
    }

    inline void
    FinishTask() {
//        std::unique_lock<RWMutex> w_locker(rw_locker_);
    }

    Vector<PhysicalOperator*>&
    GetOperators();

    inline void
    AddTask(UniquePtr<FragmentTask> task,
            UniquePtr<InputState> input_state,
            UniquePtr<OutputState> output_state);

    inline Vector<UniquePtr<FragmentTask>>&
    Tasks() {
        return tasks_;
    }

    virtual void
    InitializeOutput(FragmentTask* fragment_task, u64 task_id) = 0;

    inline SharedPtr<Table>
    GetResult() {
        std::unique_lock<std::mutex> lk(locker_);
        cv_.wait(lk, [&] { return completed_; });

        return GetResultInternal();
    }

    inline void
    Complete() {
        std::unique_lock<std::mutex> lk(locker_);
        completed_ = true;
        cv_.notify_one();
    }

    inline QueryContext*
    query_context() {
        return query_context_;
    }

    [[nodiscard]] inline FragmentType
    ContextType() const {
        return fragment_type_;
    }
protected:
    virtual SharedPtr<Table>
    GetResultInternal() = 0;

protected:
    std::mutex locker_{};
    std::condition_variable cv_{};

    PlanFragment* fragment_ptr_{};
//    HashMap<u64, UniquePtr<FragmentTask>> tasks_;
    Vector<UniquePtr<FragmentTask>> tasks_{};
    Vector<UniquePtr<InputState>> input_states_{};
    Vector<UniquePtr<OutputState>> output_states_{};
    bool finish_building_{false};
    bool completed_{false};
    i64 finished_task_count_{};
    Vector<SharedPtr<DataBlock>> data_array_{};

    FragmentType fragment_type_{FragmentType::kGlobalMaterialized};
    QueryContext* query_context_{};
};

class GlobalMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit
    GlobalMaterializedFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
        : FragmentContext(FragmentType::kGlobalMaterialized, fragment_ptr, query_context) {}

    void
    InitializeOutput(FragmentTask* fragment_task, u64 task_id) final;

    SharedPtr<Table>
    GetResultInternal() final;
protected:
    Vector<SharedPtr<DataBlock>> data_array_ptr_{};
};

class LocalMaterializedFragmentCtx : public FragmentContext {
public:
    explicit
    LocalMaterializedFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
        : FragmentContext(FragmentType::kLocalMaterialized, fragment_ptr, query_context) {}

    void
    InitializeOutput(FragmentTask* fragment_task, u64 task_id) final;

    SharedPtr<Table>
    GetResultInternal() final;
protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> result_map_{};
};

class StreamFragmentCtx : public FragmentContext {
public:
    explicit
    StreamFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
        : FragmentContext(FragmentType::kStreamFragment, fragment_ptr, query_context) {}

    void
    InitializeOutput(FragmentTask* fragment_task, u64 task_id) final;

    SharedPtr<Table>
    GetResultInternal() final;
protected:
    HashMap<u64, SharedPtr<DataBlock>> result_map_{};
};

}
