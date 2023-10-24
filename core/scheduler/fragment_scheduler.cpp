//
// Created by jinhai on 23-10-16.
//

module;

#include <sched.h>
#include <vector>

import stl;
import config;
import infinity_assert;
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

module fragment_scheduler;

namespace infinity {

// Non-static memory methods

FragmentScheduler::FragmentScheduler(const Config *config_ptr) { Init(config_ptr); }

FragmentScheduler::~FragmentScheduler() {}

void FragmentScheduler::Init(const Config *config_ptr) {
    u64 total_cpu_count = config_ptr->total_cpu_number();
    for (u64 i = 0; i < total_cpu_count; ++i) {
        cpu_set_.emplace(i);
    }

    if (cpu_set_.empty()) {
        Error<SchedulerException>("No cpu is used in scheduler", __FILE_NAME__, __LINE__);
    }

    for (i64 cpu_id : cpu_set_) {
        cpu_array_.emplace_back(cpu_id);

        UniquePtr<FragmentTaskBlockQueue> worker_queue = MakeUnique<FragmentTaskBlockQueue>();
        UniquePtr<Thread> worker_thread = MakeUnique<Thread>(&FragmentScheduler::WorkerLoop, this, worker_queue.get(), cpu_id);
        // Pin the thread to specific cpu
        ThreadUtil::pin(*worker_thread, cpu_id);

        worker_queues_.emplace(cpu_id, Move(worker_queue));
        workers.emplace(cpu_id, Move(worker_thread));
    }

    // Start coordinator
    ready_queue_ = MakeUnique<FragmentTaskBlockQueue>();
    coordinator_ = MakeUnique<Thread>(&FragmentScheduler::CoordinatorLoop, this, ready_queue_.get(), 0);

    poller_queue_ = MakeUnique<FragmentTaskPollerQueue>();
    poller_ = MakeUnique<Thread>(&FragmentScheduler::PollerLoop, this, poller_queue_.get(), 0);

    ThreadUtil::pin(*coordinator_, 0);
    ThreadUtil::pin(*poller_, 0);

    initialized_ = true;
}

void FragmentScheduler::UnInit() {
    initialized_ = false;
    UniquePtr<FragmentTask> terminate_task = MakeUnique<FragmentTask>(true);
    poller_queue_->Enqueue(terminate_task.get());
    poller_->join();
    coordinator_->join();
    for (i64 cpu_id : cpu_set_) {
        worker_queues_[cpu_id]->Enqueue(terminate_task.get());
        workers[cpu_id]->join();
    }
}

void FragmentScheduler::Schedule(QueryContext *query_context, PlanFragment *plan_fragment) {
    //    NotImplementError("Not implement FragmentScheduler::BuildFragmentTasks");
    if (plan_fragment == nullptr) {
        Error<SchedulerException>("Empty plan fragment", __FILE_NAME__, __LINE__);
    }

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
    Vector<FragmentTask *> tasks;
    FragmentContext::MakeFragmentContext(query_context, nullptr, plan_fragment, tasks);

    LOG_TRACE(Format("Create {} tasks", tasks.size()));

    for (const auto &fragment_task : tasks) {
        ScheduleTask(fragment_task);
    }
}

void FragmentScheduler::ScheduleTask(FragmentTask *task) {
    if (!initialized_) {
        Error<SchedulerException>("Scheduler isn't initialized", __FILE_NAME__, __LINE__);
    }
    poller_queue_->Enqueue(task);
}

void FragmentScheduler::CoordinatorLoop(FragmentTaskBlockQueue *ready_queue, i64 cpu_id) {
    FragmentTask *fragment_task{nullptr};
    bool running{true};
    //    LOG_TRACE("Start fragment task coordinator on CPU: {}", cpu_id);
    while (running) {
        ready_queue->Dequeue(fragment_task);
        if (fragment_task == nullptr) {
            LOG_ERROR("Fragment task coordinator: null task");
            continue;
        }

        if (fragment_task->IsTerminator()) {
            LOG_TRACE("Terminator fragment task coordinator");
            running = false;
            continue;
        }

        if (fragment_task->LastWorkerID() == -1) {
            // Select an available worker to dispatch
            i64 to_use_cpu_id = current_cpu_id_;
            do {
                to_use_cpu_id = to_use_cpu_id % cpu_array_.size();
            } while (!(DispatchTask(cpu_array_[to_use_cpu_id++], fragment_task)));

            ++current_cpu_id_;
        } else {
            // Dispatch to the same worker
            DispatchTask(fragment_task->LastWorkerID(), fragment_task);
        }
    }
    //    LOG_TRACE("Stop fragment task coordinator on CPU: {}", cpu_id);
}

void FragmentScheduler::WorkerLoop(FragmentTaskBlockQueue *task_queue, i64 worker_id) {
    FragmentTask *fragment_task{nullptr};
    bool running{true};
    //    LOG_TRACE("Start fragment task worker on CPU: {}", worker_id);
    while (running) {
        task_queue->Dequeue(fragment_task);
        if (fragment_task == nullptr) {
            LOG_ERROR(Format("worker {}: null task", worker_id));
            continue;
        }

        if (fragment_task->IsTerminator()) {
            LOG_TRACE(Format("Terminator fragment task on worker: {}", worker_id));
            running = false;
            continue;
        }

        fragment_task->OnExecute(worker_id);
        if (!fragment_task->Complete()) {
            ScheduleTask(fragment_task);
        }
    }
    //    LOG_TRACE("Stop fragment task coordinator on CPU: {}", worker_id);
}

void FragmentScheduler::PollerLoop(FragmentTaskPollerQueue *poller_queue, i64 worker_id) {
    List<FragmentTask *> local_task_list{};
    Vector<FragmentTask *> local_ready_queue{};
    bool running{true};
    //    LOG_TRACE("Start fragment task poller on CPU: {}", worker_id);
    SizeT spin_count{0};
    while (running) {
        poller_queue->DequeueBulk(local_task_list);
        auto task_iter = local_task_list.begin();
        while (!local_task_list.empty()) {
            FragmentTask *&task_ptr = (*task_iter);
            if (task_ptr->IsTerminator()) {
                running = false;
                local_ready_queue.emplace_back(task_ptr);
                local_task_list.erase(task_iter++);
            } else {
                if (task_ptr->Ready()) {
                    local_task_list.erase(task_iter++);
                    local_ready_queue.push_back(task_ptr);
                } else {
                    ++task_iter;
                }
            }

            if (local_ready_queue.empty()) {
                spin_count += 1;
            } else {
                spin_count = 0;

                ready_queue_->EnqueueBulk(local_ready_queue.begin(), local_ready_queue.size());
                local_ready_queue.clear();
            }

            if (spin_count != 0 && spin_count % 32 == 0) {
                _mm_pause();
            }
            if (spin_count == 6400) {
                spin_count = 0;
                sched_yield();
            }
            if (task_iter == local_task_list.end()) {
                task_iter = local_task_list.begin();
            }
        }
    }
    //    LOG_TRACE("Stop fragment task poller task coordinator on CPU: {}", worker_id);
}

void FragmentScheduler::SubmitTask(FragmentTask *fragment_task) { poller_queue_->Enqueue(fragment_task); }

bool FragmentScheduler::DispatchTask(i64 worker_id, FragmentTask *fragment_task) {
    if (!worker_queues_.contains(worker_id)) {
        LOG_ERROR(Format("Can't use CPU {} to run task", worker_id));
        return false;
    }
    worker_queues_[worker_id]->Enqueue(fragment_task);
    return true;
}
} // namespace infinity