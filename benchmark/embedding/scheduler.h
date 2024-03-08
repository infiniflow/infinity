//
// Created by jinhai on 23-5-5.
//

#pragma once

#include "mpsc_queue.h"
#include "blockingconcurrentqueue.h"

#include <unordered_set>
#include <unordered_map>

#include <unistd.h>

namespace infinity {

enum class TaskType {
    kTerminate,
    kDummy,
    kAnnFlat,
    kAnnIVFSQ8,
    kAnnIVFFlat,
    kAnnHNSW,
    kInvalid,
};

struct Task {
    inline explicit
    Task(TaskType type) : type_(type) {}

    virtual void
    run(int64_t worker_id) {
        // Not implemented
    }

    [[nodiscard]] inline TaskType
    type() const {
        return type_;
    }

    TaskType type_{TaskType::kInvalid};
};

struct TerminateTask final : public Task {
    inline explicit
    TerminateTask() : Task(TaskType::kTerminate) {}
};

struct DummyTask final : public Task {
    inline explicit
    DummyTask() : Task(TaskType::kDummy) {}

    void
    run(int64_t worker_id) override {
        printf("Run dummy task by worker: %ld\n", (long)worker_id);
        sleep(1);
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
    Init(const std::unordered_set<int64_t>& cpu_set);

    void
    Uninit();

    void
    ScheduleTask(int64_t worker_id, Task* task);

    static void
    ExecuteLoop(TaskQueue* task_queue, int64_t worker_id);

private:
    std::unordered_set<int64_t> cpu_set_{};
    std::unordered_map<int64_t, std::unique_ptr<TaskQueue>> task_queues_{};
    std::unordered_map<int64_t, std::unique_ptr<std::thread>> task_threads_{};
};

}
