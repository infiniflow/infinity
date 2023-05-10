//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "common/types/internal_types.h"
#include "operator.h"
#include <unistd.h>
#include <cassert>

namespace infinity {

enum class TaskType {
    kTerminate,
    kDummy,
    kPipeline,
    kExchange,
    kSink,
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
    PipelineTask(Source *source) : Task(TaskType::kPipeline), source_(source) {}

    inline void
    AddOperator(Operator* op) {
        operators_.emplace_back(op);
        buffers_.emplace_back(128);
    }

    inline void
    Run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run pipeline task by worker: %ld\n", worker_id);
        sleep(1);
    }

    inline void
    SetChild(SharedPtr<Task> child) {
        child_ = std::move(child);
    }

    inline void
    SetParent(Task* parent) {
        parent_ = parent;
    }
private:
    Vector<Operator*> operators_{};
    Vector<Buffer> buffers_{};

    Source* source_;
    Buffer source_buffer_{128};

    // If the child is exchange node, it will be shared by many pipeline task
    SharedPtr<Task> child_{};
    Task* parent_{};
};

struct ExchangeTask final : public Task {
    inline explicit
    ExchangeTask(Source* source_op) : Task(TaskType::kExchange), source_op_(source_op) {}

    inline void
    Run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("run source task by worker: %ld\n", worker_id);
        source_op_->Run();
//        sleep(1);
    }


    inline void
    SetChild(UniquePtr<Task> child) {
        child_ = std::move(child);
    }

    inline void
    AddParent(Task* parent) {
        parents_.emplace_back(parent);
    }
private:
    Source* source_op_{};
    UniquePtr<Task> child_{};
    Vector<Task*> parents_{};
};

struct SinkTask final : public Task {
    inline explicit
    SinkTask(Sink* sink_op) : Task(TaskType::kSink), sink_op_(sink_op) {}

    inline void
    Run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run sink task by worker: %ld\n", worker_id);
        sleep(1);
    }

    inline void
    AddChild(UniquePtr<Task> child, u64 child_index) {
        if(child->type_ == TaskType::kPipeline) {
            // Pipeline task
            PipelineTask* pipeline_task = (PipelineTask*)(child.get());

            // Set sink_op input buffer as the output of pipeline task.

//            pipeline_task->SetOuputBuffer()
//            sink_op_->AddInput(pipeline_task->GetOutputBufferPtr());
        } else {
            printf("Unexpected task type");
            assert(false);
        }
        children_.emplace_back(std::move(child));
    }

    inline void
    SetParent(Task* parent) {
        parent_ = parent;
    }

private:
    Sink* sink_op_{};

    Vector<UniquePtr<Task>> children_{};
    Task* parent_{};
};

}