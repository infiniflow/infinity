//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "common/types/internal_types.h"
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
};

struct TerminateTask final : public Task {
    inline explicit
    TerminateTask() : Task(TaskType::kTerminate) {}
};

struct DummyTask final : public Task {
    inline explicit
    DummyTask() : Task(TaskType::kDummy) {}

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
    SourceTask() : Task(TaskType::kSource) {}

    void
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run source task by worker: %ld\n", worker_id);
        sleep(1);
    }
};

struct SinkTask final : public Task {
    inline explicit
    SinkTask() : Task(TaskType::kSink) {}

    void
    run(i64 worker_id) override {
        last_worker_id_ = worker_id;
        printf("Run sink task by worker: %ld\n", worker_id);
        sleep(1);
    }
};

}