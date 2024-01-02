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

        worker_array_.emplace_back(cpu_id, Move(worker_queue), Move(worker_thread));
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
    last_cpu_id_ = 0; // FIXME
    u64 &worker_id = last_cpu_id_;
    for (auto *plan_fragment : fragments) {
        auto &tasks = plan_fragment->GetContext()->Tasks();
        for (auto &task : tasks) {
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
    last_cpu_id_ = 0; // FIXME
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
    task->set_status(FragmentTaskStatus::kRunning);
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
        LOG_TRACE(Format("Fragment id: {}, type: {}", fragment->FragmentID(), FragmentType2String(fragment->GetFragmentType())));
        auto *fragment_ctx = fragment->GetContext();
        for (auto &task : fragment_ctx->Tasks()) {
            LOG_TRACE(Format("Task id: {}, status: {}", task->TaskID(), FragmentTaskStatus2String(task->status())));
        }
        for (auto iter = fragment_ctx->GetOperators().begin(); iter != fragment_ctx->GetOperators().end(); ++iter) {
            LOG_TRACE(Format("Operator type: {}", PhysicalOperatorToString((*iter)->operator_type())));
        }
        for (auto &child : fragment->Children()) {
            TraverseFragmentTree(child.get());
        }
    };
    TraverseFragmentTree(root);
    LOG_TRACE("");
}


// void TaskScheduler::ScheduleOneWorkerPerQuery(QueryContext *query_context, const Vector<FragmentTask *> &tasks, PlanFragment *plan_fragment) {
//     LOG_TRACE(Format("Schedule {} tasks of query id: {} into scheduler with OneWorkerPerQuery policy", tasks.size(), query_context->query_id()));
//     u64 worker_id = ProposedWorkerID(query_context->GetTxn()->TxnID());
//     for (const auto &fragment_task : tasks) {
//         ScheduleTask(fragment_task, worker_id);
//     }
// }

// void TaskScheduler::ScheduleOneWorkerIfPossible(QueryContext *query_context, const Vector<FragmentTask *> &tasks, PlanFragment *plan_fragment) {
//     // Schedule worker 0 if possible
//     u64 scheduled_worker = u64_max;
//     u64 min_load_worker{0};
//     u64 min_work_load{u64_max};
//     for (u64 proposed_worker = 0; proposed_worker < worker_count_; ++proposed_worker) {
//         u64 current_work_load = worker_workloads_[proposed_worker];
//         if (current_work_load < 1) {
//             scheduled_worker = proposed_worker;
//             break;
//         } else {
//             if (current_work_load < min_work_load) {
//                 min_load_worker = proposed_worker;
//                 min_work_load = current_work_load;
//             }
//         }
//     }

//     if (scheduled_worker == u64_max) {
//         scheduled_worker = min_load_worker;
//     }

//     worker_workloads_[scheduled_worker] += tasks.size();
//     LOG_TRACE(Format("Schedule {} tasks of query id: {} into worker: {} with ScheduleOneWorkerIfPossible policy",
//                      tasks.size(),
//                      query_context->query_id(),
//                      scheduled_worker));
//     for (const auto &fragment_task : tasks) {
//         ScheduleTask(fragment_task, scheduled_worker);
//     }
// }

// void TaskScheduler::ScheduleRoundRobin(const Vector<FragmentTask *> &tasks) {
//     for (const auto &fragment_task : tasks) {
//         ScheduleTask(fragment_task, last_cpu_id_++);
//         last_cpu_id_ %= worker_count_;
//     }
// }

} // namespace infinity
