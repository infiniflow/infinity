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

export module task_scheduler;

import config;
import stl;
import fragment_task;
import blocking_queue;
import base_statement;

namespace infinity {

class QueryContext;
class PlanFragment;

using FragmentTaskBlockQueue = BlockingQueue<FragmentTask*>;

struct Worker {
    Worker(u64 cpu_id, UniquePtr<FragmentTaskBlockQueue> queue, UniquePtr<Thread> thread)
        : cpu_id_(cpu_id), queue_(std::move(queue)), thread_(std::move(thread)) {}
    u64 cpu_id_{0};
    UniquePtr<FragmentTaskBlockQueue> queue_{};
    UniquePtr<Thread> thread_{};
};

export class TaskScheduler {
public:
    explicit TaskScheduler(Config *config_ptr);

    ~TaskScheduler() = default;

    void Init(Config *config_ptr);

    void UnInit();

    // Schedule start fragments
    void Schedule(PlanFragment * plan_fragment_root, const BaseStatement* base_statement, bool to_threads1 = false);

    // `plan_fragment` can be scheduled because all of its dependencies are met.
    void ScheduleFragment(PlanFragment *plan_fragment);

    void DumpPlanFragment(PlanFragment *plan_fragment);

private:
    u64 FindLeastWorkloadWorker();

    u64 FindLeastWorkloadWorker1();

    void ScheduleTask(FragmentTask *task, u64 worker_id);

    void RunTask(FragmentTask *task);

    void WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id);

private:
    bool initialized_{false};

    Vector<Worker> worker_array_{};
    Deque<Atomic<u64>> worker_workloads_{};

    u64 worker_count_{0};
    u64 worker_count1_{0};
};

} // namespace infinity
