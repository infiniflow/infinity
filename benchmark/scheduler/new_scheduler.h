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

class NewScheduler {
public:
    static void
    Init(const HashSet<i64>& cpu_set);

    static void
    Uninit();

    static void
    ScheduleTask(i64 worker_id, Task* task);

private:
    static void
    CoordinatorLoop(i64 cpu_id);

    static void
    WorkerLoop(TaskQueue* task_queue, i64 worker_id);

    static i64
    GetAvailableCPU();
private:
    static HashSet<i64> cpu_set;

    static HashMap<i64, UniquePtr<TaskQueue>> task_queues;
    static HashMap<i64, UniquePtr<Thread>> workers;

    static UniquePtr<TaskQueue> input_queue;
    static UniquePtr<Thread> coordinator;
};

}
