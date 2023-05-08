//
// Created by jinhai on 23-5-5.
//

#include "new_scheduler.h"
#include "common/utility/threadutil.h"

#include <iostream>

namespace infinity {

HashSet<i64> NewScheduler::cpu_set{};
HashMap<i64, UniquePtr<TaskQueue>> NewScheduler::task_queues{};
HashMap<i64, UniquePtr<Thread>> NewScheduler::workers{};
UniquePtr<TaskQueue> NewScheduler::input_queue{};
UniquePtr<Thread> NewScheduler::coordinator{};

void
NewScheduler::CoordinatorLoop(i64 cpu_id) {
    Task* input_task{nullptr};
    bool running{true};
    printf("start coordinator on CPU: %ld\n", cpu_id);
    while(running) {
        NewScheduler::input_queue->Dequeue(input_task);
        if(input_task == nullptr) {
            printf("coordinator: null task\n");
            continue;
        }

        switch (input_task->type()) {
            case TaskType::kTerminate: {
                printf("terminate coordinator on CPU: %ld\n", cpu_id);
                running = false;
                break;
            }
            case TaskType::kDummy: {
                DummyTask* dummy_task = (DummyTask*)input_task;
                if(dummy_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    i64 available_cpu_id = NewScheduler::GetAvailableCPU();
                    NewScheduler::ScheduleTask(available_cpu_id, dummy_task);
                } else {
                    NewScheduler::ScheduleTask(dummy_task->last_worker_id_, dummy_task);
                }
                break;
            }
            case TaskType::kPipeline: {
                PipelineTask* dummy_task = (PipelineTask*)input_task;
                // Construct pipeline task and schedule it.
                if(dummy_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    i64 available_cpu_id = NewScheduler::GetAvailableCPU();
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
NewScheduler::WorkerLoop(TaskQueue* task_queue, i64 worker_id) {
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
            printf("terminate worker on CPU: %ld\n", worker_id);
            running = false;
        } else {
            task->run(worker_id);
        }
    }
}

void
NewScheduler::Init(const HashSet<i64>& input_cpu_set) {
    if(!cpu_set.empty()) {
        std::cerr << "scheduler was initialized before" << std::endl;
        return ;
    }
    cpu_set = input_cpu_set;

    for(i64 cpu_id: cpu_set) {
        UniquePtr<TaskQueue> task_queue = MakeUnique<TaskQueue>();
        UniquePtr<Thread> task_thread = MakeUnique<Thread>(WorkerLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues.emplace(cpu_id, std::move(task_queue));
        workers.emplace(cpu_id, std::move(task_thread));
    }

    // Start coordinator
    input_queue = MakeUnique<TaskQueue>();
    coordinator = MakeUnique<Thread>(CoordinatorLoop, 0);
    ThreadUtil::pin(*coordinator, 0);
}

i64
NewScheduler::GetAvailableCPU() {
    assert(false);
    return 0;
}

void
NewScheduler::Uninit() {
    UniquePtr<TerminateTask> terminate_task = MakeUnique<TerminateTask>();
    input_queue->Enqueue(terminate_task.get());
    coordinator->join();
    for(i64 cpu_id: cpu_set) {
        task_queues[cpu_id]->Enqueue(terminate_task.get());
        workers[cpu_id]->join();
    }
}

void
NewScheduler::ScheduleTask(i64 worker_id, Task* task) {
    task_queues[worker_id]->Enqueue(task);
}

}