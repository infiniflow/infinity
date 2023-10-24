//
// Created by jinhai on 23-5-11.
//


#include "task.h"
#include "common/utility/threadutil.h"
#include <iostream>

namespace infinity {

HashSet<i64> NewScheduler::cpu_set{};
HashMap<i64, UniquePtr<BlockingQueue>> NewScheduler::task_queues{};
HashMap<i64, UniquePtr<std::thread>> NewScheduler::workers{};
UniquePtr<BlockingQueue> NewScheduler::input_queue{};
UniquePtr<std::thread> NewScheduler::coordinator{};
Vector<i64> NewScheduler::cpu_array{};
u64 NewScheduler::current_cpu_id{};

void
NewScheduler::CoordinatorLoop(i64 cpu_id) {
    Vector<Task*> input_tasks(50);
//    Task* input_task{nullptr};
    bool running{true};
    printf("start coordinator on CPU: %ld\n", cpu_id);
    while(running) {
        SizeT task_count = NewScheduler::input_queue->DequeueBulk(input_tasks.begin(), 50);
//        if(task_count > 1) {
//            printf("Get tasks count: %lu\n", task_count);
//        }
        for(SizeT idx = 0; idx < task_count; ++idx) {
            Task* input_task = input_tasks[idx];
            if(__builtin_expect((input_task == nullptr), false)) {
                printf("coordinator: null task\n");
                continue;
            }

            switch(input_task->type()) {
                case TaskType::kPipeline: {
//                printf("coordinator receives PIPELINE TASK on CPU: %ld\n", cpu_id);
                    PipelineTask* pipeline_task = (PipelineTask*)input_task;
                    // Construct pipeline task and schedule it.
                    if(__builtin_expect((pipeline_task->last_worker_id_ == -1), true)) {
                        // Select an available cpu
                        current_cpu_id = current_cpu_id % cpu_array.size();
//                    printf("Dispatched to CPU: %ld\n", cpu_array[current_cpu_id]);
                        NewScheduler::DispatchTask(cpu_array[current_cpu_id], pipeline_task);
                        ++current_cpu_id;
                    } else {
                        NewScheduler::DispatchTask(pipeline_task->last_worker_id_, pipeline_task);
                    }
                    break;
                }
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
                        ++current_cpu_id;
                    } else {
                        NewScheduler::DispatchTask(dummy_task->last_worker_id_, dummy_task);
                    }
                    break;
                }
                case TaskType::kInvalid: {
                    printf("receive invalid type of task, terminate coordinator on CPU: %ld\n", cpu_id);
                    running = false;
                    break;
                }
            }
        }
    }
}

void
NewScheduler::WorkerLoop(BlockingQueue* task_queue, i64 worker_id) {
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
            case TaskType::kPipeline: {
                task->Run(worker_id);
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
        return;
    }
    cpu_set = input_cpu_set;

    cpu_array.reserve(cpu_set.size());
    for(i64 cpu_id: cpu_set) {
        cpu_array.emplace_back(cpu_id);

        UniquePtr<BlockingQueue> task_queue = MakeUnique<BlockingQueue>();
        UniquePtr<std::thread> task_thread = MakeUnique<std::thread>(WorkerLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues.emplace(cpu_id, std::move(task_queue));
        workers.emplace(cpu_id, std::move(task_thread));
    }

    // Start coordinator
    input_queue = MakeUnique<BlockingQueue>();
    coordinator = MakeUnique<std::thread>(CoordinatorLoop, 0);
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

//void
//NewScheduler::DispatchTask(i64 worker_id, Task* task) {
////    if(!task_queues.contains(worker_id)) {
////        printf("Can't use this CPU: %ld\n", worker_id);
////        assert(false);
////    }
//    task_queues[worker_id]->Enqueue(task);
//}

}