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

import config;
import stl;
import fragment_task;
import blocking_queue;

export module task_scheduler;

namespace infinity {

class QueryContext;
class PlanFragment;

using FragmentTaskBlockQueue = BlockingQueue<FragmentTask*>;

struct Worker {
    Worker(u64 cpu_id, UniquePtr<FragmentTaskBlockQueue> queue, UniquePtr<Thread> thread)
        : cpu_id_(cpu_id), queue_(Move(queue)), thread_(Move(thread)) {}
    u64 cpu_id_{0};
    UniquePtr<FragmentTaskBlockQueue> queue_{};
    UniquePtr<Thread> thread_{};
};

export class TaskScheduler {
public:
    explicit TaskScheduler(const Config *config_ptr);

    ~TaskScheduler() = default;

    void Init(const Config *config_ptr);

    void UnInit();

    void Schedule(QueryContext* query_context, const Vector<FragmentTask *> &tasks, PlanFragment* plan_fragment);

private:
    void ScheduleOneWorkerPerQuery(QueryContext* query_context, const Vector<FragmentTask *> &tasks, PlanFragment* plan_fragment);
    void ScheduleOneWorkerIfPossible(QueryContext* query_context, const Vector<FragmentTask *> &tasks, PlanFragment* plan_fragment);
    void ScheduleRoundRobin(QueryContext* query_context, const Vector<FragmentTask *> &tasks, PlanFragment* plan_fragment);

    inline void ScheduleTask(FragmentTask *task, u64 worker_id) {
        worker_array_[worker_id].queue_->Enqueue(task);
    }

    inline u64 ProposedWorkerID(u64 object_id) const {
        return (object_id) % worker_count_;
    }

    void ToReadyQueue(FragmentTask *task);

    void CoordinatorLoop(FragmentTaskBlockQueue *ready_queue, i64 cpu_id);

    void WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id);

private:
    bool initialized_{false};

    Vector<Worker> worker_array_{};
    Deque<Atomic<u64>> worker_workloads_{};

    UniquePtr<FragmentTaskBlockQueue> ready_queue_{};
    UniquePtr<Thread> coordinator_{};

    u64 worker_count_{0};
};

} // namespace infinity
