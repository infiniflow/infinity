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
Scheduler::Init(i32 thread_count) {
    if(thread_count_ != -1) {
        std::cerr << "scheduler was initialized before" << std::endl;
        return ;
    }
    thread_count_ = thread_count;

    for(SizeT idx = 0; idx < thread_count_; ++ idx) {
        UniquePtr<Thread> thread_ptr = MakeUnique<Thread>();
        UniquePtr<std::atomic<bool>> running = MakeUnique<std::atomic<bool>>(true);
        UniquePtr<TaskQueue> task_queue = MakeUnique<TaskQueue>();
        UniquePtr<Thread> task_thread = MakeUnique<Thread>(ExecuteLoop, task_queue.get(), idx);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, idx);

        task_queues_.emplace_back(std::move(task_queue));
        task_threads_.emplace_back(std::move(task_thread));

        std::cout << "Start worker: " << idx << std::endl;
    }
}

void
Scheduler::Uninit() {
    UniquePtr<TerminateTask> terminate_task = MakeUnique<TerminateTask>();
    for(SizeT idx = 0; idx < thread_count_; ++ idx) {
        task_queues_[idx]->Enqueue(terminate_task.get());
        task_threads_[idx]->join();
        std::cout << "Stop worker: " << idx << std::endl;
    }
}

void
Scheduler::ScheduleTask(i64 worker_id, Task* task) {
    task_queues_[worker_id]->Enqueue(task);
}

}