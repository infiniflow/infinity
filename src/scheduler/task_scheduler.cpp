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

module task_scheduler;

import stl;
import config;
import status;
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
import physical_sink;

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
        UnrecoverableError("No cpu is used in scheduler");
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

u64 TaskScheduler::FindLeastWorkloadWorker() {
    u64 min_workload = worker_workloads_[0];
    u64 min_workload_worker_id = 0;
    for (u64 worker_id = 1; worker_id < worker_count_ && min_workload; ++worker_id) {
        if (worker_workloads_[worker_id] < min_workload) {
            min_workload = worker_workloads_[worker_id];
            min_workload_worker_id = worker_id;
        }
    }
    return min_workload_worker_id;
}

Pair<Vector<PlanFragment *>, SizeT> TaskScheduler::GetStartFragments(PlanFragment *plan_fragment) {
    Vector<PlanFragment *> leaf_fragments;
    SizeT all_fragment_n = 0;
    StdFunction<void(PlanFragment *)> TraversePlanFragmentTree = [&](PlanFragment *root) {
        all_fragment_n += root->GetContext()->Tasks().size();
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

    return {leaf_fragments, all_fragment_n};
}

void TaskScheduler::Schedule(PlanFragment *plan_fragment) {
    if (!initialized_) {
        UnrecoverableError("Scheduler isn't initialized");
    }
    // DumpPlanFragment(plan_fragment);

    auto [fragments, task_n] = GetStartFragments(plan_fragment);
    plan_fragment->GetContext()->notifier()->SetTaskN(task_n);
    for (auto *plan_fragment : fragments) {
        auto &tasks = plan_fragment->GetContext()->Tasks();
        for (auto &task : tasks) {
            // set the status to running
            if (!task->TryIntoWorkerLoop()) {
                UnrecoverableError("Task can't be scheduled");
            }
            u64 worker_id = FindLeastWorkloadWorker();
            ScheduleTask(task.get(), worker_id);
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
    for (auto *task_ptr : task_ptrs) {
        if (task_ptr->LastWorkerID() == -1) {
            u64 worker_id = FindLeastWorkloadWorker();
            ScheduleTask(task_ptr, worker_id);
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
        auto *fragment_ctx = fragment_task->fragment_context();
        if (!fragment_ctx->notifier()->StartTask()) {
            --worker_workloads_[worker_id];
            iter = task_lists.erase(iter);
            continue;
        }

        fragment_task->OnExecute(worker_id);
        fragment_task->SetLastWorkID(worker_id);

        bool error = false;
        bool finish = false;

        if (fragment_task->status() != FragmentTaskStatus::kError) {
            if (fragment_task->IsComplete()) {
                auto *sink_op = fragment_ctx->GetSinkOperator();
                --worker_workloads_[worker_id];
                fragment_task->CompleteTask();
                iter = task_lists.erase(iter);
                finish = true;
            } else if (fragment_task->QuitFromWorkerLoop()) {
                --worker_workloads_[worker_id];
                iter = task_lists.erase(iter);
            } else {
                ++iter;
            }
        } else {
            error = true;
            finish = true;
            --worker_workloads_[worker_id];
            iter = task_lists.erase(iter);
        }
        if (finish) {
            fragment_ctx->notifier()->FinishTask(error);
        }
    }
}

void TaskScheduler::DumpPlanFragment(PlanFragment *root) {
    StdFunction<void(PlanFragment *)> TraverseFragmentTree = [&](PlanFragment *fragment) {
        auto *fragment_ctx = fragment->GetContext();
        LOG_INFO(fmt::format("Fragment id: {}, type: {}, ctx: {}",
                             fragment->FragmentID(),
                             FragmentType2String(fragment->GetFragmentType()),
                             u64(fragment_ctx)));
        fragment_ctx->DumpFragmentCtx();
        for (auto &child : fragment->Children()) {
            TraverseFragmentTree(child.get());
        }
    };
    LOG_INFO(">>> DUMP START");
    TraverseFragmentTree(root);
    LOG_INFO(">>> DUMP END");
}

} // namespace infinity
