//
// Created by jinhai on 23-5-11.
//


#include "task.h"
#include "threadutil.h"
#include <iostream>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <emmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse2.h>
#endif

namespace infinity {

std::unordered_set<int64_t> NewScheduler::cpu_set{};
std::unordered_map<int64_t, std::unique_ptr<BlockingQueue>> NewScheduler::task_queues{};
std::unordered_map<int64_t, std::unique_ptr<std::thread>> NewScheduler::workers{};

std::unique_ptr<BlockingQueue> NewScheduler::ready_queue{};
std::unique_ptr<std::thread> NewScheduler::coordinator{};

std::unique_ptr<PollerQueue> NewScheduler::poller_queue{};
std::unique_ptr<std::thread> NewScheduler::poller{};

std::vector<int64_t> NewScheduler::cpu_array{};
uint64_t NewScheduler::current_cpu_id{};

void
NewScheduler::PollerLoop(int64_t cpu_id) {
    std::list<Task*> local_task_list{};
    std::vector<Task*> local_ready_queue{};
    bool running{true};
    printf("start poller on CPU: %ld\n", cpu_id);
    size_t spin_count{0};
    while(running) {
        poller_queue->DequeueBulk(local_task_list);
        auto task_iter = local_task_list.begin();
        while(!local_task_list.empty()) {
            Task*& task_ptr = (*task_iter);
            if(task_ptr->type() == TaskType::kTerminate) {
                running = false;
                local_ready_queue.emplace_back(task_ptr);
                local_task_list.erase(task_iter++);
//                ++ task_iter;
            } else {
                TaskState task_state = task_ptr->state_.load();
                switch(task_state) {
                    case TaskState::kRunning:
                    case TaskState::kCancelled:
                    case TaskState::kFinished:
                    case TaskState::kReady: {
                        local_task_list.erase(task_iter++);
                        local_ready_queue.push_back(task_ptr);
                        break;
                    }
                    case TaskState::kPending: {
                        ++task_iter;
                    }
                }
            }

            if(local_ready_queue.empty()) {
                spin_count += 1;
            } else {
                spin_count = 0;

                ready_queue->EnqueueBulk(local_ready_queue.begin(), local_ready_queue.size());
                local_ready_queue.clear();
            }

            if(spin_count != 0 && spin_count % 32 == 0) {
                _mm_pause();
            }
            if(spin_count == 6400) {
                spin_count = 0;
                sched_yield();
            }
            if(task_iter == local_task_list.end()) {
                task_iter = local_task_list.begin();
            }
        }
    }
    printf("stop poller on CPU: %ld\n", cpu_id);
}

void
NewScheduler::CoordinatorLoop(int64_t cpu_id) {
    Task* input_task{nullptr};
    bool running{true};
    printf("start coordinator on CPU: %ld\n", cpu_id);
    while(running) {
        NewScheduler::ready_queue->Dequeue(input_task);
        if(input_task == nullptr) {
            printf("coordinator: null task\n");
            continue;
        }

        switch(input_task->type()) {
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
//                printf("coordinator receives PIPELINE TASK on CPU: %ld\n", cpu_id);
                PipelineTask* pipeline_task = (PipelineTask*)input_task;
                // Construct pipeline task and schedule it.
                if(pipeline_task->last_worker_id_ == -1) {
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
            case TaskType::kInvalid: {
                printf("receive invalid type of task, terminate coordinator on CPU: %ld\n", cpu_id);
                running = false;
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
NewScheduler::WorkerLoop(BlockingQueue* task_queue, int64_t worker_id) {
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
NewScheduler::Init(const std::unordered_set<int64_t>& input_cpu_set) {
    if(!cpu_set.empty()) {
        std::cerr << "scheduler was initialized before" << std::endl;
        return;
    }
    cpu_set = input_cpu_set;

    cpu_array.reserve(cpu_set.size());
    for(int64_t cpu_id: cpu_set) {
        cpu_array.emplace_back(cpu_id);

        std::unique_ptr<BlockingQueue> task_queue = std::make_unique<BlockingQueue>();
        std::unique_ptr<std::thread> task_thread = std::make_unique<std::thread>(WorkerLoop, task_queue.get(), cpu_id);

        // Pin the thread to specific cpu
        ThreadUtil::pin(*task_thread, cpu_id);

        task_queues.emplace(cpu_id, std::move(task_queue));
        workers.emplace(cpu_id, std::move(task_thread));
    }

    // Start coordinator
    ready_queue = std::make_unique<BlockingQueue>();
    coordinator = std::make_unique<std::thread>(CoordinatorLoop, 0);

    poller_queue = std::make_unique<PollerQueue>();
    poller = std::make_unique<std::thread>(PollerLoop, 0);
    ThreadUtil::pin(*coordinator, 0);
    ThreadUtil::pin(*poller, 0);
}

int64_t
NewScheduler::GetAvailableCPU() {
    assert(false);
    return 0;
}

void
NewScheduler::Uninit() {
    std::unique_ptr<TerminateTask> terminate_task = std::make_unique<TerminateTask>();
    poller_queue->Enqueue(terminate_task.get());
    poller->join();
    coordinator->join();
    for(int64_t cpu_id: cpu_set) {
        task_queues[cpu_id]->Enqueue(terminate_task.get());
        workers[cpu_id]->join();
    }
}

void
NewScheduler::RunTask(Task* task) {
    poller_queue->Enqueue(task);
}

void
NewScheduler::DispatchTask(int64_t worker_id, Task* task) {
    if(!task_queues.contains(worker_id)) {
        printf("Can't use this CPU: %ld\n", worker_id);
        assert(false);
    }
    task_queues[worker_id]->Enqueue(task);
}

}
