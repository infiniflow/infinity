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
//import query_context;
//import plan_fragment;
import fragment_task;
import fragment_task_block_queue;
import fragment_task_poller_queue;

export module fragment_scheduler;

namespace infinity {

class QueryContext;
class PlanFragment;

export class FragmentScheduler {
public:
    explicit FragmentScheduler(const Config *config_ptr);

    ~FragmentScheduler();

    void Init(const Config *config_ptr);

    void UnInit();

    void Schedule(const Vector<FragmentTask *>& tasks);

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
