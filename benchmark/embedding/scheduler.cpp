//
// Created by jinhai on 23-5-5.
//

#include "scheduler.h"
#include "threadutil.h"

#include <iostream>
#include <unordered_set>

namespace infinity {

void
Scheduler::ExecuteLoop(TaskQueue* task_queue, int64_t worker_id) {
    Task* task{nullptr};
    bool running{true};
    while(running) {
        task_queue->Dequeue(task);
        if(task == nullptr) {
            std::cout << "Null task" << std::endl;
            continue;
        }
        if(task->type() == TaskType::kTerminate) {
            running = false;
        } else {
            task->run(worker_id);
        }
    }
}

void
Scheduler::Init(const std::unordered_set<int64_t>& cpu_set) {
    if(!cpu_set_.empty()) {
        std::cerr << "scheduler was initialized before" << std::endl;
        return;
    }
    cpu_set_ = cpu_set;

    for(int64_t cpu_id: cpu_set) {
        std::unique_ptr<std::thread> thread_ptr = std::make_unique<std::thread>();
        std::unique_ptr<std::atomic<bool>> running = std::make_unique<std::atomic<bool>>(true);
        std::unique_ptr<TaskQueue> task_queue = std::make_unique<TaskQueue>();
        std::unique_ptr<std::thread> task_thread = std::make_unique<std::thread>(ExecuteLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues_.emplace(cpu_id, std::move(task_queue));
        task_threads_.emplace(cpu_id, std::move(task_thread));

        std::cout << "Start worker: " << cpu_id << std::endl;
    }
}

void
Scheduler::Uninit() {
    std::unique_ptr<TerminateTask> terminate_task = std::make_unique<TerminateTask>();
    for(int64_t cpu_id: cpu_set_) {
        task_queues_[cpu_id]->Enqueue(terminate_task.get());
        task_threads_[cpu_id]->join();
        std::cout << "Stop worker: " << cpu_id << std::endl;
    }
}

void
Scheduler::ScheduleTask(int64_t worker_id, Task* task) {
    task_queues_[worker_id]->Enqueue(task);
}

}