//
// Created by jinhai on 23-5-5.
//

#include "scheduler.h"
#include "common/utility/threadutil.h"

#include <iostream>

namespace infinity {

void
Scheduler::CoordinatorLoop(Scheduler* scheduler_instance, i64 cpu_id) {
    Task* input_task{nullptr};
    bool running{true};
    printf("start coordinator on CPU: %ld", cpu_id);
    while(running) {
        scheduler_instance->input_queue_->Dequeue(input_task);
        if(input_task == nullptr) {
            printf("coordinator: null task\n");
            continue;
        }

        switch (input_task->type()) {
            case TaskType::kTerminate: {
                running = false;
                break;
            }
            case TaskType::kDummy: {
                DummyTask* dummy_task = (DummyTask*)input_task;
                if(dummy_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    i64 available_cpu_id = scheduler_instance->GetAvailableCPU();
                    scheduler_instance->ScheduleTask(available_cpu_id, dummy_task);
                } else {
                    scheduler_instance->ScheduleTask(dummy_task->last_worker_id_, dummy_task);
                }
                break;
            }
            case TaskType::kPipeline: {
                PipelineTask* dummy_task = (PipelineTask*)input_task;
                // Construct pipeline task and schedule it.
                if(dummy_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    i64 available_cpu_id = scheduler_instance->GetAvailableCPU();
                } else {
                }
                break;
            }
            case TaskType::kInvalid:
                break;
            case TaskType::kSource:
                break;
            case TaskType::kSink:
                break;
        }
        if(input_task->type() == TaskType::kTerminate) {
            running = false;
        } else {
//            task->run(worker_id);
        }
    }
}

void
Scheduler::WorkerLoop(TaskQueue* task_queue, i64 worker_id) {
    Task* task{nullptr};
    bool running{true};
    printf("start worker on CPU: %ld\n", worker_id);
    while(running) {
        task_queue->Dequeue(task);
        if(task == nullptr) {
            printf("worker %ld: null task\n", worker_id);
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
        return ;
    }
    cpu_set_ = cpu_set;

    for(i64 cpu_id: cpu_set) {
        UniquePtr<Thread> thread_ptr = MakeUnique<Thread>();
        UniquePtr<std::atomic<bool>> running = MakeUnique<std::atomic<bool>>(true);
        UniquePtr<TaskQueue> task_queue = MakeUnique<TaskQueue>();
        UniquePtr<Thread> task_thread = MakeUnique<Thread>(WorkerLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues_.emplace(cpu_id, std::move(task_queue));
        workers_.emplace(cpu_id, std::move(task_thread));
    }

    // Start coordinator
    auto coordinator = MakeUnique<Thread>(CoordinatorLoop, this, 0);
    ThreadUtil::pin(*coordinator, 0);
}

i64
Scheduler::GetAvailableCPU() {
    assert(false);
    return 0;
}

void
Scheduler::Uninit() {
    UniquePtr<TerminateTask> terminate_task = MakeUnique<TerminateTask>();
    input_queue_->Enqueue(terminate_task.get());
    coordinator_->join();
    for(i64 cpu_id: cpu_set_) {
        task_queues_[cpu_id]->Enqueue(terminate_task.get());
        workers_[cpu_id]->join();
        std::cout << "Stop worker: " << cpu_id << std::endl;

    }
}

void
Scheduler::ScheduleTask(i64 worker_id, Task* task) {
    task_queues_[worker_id]->Enqueue(task);
}

}