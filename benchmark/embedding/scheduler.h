//
// Created by jinhai on 23-5-5.
//

#pragma once

#include "common/utility/mpsc_queue.h"
#include "common/types/internal_types.h"
#include "blockingconcurrentqueue.h"

namespace infinity {

enum class TaskType {
    kTerminate,
    kDummy,
    kAnnFlat,
    kAnnIVFFlat,
    kInvalid,
};

struct Task {
    inline explicit
    Task(TaskType type) : type_(type) {}

    virtual void
    run(i64 worker_id) {
        // Not implemented
    }

    [[nodiscard]] inline TaskType
    type() const {
        return type_;
    }

    TaskType type_{TaskType::kInvalid};
};

struct TerminateTask final: public Task {
    inline explicit
    TerminateTask() : Task(TaskType::kTerminate) {}
};

struct DummyTask final: public Task {
    inline explicit
    DummyTask() : Task(TaskType::kDummy) {}

    void
    run(i64 worker_id) override {
        printf("Run dummy task by worker: %ld\n", worker_id);
        usleep(1000 * 1000);
    }
};

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
//    MPSCQueue<Task*> queue_;
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

    static void
    ExecuteLoop(TaskQueue* task_queue, i64 worker_id);
private:
    HashSet<i64> cpu_set_{};

    HashMap<i64, UniquePtr<TaskQueue>> task_queues_{};
    HashMap<i64, UniquePtr<Thread>> task_threads_{};
};

}
