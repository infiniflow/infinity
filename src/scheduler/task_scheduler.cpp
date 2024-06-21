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
import base_statement;
import extra_ddl_info;
import create_statement;
import command_statement;

namespace infinity {

// Non-static memory methods

TaskScheduler::TaskScheduler(Config *config_ptr) { Init(config_ptr); }

void TaskScheduler::Init(Config *config_ptr) {
    const u64 cpu_count = Thread::hardware_concurrency();
    const u64 config_cpu_limit = config_ptr->CPULimit();
    worker_count_ = std::min(cpu_count, config_cpu_limit);
    worker_array_.reserve(worker_count_);
    worker_workloads_.resize(worker_count_);

    Vector<u64> cpu_id_vec;
    cpu_id_vec.reserve(cpu_count);
    // even cpus first
    for (u64 cpu_id = 0; cpu_id < cpu_count; cpu_id += 2) {
        cpu_id_vec.push_back(cpu_id);
    }
    // then add odd cpus
    for (u64 cpu_id = 1; cpu_id < cpu_count; cpu_id += 2) {
        cpu_id_vec.push_back(cpu_id);
    }

    for (u64 worker_id = 0; worker_id < worker_count_; ++worker_id) {
        const u64 cpu_id = cpu_id_vec[worker_id];
        UniquePtr<FragmentTaskBlockQueue> worker_queue = MakeUnique<FragmentTaskBlockQueue>();
        UniquePtr<Thread> worker_thread = MakeUnique<Thread>(&TaskScheduler::WorkerLoop, this, worker_queue.get(), worker_id);
        // Pin the thread to specific cpu
        ThreadUtil::pin(*worker_thread, cpu_id);

        worker_array_.emplace_back(cpu_id, std::move(worker_queue), std::move(worker_thread));
        worker_workloads_[worker_id] = 0;
    }

    if (worker_array_.empty()) {
        String error_message = "No cpu is used in scheduler";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
        u64 current_worker_load = worker_workloads_[worker_id];
        if (current_worker_load < min_workload) {
            min_workload = current_worker_load;
            min_workload_worker_id = worker_id;
        }
    }
    return min_workload_worker_id;
}

void TaskScheduler::Schedule(PlanFragment *plan_fragment, const BaseStatement *base_statement) {
    if (!initialized_) {
        String error_message = "Scheduler isn't initialized";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    // DumpPlanFragment(plan_fragment);
    bool use_scheduler = false;
    switch(base_statement->Type()) {
        case StatementType::kSelect:
        case StatementType::kExplain:
        case StatementType::kDelete:
        case StatementType::kUpdate: 
        case StatementType::kCompact:{
            use_scheduler = true; // continue;
            break;
        }
        case StatementType::kCreate: {
            const CreateStatement *create_statement = static_cast<const CreateStatement *>(base_statement);
            if (create_statement->create_info_->type_ == DDLType::kIndex) {
                // Create index will generate multiple tasks
                use_scheduler = true;
            }
            break;
        }
        default: {
            ;
        }
    }

    if(!use_scheduler) {
        if (!plan_fragment->HasChild()) {
            if (plan_fragment->GetContext()->Tasks().size() == 1) {
                FragmentTask *task = plan_fragment->GetContext()->Tasks()[0].get();
                RunTask(task);
                return ;
            } else {
                String error_message = "Oops! None select and create idnex statement has multiple fragments.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
        } else {
            String error_message = "None select statement has multiple fragments.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    Vector<PlanFragment *> start_fragments;
    SizeT task_n = plan_fragment->GetStartFragments(start_fragments);
    plan_fragment->GetContext()->notifier()->SetTaskN(task_n);
    for (auto *sub_fragment : start_fragments) {
        auto &tasks = sub_fragment->GetContext()->Tasks();
        for (auto &task : tasks) {
            // set the status to running
            if (!task->TryIntoWorkerLoop()) {
                String error_message = "Task can't be scheduled";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            u64 worker_id = FindLeastWorkloadWorker();
            ScheduleTask(task.get(), worker_id);
        }
    }
}

void TaskScheduler::RunTask(FragmentTask *task) {

    bool finish = false;
    bool error = false;
    task->fragment_context()->notifier()->SetTaskN(1);
    do {
        task->TryIntoWorkerLoop();
        task->OnExecute();
        if (task->status() != FragmentTaskStatus::kError) {
            if (task->IsComplete()) {
                task->CompleteTask();
                finish = true;
            }
        } else {
            error = true;
            finish = true;
        }
    } while (!finish);
    task->fragment_context()->notifier()->FinishTask(error, task->fragment_context());
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
    auto last_iter = task_lists.end();
    while (true) {
        if (iter == last_iter) {
            Vector<FragmentTask *> dequeue_output;
            if (task_lists.empty()) {
                task_queue->DequeueBulk(dequeue_output);
            } else {
                task_queue->TryDequeueBulk(dequeue_output);
            }
            if (!dequeue_output.empty()) {
                task_lists.insert(task_lists.end(), dequeue_output.begin(), dequeue_output.end());
            }
            last_iter = task_lists.end();
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

        fragment_task->OnExecute();
        fragment_task->SetLastWorkID(worker_id);

        bool error = false;
        bool finish = false;

        if (fragment_task->status() != FragmentTaskStatus::kError) {
            if (fragment_task->IsComplete()) {
                // auto *sink_op = fragment_ctx->GetSinkOperator();
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
        if (finish || error) {
            fragment_ctx->notifier()->FinishTask(error, fragment_ctx);
        } else {
            fragment_ctx->notifier()->UnstartTask();
        }
    }
}

void TaskScheduler::DumpPlanFragment(PlanFragment *root) {
    std::function<void(PlanFragment *)> TraverseFragmentTree = [&](PlanFragment *fragment) {
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
