//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "executor/fragment/plan_fragment.h"
#include "fragment_task_block_queue.h"
#include "fragment_task_poller_queue.h"
#include "common/types/alias/concurrency.h"

namespace infinity {

class Config;

class FragmentScheduler {
public:
    explicit FragmentScheduler(const Config *config_ptr);

    ~FragmentScheduler();

    void Init(const Config *config_ptr);

    void UnInit();

    void Schedule(QueryContext *query_context, PlanFragment *plan_fragment);

private:
    void ScheduleTask(FragmentTask *task);

    void CoordinatorLoop(FragmentTaskBlockQueue *ready_queue, i64 cpu_id);

    void WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id);

    void PollerLoop(FragmentTaskPollerQueue *poller_queue, i64 cpu_id);

    void SubmitTask(FragmentTask *fragment_task);

    bool DispatchTask(i64 worker_id, FragmentTask *task);

private:
    bool initialized_{false};

    HashSet<i64> cpu_set_{};
    Vector<i64> cpu_array_{};

    //    Vector<SharedPtr<Thread>> worker_array_{};
    HashMap<i64, UniquePtr<FragmentTaskBlockQueue>> worker_queues_{};
    HashMap<i64, UniquePtr<Thread>> workers{};

    UniquePtr<FragmentTaskBlockQueue> ready_queue_{};
    UniquePtr<Thread> coordinator_{};

    UniquePtr<FragmentTaskPollerQueue> poller_queue_{};
    UniquePtr<Thread> poller_{};

    i64 current_cpu_id_{};
};

} // namespace infinity
