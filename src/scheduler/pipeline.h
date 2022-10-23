//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "storage/table.h"
#include "main/query_context.h"

#include <atomic>
#include <memory>
#include <vector>

enum class TaskState {
    kCreated,
    kScheduled,
    kStarted,
    kDone,
};

namespace infinity {


class Pipeline : public std::enable_shared_from_this<Pipeline> {
public:
    explicit Pipeline(uint64_t id) : id_(id) {}
    virtual ~Pipeline() = 0;

    bool IsReady() const { return pending_predecessors_ == 0; }
    bool IsDone() const { return state_ == TaskState::kDone; }

    // Set dependencies
    void SetPredecessorOf(const std::shared_ptr<Pipeline>& successor);
    const std::vector<std::weak_ptr<Pipeline>>& predecessors() const { return predecessors_; }
    const std::vector<std::shared_ptr<Pipeline>>& successors() const { return successors_; }

    void Schedule();
    void Execute(std::shared_ptr<QueryContext>& query_context);

    TaskState state() const { return state_; }
    uint64_t Id() const { return id_; }
    virtual std::shared_ptr<Table> GetResult() = 0;

protected:
    virtual void
    OnExecute(std::shared_ptr<QueryContext>& query_context) = 0;

private:
    void OnPredecessorDone();

    // Task state
    TaskState state_{TaskState::kCreated};

    // Task dependencies
    std::atomic<uint64_t> pending_predecessors_{0};
    std::vector<std::weak_ptr<Pipeline>> predecessors_;
    std::vector<std::shared_ptr<Pipeline>> successors_;

    uint64_t id_{0};
};


}
