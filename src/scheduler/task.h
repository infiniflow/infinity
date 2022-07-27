//
// Created by JinHai on 2022/7/28.
//

#pragma once

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


class Task : public std::enable_shared_from_this<Task> {
public:
    Task() = default;
    virtual ~Task() = 0;

    bool IsReady() const { return pending_predecessors_ == 0; }
    bool IsDone() const { return state_ == TaskState::kDone; }

    // Set dependencies
    void SetPredecessorOf(const std::shared_ptr<Task>& successor);
    const std::vector<std::weak_ptr<Task>>& predecessors() const { return predecessors_; }
    const std::vector<std::shared_ptr<Task>>& successors() const { return successors_; }

    void Schedule();
    void Execute();

    TaskState state() const { return state_; }

protected:
    virtual void OnExecute() = 0;

private:
    void OnPredecessorDone();

    // Task state
    TaskState state_{TaskState::kCreated};

    // Task dependencies
    std::atomic<uint64_t> pending_predecessors_{0};
    std::vector<std::weak_ptr<Task>> predecessors_;
    std::vector<std::shared_ptr<Task>> successors_;
};


}
