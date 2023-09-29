//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "common/types/internal_types.h"
#include "operator.h"
#include "buffer_queue.h"
#include "task_queue.h"
#include <unistd.h>
#include <cassert>

namespace infinity {

class Task;

class NewScheduler {
public:
    static void
    Init(const HashSet<i64>& cpu_set);

    static void
    Uninit();

    static void
    RunTask(Task* task);

    inline static void
    DispatchTask(i64 worker_id, Task* task) {
        task_queues[worker_id]->Enqueue(task);
    }
private:
    static void
    CoordinatorLoop(i64 cpu_id);

    static void
    WorkerLoop(BlockingQueue* task_queue, i64 worker_id);

    static i64
    GetAvailableCPU();

private:
    static HashSet<i64> cpu_set;

    static HashMap<i64, UniquePtr<BlockingQueue>> task_queues;
    static HashMap<i64, UniquePtr<Thread>> workers;

    static UniquePtr<BlockingQueue> input_queue;
    static UniquePtr<Thread> coordinator;

    static Vector<i64> cpu_array;
    static u64 current_cpu_id;
};


#define BUFFER_SIZE 128

enum class TaskType {
    kTerminate,
    kDummy,
    kPipeline,
    kInvalid,
};

struct Task {
    inline explicit
    Task(TaskType type) : type_(type) {}

    virtual void
    Run(i64 worker_id) {
        // Not implemented
        last_worker_id_ = worker_id;
    }

    [[nodiscard]] inline TaskType
    type() const {
        return type_;
    }

    TaskType type_{TaskType::kInvalid};
    i64 last_worker_id_{-1};
    bool ready_{false};
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
    Run(i64 worker_id) override {
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
            input_queue_ = MakeUnique<ConcurrentQueue>();
        }
    }

    inline void
    AddOperator(Operator* op) {
        operators_.emplace_back(op);
        buffers_.emplace_back(MakeUnique<Buffer>(BUFFER_SIZE));
    }

    inline void
    Run(i64 worker_id) override {
        last_worker_id_ = worker_id;
//        printf("Run pipeline task by worker: %ld\n", worker_id);

        // Read data from source buffer or input queue
        if(input_queue_ == nullptr) {
            String id_str = std::to_string(worker_id);
            source_buffer_ = MakeShared<Buffer>(BUFFER_SIZE);
            source_buffer_->Append(id_str.c_str());
//            memcpy((void*)(source_buffer_.get()), id_str.c_str(), id_str.size());
        } else {
//            printf("Get data from input queue\n");
            input_queue_->TryDequeue(source_buffer_);
        }

        // process the data one by one operator and push to next operator
        SizeT op_count = operators_.size();
        assert(op_count > 0);
        operators_[0]->Run(source_buffer_.get(), buffers_[0].get());
        for(SizeT idx = 1; idx < op_count; ++idx) {
            operators_[idx]->Run(buffers_[idx - 1].get(), buffers_[idx].get());
        }

        // push the data into output queue
        sink_->Run(buffers_.back().get(), output_queues_);

        // put the parent task into scheduler
        for(Task* parent: parents_) {
//            printf("Notify parent to run\n");
            NewScheduler::RunTask(parent);
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
    SetChildren(Vector<SharedPtr<Task>> children) {
        children_ = std::move(children);
        for(const SharedPtr<Task>& child: children_) {
            PipelineTask* child_pipeline = (PipelineTask*)child.get();
            child_pipeline->AddOutputQueue(input_queue_.get());
            child_pipeline->AddParent(this);
        }
    }

    [[nodiscard]] inline const Vector<SharedPtr<Task>>&
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
    Vector<ConcurrentQueue*> output_queues_;

    Vector<Operator*> operators_{};
    Vector<SharedPtr<Buffer>> buffers_{};

    Source* source_{};
    SharedPtr<Buffer> source_buffer_ = nullptr;
    // Wait-free queue
    UniquePtr<ConcurrentQueue> input_queue_{nullptr};

    Vector<SharedPtr<Task>> children_{};
    Vector<Task*> parents_{};

    bool root_task_{false};
    bool completed_{false};
    std::mutex result_lk_;
    std::condition_variable result_cv_;
//    std::atomic_bool wait_flag_{false};
};

}
