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

#include <list>
#include <sched.h>
#include <vector>

import stl;
import config;

import infinity_exception;
import fragment_task_block_queue;
import fragment_task_poller_queue;
import threadutil;
import fragment_task;
import logger;
import third_party;
import query_context;
import plan_fragment;
import fragment_context;

import defer_op;

module new_task_scheduler;

namespace infinity {

// Non-static memory methods

TaskScheduler::TaskScheduler(const Config *config_ptr) { Init(config_ptr); }

void TaskScheduler::Init(const Config *config_ptr) {
    worker_count_ = config_ptr->total_cpu_number();
    worker_array_.reserve(worker_count_);
    for (u64 cpu_id = 0; cpu_id < worker_count_; ++cpu_id) {
        UniquePtr<FragmentTaskBlockQueue> worker_queue = MakeUnique<FragmentTaskBlockQueue>();
        UniquePtr<Thread> worker_thread = MakeUnique<Thread>(&TaskScheduler::WorkerLoop, this, worker_queue.get(), cpu_id);
        // Pin the thread to specific cpu
        ThreadUtil::pin(*worker_thread, cpu_id);

        worker_array_.emplace_back(cpu_id, Move(worker_queue), Move(worker_thread));
    }

    if (worker_array_.empty()) {
        Error<SchedulerException>("No cpu is used in scheduler");
    }

    // Start coordinator
    ready_queue_ = MakeUnique<FragmentTaskBlockQueue>();
    coordinator_ = MakeUnique<Thread>(&TaskScheduler::CoordinatorLoop, this, ready_queue_.get(), 0);

    ThreadUtil::pin(*coordinator_, 0);

    initialized_ = true;
}

void TaskScheduler::UnInit() {
    initialized_ = false;
    UniquePtr<FragmentTask> terminate_task = MakeUnique<FragmentTask>(true);
    ready_queue_->Enqueue(terminate_task.get());
    coordinator_->join();
    for (const auto &worker : worker_array_) {
        worker.queue_->Enqueue(terminate_task.get());
        worker.thread_->join();
    }
}

void TaskScheduler::Schedule(const Vector<FragmentTask *> &tasks) {
    //    Vector<UniquePtr<PlanFragment>>& children = plan_fragment->Children();
    //    if(!children.empty()) {
    //        SchedulerError("Only support one fragment query")
    //    }
    // 1. Recursive traverse the fragment tree
    // 2. Check the fragment
    //    if the first op is SCAN op, then get all block entry and create the source type is kScan.
    //    if the first op isn't SCAN op, fragment task source type is kQueue and a task_result_queue need to be created.
    //    According to the fragment output type to set the correct fragment task sink type.
    //    Set the queue of parent fragment task.

    LOG_TRACE(Format("Create {} tasks", tasks.size()));

    for (const auto &fragment_task : tasks) {
        LOG_TRACE(Format("task pointer {},  task id {}", u64(fragment_task), fragment_task->TaskID()));
        LOG_TRACE(fragment_task->PhysOpsToString());
        ScheduleTask(fragment_task);
    }
}

void TaskScheduler::ScheduleTask(FragmentTask *task) {
    if (!initialized_) {
        Error<SchedulerException>("Scheduler isn't initialized");
    }
    u64 worker_id = task->ProposedCPUID(worker_count_);
    LOG_TRACE(Format("schedule task id {} on cpu {}", task->TaskID(), worker_id));
    worker_array_[worker_id].queue_->Enqueue(task);
}

void TaskScheduler::ToReadyQueue(FragmentTask *task) {
    if (!initialized_) {
        Error<SchedulerException>("Scheduler isn't initialized");
    }
}

void TaskScheduler::CoordinatorLoop(FragmentTaskBlockQueue *ready_queue, i64 cpu_id) {
    FragmentTask *fragment_task{nullptr};
    bool running{true};
    LOG_TRACE(Format("Start task coordinator on CPU: {}", cpu_id));
    u64 current_cpu_id{0};
    HashSet<u64> fragment_task_ptr;
    while (running) {
        ready_queue->Dequeue(fragment_task);
        if (auto iter = fragment_task_ptr.find(u64(fragment_task)); iter == fragment_task_ptr.end()) {
            LOG_TRACE(Format("Ready queue: task to cpu {}, ready queue get task {} ops {} ready: {}",
                             fragment_task->LastWorkerID(),
                             fragment_task->TaskID(),
                             fragment_task->PhysOpsToString(),
                             fragment_task->Ready()));
            fragment_task_ptr.emplace(u64(fragment_task));
        }
        if (fragment_task == nullptr) {
            LOG_ERROR("Fragment task coordinator: null task");
            continue;
        }

        if (fragment_task->IsTerminator()) {
            LOG_TRACE("Terminator fragment task coordinator");
            running = false;
            continue;
        }

        if (!fragment_task->Ready()) {
            ready_queue->Enqueue(fragment_task);
            continue;
        }

        if (fragment_task->LastWorkerID() == -1) {
            // Select an available worker to dispatch
            u64 to_use_cpu_id = current_cpu_id;
            ++current_cpu_id;
            to_use_cpu_id %= worker_count_;
            LOG_TRACE(Format("1 Dispatch task to cpu {}", to_use_cpu_id));
            worker_array_[to_use_cpu_id].queue_->Enqueue(fragment_task);
        } else {
            // Dispatch to the same worker
            LOG_TRACE(Format("2 Dispatch task to cpu {}, ready queue get task {} ops {} ready: {}",
                             fragment_task->LastWorkerID(),
                             fragment_task->TaskID(),
                             fragment_task->PhysOpsToString(),
                             fragment_task->Ready()));
            worker_array_[fragment_task->LastWorkerID()].queue_->Enqueue(fragment_task);
        }
    }
    LOG_TRACE(Format("Stop task coordinator on CPU: {}", cpu_id));
}

void TaskScheduler::WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id) {
    FragmentTask *fragment_task{nullptr};
    bool running{true};
    LOG_TRACE(Format("Start fragment task worker on CPU: {}", worker_id));
    DeferFn defer_fn([&]() { LOG_TRACE(Format("Stop fragment task worker on CPU: {}", worker_id)); });
    while (running) {
        task_queue->Dequeue(fragment_task);
        LOG_TRACE(Format("cpu {} get task {} ops {}", worker_id, fragment_task->TaskID(), fragment_task->PhysOpsToString()));
        if (fragment_task == nullptr) {
            LOG_ERROR(Format("worker {}: null task", worker_id));
            continue;
        }

        if (fragment_task->IsTerminator()) {
            LOG_TRACE(Format("Terminator fragment task on worker: {}", worker_id));
            running = false;
            continue;
        }

        if (!fragment_task->Ready()) {
            LOG_TRACE(Format("fragment task is not ready, {}", fragment_task->PhysOpsToString()));
            ready_queue_->Enqueue(fragment_task);
            continue;
        }

        fragment_task->OnExecute(worker_id);
        fragment_task->SetLastWorkID(worker_id);
        if (!fragment_task->IsComplete()) {
            LOG_TRACE(Format("fragment task is not complete. cpu {}, get task {} ops {} ready: {}",
                             fragment_task->LastWorkerID(),
                             fragment_task->TaskID(),
                             fragment_task->PhysOpsToString(),
                             fragment_task->Ready()));
            ready_queue_->Enqueue(fragment_task);
        } else {
            LOG_TRACE("fragment task is complete");
            fragment_task->TryCompleteFragment();
        }
    }
    //    LOG_TRACE("Stop fragment task coordinator on CPU: {}", worker_id);
}

} // namespace infinity
