//
// Created by jinhai on 23-5-5.
//

#include "scheduler.h"
#include "common/utility/threadutil.h"

#include <iostream>

namespace infinity {

void
Scheduler::ExecuteLoop(TaskQueue* task_queue, i64 worker_id) {
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
Scheduler::Init(const HashSet<i64>& cpu_set) {
    if(!cpu_set_.empty()) {
        std::cerr << "scheduler was initialized before" << std::endl;
        return;
    }
    cpu_set_ = cpu_set;

    for(i64 cpu_id: cpu_set) {
        UniquePtr<Thread> thread_ptr = MakeUnique<Thread>();
        UniquePtr<std::atomic<bool>> running = MakeUnique<std::atomic<bool>>(true);
        UniquePtr<TaskQueue> task_queue = MakeUnique<TaskQueue>();
        UniquePtr<Thread> task_thread = MakeUnique<Thread>(ExecuteLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues_.emplace(cpu_id, std::move(task_queue));
        task_threads_.emplace(cpu_id, std::move(task_thread));

        std::cout << "Start worker: " << cpu_id << std::endl;
    }
}

void
Scheduler::Uninit() {
    UniquePtr<TerminateTask> terminate_task = MakeUnique<TerminateTask>();
    for(i64 cpu_id: cpu_set_) {
        task_queues_[cpu_id]->Enqueue(terminate_task.get());
        task_threads_[cpu_id]->join();
        std::cout << "Stop worker: " << cpu_id << std::endl;
    }
}

void
Scheduler::ScheduleTask(i64 worker_id, Task* task) {
    task_queues_[worker_id]->Enqueue(task);
}

}