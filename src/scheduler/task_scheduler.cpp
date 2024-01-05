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

import stl;
import config;

import infinity_exception;
import threadutil;
import fragment_task;
import logger;
import third_party;
import query_context;
import plan_fragment;
import fragment_context;
import default_values;
import physical_operator_type;
import physical_operator;

module task_scheduler;

namespace infinity {

// Non-static memory methods

TaskScheduler::TaskScheduler(const Config *config_ptr) { Init(config_ptr); }

void TaskScheduler::Init(const Config *config_ptr) {
    worker_count_ = config_ptr->worker_cpu_limit();
    worker_array_.reserve(worker_count_);
    worker_workloads_.resize(worker_count_);
    u64 cpu_count = Thread::hardware_concurrency();
    for (u64 cpu_id = 0; cpu_id < worker_count_; ++cpu_id) {
        UniquePtr<FragmentTaskBlockQueue> worker_queue = MakeUnique<FragmentTaskBlockQueue>();
        UniquePtr<Thread> worker_thread = MakeUnique<Thread>(&TaskScheduler::WorkerLoop, this, worker_queue.get(), cpu_id);
        // Pin the thread to specific cpu
        ThreadUtil::pin(*worker_thread, cpu_id % cpu_count);

        worker_array_.emplace_back(cpu_id, std::move(worker_queue), std::move(worker_thread));
        worker_workloads_[cpu_id] = 0;
    }

    if (worker_array_.empty()) {
        Error<SchedulerException>("No cpu is used in scheduler");
    }

    initialized_ = true;
}

void TaskScheduler::UnInit() {
    initialized_ = false;
    UniquePtr<FragmentTask> terminate_task = MakeUnique<FragmentTask>(true);

    for (const auto &worker : worker_array_) {
        worker.queue_->Enqueue(terminate_task.get());
        worker.thread_->join();
    }
}

Vector<PlanFragment *> TaskScheduler::GetStartFragments(PlanFragment *plan_fragment) {
    Vector<PlanFragment *> leaf_fragments;
    StdFunction<void(PlanFragment *)> TraversePlanFragmentTree = [&](PlanFragment *root) {
        if (root->Children().empty()) {
            leaf_fragments.emplace_back(root);
            return;
        }
        for (auto &child : root->Children()) {
            TraversePlanFragmentTree(child.get());
        }
    };
    // Traverse the tree to get all leaf fragments
    TraversePlanFragmentTree(plan_fragment);

    return leaf_fragments;
}

void TaskScheduler::Schedule(PlanFragment *plan_fragment) {
    if (!initialized_) {
        Error<SchedulerException>("Scheduler isn't initialized");
    }

    Vector<PlanFragment *> fragments = GetStartFragments(plan_fragment);
    if (worker_workloads_[0] == 0) {
        last_cpu_id_ = 0; // FIXME
    }
    u64 &worker_id = last_cpu_id_;
    for (auto *plan_fragment : fragments) {
        auto &tasks = plan_fragment->GetContext()->Tasks();
        for (auto &task : tasks) {
            // set the status to running
            if (!task->TryIntoWorkerLoop()) {
                Error<SchedulerException>("Task can't be scheduled");
            }
            ScheduleTask(task.get(), worker_id);
            ++worker_id;
            worker_id %= worker_count_;
        }
    }
}

void TaskScheduler::ScheduleFragment(PlanFragment *plan_fragment) {
    Vector<FragmentTask *> task_ptrs;
    auto &tasks = plan_fragment->GetContext()->Tasks();
    for (auto &task : tasks) {
        if (task->TryIntoWorkerLoop()) {
            task_ptrs.emplace_back(task.get());
        }
    }
    // last_cpu_id_ = 0; // FIXME
    u64 &worker_id = last_cpu_id_;
    for (auto *task_ptr : task_ptrs) {
        if (task_ptr->LastWorkerID() == -1) {
            ScheduleTask(task_ptr, worker_id);
            ++worker_id;
            worker_id %= worker_count_;
        } else {
            ScheduleTask(task_ptr, task_ptr->LastWorkerID());
        }
    }
}

void TaskScheduler::ScheduleTask(FragmentTask *task, u64 worker_id) {
    ++worker_workloads_[worker_id];
    worker_array_[worker_id].queue_->Enqueue(task);
}

void TaskScheduler::WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id) {
    List<FragmentTask *> task_lists;
    auto iter = task_lists.end();
    while (true) {
        Vector<FragmentTask *> dequeue_output;
        if (task_lists.empty()) {
            task_queue->DequeueBulk(dequeue_output);
        } else {
            task_queue->TryDequeueBulk(dequeue_output);
        }
        if (!dequeue_output.empty()) {
            task_lists.insert(task_lists.end(), dequeue_output.begin(), dequeue_output.end());
        }
        if (iter == task_lists.end()) {
            iter = task_lists.begin();
        }
        auto &fragment_task = *iter;
        if (fragment_task->IsTerminator()) {
            break;
        }

        fragment_task->OnExecute(worker_id);
        fragment_task->SetLastWorkID(worker_id);

        if (fragment_task->IsComplete()) {
            --worker_workloads_[worker_id];
            fragment_task->CompleteTask();
            iter = task_lists.erase(iter);
        } else if (fragment_task->QuitFromWorkerLoop()) {
            --worker_workloads_[worker_id];
            iter = task_lists.erase(iter);
        } else {
            ++iter;
        }
    }
}

void TaskScheduler::DumpPlanFragment(PlanFragment *root) {
    StdFunction<void(PlanFragment *)> TraverseFragmentTree = [&](PlanFragment *fragment) {
        LOG_TRACE(fmt::format("Fragment id: {}, type: {}", fragment->FragmentID(), FragmentType2String(fragment->GetFragmentType())));
        auto *fragment_ctx = fragment->GetContext();
        for (auto &task : fragment_ctx->Tasks()) {
            LOG_TRACE(fmt::format("Task id: {}, status: {}", task->TaskID(), FragmentTaskStatus2String(task->status())));
        }
        for (auto iter = fragment_ctx->GetOperators().begin(); iter != fragment_ctx->GetOperators().end(); ++iter) {
            LOG_TRACE(fmt::format("Operator type: {}", PhysicalOperatorToString((*iter)->operator_type())));
        }
        for (auto &child : fragment->Children()) {
            TraverseFragmentTree(child.get());
        }
    };
    TraverseFragmentTree(root);
    LOG_TRACE("");
}

} // namespace infinity
