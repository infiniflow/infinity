//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "common/types/internal_types.h"
#include "operator.h"
#include <unistd.h>

namespace infinity {

enum class TaskType {
    kTerminate,
    kDummy,
    kPipeline,
    kSource,
    kSink,
    kInvalid,
};

struct Task {
    inline explicit
    Task(TaskType type) : type_(type) {}

    virtual void
    run(i64 worker_id) {
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
    SharedPtr<Task> child_{};
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
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run dummy task by worker: %ld\n", worker_id);
        sleep(1);
    }
};

struct PipelineTask final : public Task {
    inline explicit
    PipelineTask() : Task(TaskType::kPipeline) {}

    void
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run pipeline task by worker: %ld\n", worker_id);
        sleep(1);
    }
};

struct SourceTask final : public Task {
    inline explicit
    SourceTask(Source* source_op) : Task(TaskType::kSource), source_op_(source_op) {}

    void
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("run source task by worker: %ld\n", worker_id);
        source_op_->Run();
//        sleep(1);
    }

    Source* source_op_{};
};

struct SinkTask final : public Task {
    inline explicit
    SinkTask(Sink* sink_op) : Task(TaskType::kSink), sink_op_(sink_op) {}

    void
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run sink task by worker: %ld\n", worker_id);
        sleep(1);
    }
    Sink* sink_op_{};
};

}