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
Vector<i64> NewScheduler::cpu_array{};
u64 NewScheduler::current_cpu_id{};

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
                    NewScheduler::DispatchTask(current_cpu_id % cpu_array.size(), dummy_task);
                } else {
                    NewScheduler::DispatchTask(dummy_task->last_worker_id_, dummy_task);
                }
                break;
            }
            case TaskType::kPipeline: {
                printf("receive PIPELINE type of task on CPU: %ld\n", cpu_id);
                PipelineTask* pipeline_task = (PipelineTask*)input_task;
                // Construct pipeline task and schedule it.
                if(pipeline_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    NewScheduler::DispatchTask(current_cpu_id % cpu_array.size(), pipeline_task);
                } else {
                    NewScheduler::DispatchTask(pipeline_task->last_worker_id_, pipeline_task);
                }
                break;
            }
            case TaskType::kInvalid: {
                printf("receive invalid type of task, terminate coordinator on CPU: %ld\n", cpu_id);
                running = false;
                break;
            }
            case TaskType::kSource: {
                printf("receive SOURCE type of task on CPU: %ld\n", cpu_id);
                if(input_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    NewScheduler::DispatchTask(current_cpu_id % cpu_array.size(), input_task);
                } else {
                    NewScheduler::DispatchTask(input_task->last_worker_id_, input_task);
                }
                break;
            }
            case TaskType::kSink: {
                printf("receive SINK type of task on CPU: %ld\n", cpu_id);
                if(input_task->last_worker_id_ == -1) {
                    // Select an available cpu
                    NewScheduler::DispatchTask(current_cpu_id % cpu_array.size(), input_task);
                } else {
                    NewScheduler::DispatchTask(input_task->last_worker_id_, input_task);
                }
                break;
            }
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
        switch(task->type()) {
            case TaskType::kTerminate: {
                printf("terminate worker on CPU: %ld\n", worker_id);
                running = false;
                break;
            }
            case TaskType::kDummy:
            case TaskType::kPipeline:
            case TaskType::kSource:
            case TaskType::kSink: {
                task->run(worker_id);
                break;
            }
            case TaskType::kInvalid: {
                printf("receive invalid type of task, terminate worker on CPU: %ld\n", worker_id);
                running = false;
                break;
            }
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

    cpu_array.reserve(cpu_set.size());
    for(i64 cpu_id: cpu_set) {
        cpu_array.emplace_back(cpu_id);

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
NewScheduler::RunTask(Task* task) {
    input_queue->Enqueue(task);
}

void
NewScheduler::DispatchTask(i64 worker_id, Task* task) {
    task_queues[worker_id]->Enqueue(task);
}

}