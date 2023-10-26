//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "operator.h"
#include "buffer_queue.h"
#include "task_queue.h"
#include "poller_queue.h"
#include <unistd.h>
#include <cassert>
#include <unordered_set>
#include <unordered_map>

namespace infinity {

class Task;

class NewScheduler {
public:
    static void
    Init(const std::unordered_set<int64_t>& cpu_set);

    static void
    Uninit();

    static void
    RunTask(Task* task);

private:
    static void
    DispatchTask(int64_t worker_id, Task* task);

    static void
    PollerLoop(int64_t cpu_id);

    static void
    CoordinatorLoop(int64_t cpu_id);

    static void
    WorkerLoop(BlockingQueue* task_queue, int64_t worker_id);

    static int64_t
    GetAvailableCPU();

private:
    static std::unordered_set<int64_t> cpu_set;

    static std::unordered_map<int64_t, std::unique_ptr<BlockingQueue>> task_queues;
    static std::unordered_map<int64_t, std::unique_ptr<std::thread>> workers;

    static std::unique_ptr<BlockingQueue> ready_queue;
    static std::unique_ptr<std::thread> coordinator;

    static std::unique_ptr<PollerQueue> poller_queue;
    static std::unique_ptr<std::thread> poller;

    static std::vector<int64_t> cpu_array;
    static uint64_t current_cpu_id;
};


#define BUFFER_SIZE 128

enum class TaskType {
    kTerminate,
    kDummy,
    kPipeline,
    kInvalid,
};

enum class TaskState {
    kReady,
    kFinished,
    kCancelled,
    kPending,
    kRunning,
};

struct Task {
    inline explicit
    Task(TaskType type) : type_(type) {}

    virtual void
    Run(int64_t worker_id) {
        // Not implemented
        last_worker_id_ = worker_id;
    }

    inline void
    set_state(TaskState state) {
        state_.store(state);
    }

    [[nodiscard]] inline TaskType
    type() const {
        return type_;
    }

    TaskType type_{TaskType::kInvalid};
    int64_t last_worker_id_{-1};
    bool ready_{false};
    std::atomic<TaskState> state_{TaskState::kPending};
};

struct TerminateTask final : public Task {
    inline explicit
    TerminateTask() : Task(TaskType::kTerminate) {
        ready_ = true;
    }
};

struct DummyTask final : public Task {
    inline explicit
    DummyTask() : Task(TaskType::kDummy) {
        ready_ = true;
    }

    void
    Run(int64_t worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run dummy task by worker: %ld\n", worker_id);
        sleep(1);
    }
};

struct PipelineTask final : public Task {
    inline explicit
    PipelineTask() : Task(TaskType::kPipeline) {}

    inline void
    Init() {
        if(parents_.empty()) {
            root_task_ = true;
        } else {
            root_task_ = false;
        }
    }

    inline void
    AddSink(Sink* sink) {
        sink_ = sink;
    }

    inline void
    AddSource(Source* source, bool input_queue) {
        source_ = source;
        if(input_queue) {
            input_queue_ = std::make_unique<ConcurrentQueue>();
        }
    }

    inline void
    AddOperator(Operator* op) {
        operators_.emplace_back(op);
        buffers_.emplace_back(std::make_unique<Buffer>(BUFFER_SIZE));
    }

    inline void
    Run(int64_t worker_id) override {
        last_worker_id_ = worker_id;
//        printf("Run pipeline task by worker: %ld\n", worker_id);

        // Read data from source buffer or input queue
        source_buffer_ = std::make_shared<Buffer>(BUFFER_SIZE);
        source_->Run(input_queue_.get(), nullptr, source_buffer_);

        // process the data one by one operator and push to next operator
        size_t op_count = operators_.size();
        assert(op_count > 0);
        operators_[0]->Run(source_buffer_.get(), buffers_[0].get());
        for(size_t idx = 1; idx < op_count; ++idx) {
            operators_[idx]->Run(buffers_[idx - 1].get(), buffers_[idx].get());
        }

        // push the data into output queue
        sink_->Run(buffers_.back().get(), output_queues_);

        // put the parent task into scheduler
        for(Task* parent: parents_) {
//            printf("Notify parent to run\n");
            parent->set_state(TaskState::kReady);
//            NewScheduler::RunTask(parent);
        }

        if(root_task_) {
//            wait_flag_.notify_one();
//            printf("Notify result\n");
            std::unique_lock<std::mutex> lck(result_lk_);
            completed_ = true;
            result_cv_.notify_one();
        }
//        sleep(1);
    }

    inline void
    SetChildren(std::vector<std::shared_ptr<Task>> children) {
        children_ = std::move(children);
        for(const std::shared_ptr<Task>& child: children_) {
            PipelineTask* child_pipeline = (PipelineTask*)child.get();
            child_pipeline->AddOutputQueue(input_queue_.get());
            child_pipeline->AddParent(this);
        }
    }

    [[nodiscard]] inline const std::vector<std::shared_ptr<Task>>&
    children() const {
        return children_;
    }

    inline void
    GetResult() {
//        wait_flag_.wait(true);
        std::unique_lock<std::mutex> locker(result_lk_);
        result_cv_.wait(locker, [&] {
            return completed_;
        });
//        printf("Get result\n");
    }

private:
    inline void
    AddOutputQueue(ConcurrentQueue* queue) {
        output_queues_.emplace_back(queue);
    }

    inline void
    AddParent(Task* parent) {
        parents_.emplace_back(parent);
    }
private:
    Sink* sink_{};
    std::vector<ConcurrentQueue*> output_queues_;

    std::vector<Operator*> operators_{};
    std::vector<std::shared_ptr<Buffer>> buffers_{};

    Source* source_{};
    std::shared_ptr<Buffer> source_buffer_ = nullptr;
    // Wait-free queue
    std::unique_ptr<ConcurrentQueue> input_queue_{nullptr};

    std::vector<std::shared_ptr<Task>> children_{};
    std::vector<Task*> parents_{};

    bool root_task_{false};
    bool completed_{false};
    std::mutex result_lk_;
    std::condition_variable result_cv_;
//    std::atomic_bool wait_flag_{false};
};

}
