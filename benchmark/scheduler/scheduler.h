//
// Created by jinhai on 23-5-5.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include "task.h"

namespace infinity {

struct TaskQueue {
    void
    Enqueue(Task* task) {
        queue_.enqueue(task);
    }

    void
    Dequeue(Task*& task) {
        queue_.wait_dequeue(task);
    }

    moodycamel::BlockingConcurrentQueue<Task*> queue_;
};

class Scheduler {
public:
    Scheduler() = default;

    void
    Init(const HashSet<i64>& cpu_set);

    void
    Uninit();

    void
    ScheduleTask(i64 worker_id, Task* task);

private:
    static void
    CoordinatorLoop(Scheduler* scheduler_instance, i64 cpu_id);

    static void
    WorkerLoop(TaskQueue* task_queue, i64 worker_id);

    i64
    GetAvailableCPU();
private:
    HashSet<i64> cpu_set_{};

    HashMap<i64, UniquePtr<TaskQueue>> task_queues_{};
    HashMap<i64, UniquePtr<Thread>> workers_{};

    UniquePtr<TaskQueue> input_queue_{};
    UniquePtr<Thread> coordinator_{};
};

}
