//
// Created by jinhai on 23-9-8.
//

#pragma once


#include "main/query_context.h"
#include "scheduler/fragment_task.h"
#include "executor/physical_operator.h"
#include "executor/operator/physical_sink.h"
#include "executor/operator/physical_source.h"

namespace infinity {

class PlanFragment;


//enum class FragmentStatus {
//    kNotStart,
//    k
//    kStart,
//    kFinish,
//};
enum class FragmentType {
    kInvalid,
    kSerialMaterialize,
    kParallelMaterialize,
    kParallelStream,
};

class FragmentContext {
public:
    static void
    MakeFragmentContext(QueryContext* query_context,
                        FragmentContext* parent_context,
                        PlanFragment* fragment_ptr,
                        Vector<FragmentTask*>& tasks);

public:
    explicit
    FragmentContext(PlanFragment* fragment_ptr, QueryContext* query_context);

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

    [[nodiscard]] PhysicalSink*
    GetSinkOperator() const;

    [[nodiscard]] PhysicalSource*
    GetSourceOperator() const;

    void
    CreateTasks(i64 parallel_count, i64 operator_count);

    inline Vector<UniquePtr<FragmentTask>>&
    Tasks() {
        return tasks_;
    }

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

    bool finish_building_{false};
    bool completed_{false};
    i64 finished_task_count_{};
    Vector<SharedPtr<DataBlock>> data_array_{};

    FragmentType fragment_type_{FragmentType::kInvalid};
    QueryContext* query_context_{};
};

class SerialMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit
    SerialMaterializedFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
            : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table>
    GetResultInternal() final;

protected:
    Vector<SharedPtr<DataBlock>> task_result_{};
};

class ParallelMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit
    ParallelMaterializedFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
            : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table>
    GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

class ParallelStreamFragmentCtx : public FragmentContext {
public:
    explicit
    ParallelStreamFragmentCtx(PlanFragment* fragment_ptr, QueryContext* query_context)
            : FragmentContext(fragment_ptr, query_context) {}

    SharedPtr<Table>
    GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

}
