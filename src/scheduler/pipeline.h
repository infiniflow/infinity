//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "storage/table.h"

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

class QueryContext;
class Pipeline : public std::enable_shared_from_this<Pipeline> {
public:
    explicit
    Pipeline(u64 id) : id_(id) {}

    virtual
    ~Pipeline() = 0;

    bool
    IsReady() const {
        return pending_predecessors_ == 0;
    }

    bool
    IsDone() const {
        return state_ == TaskState::kDone;
    }

    // Set dependencies
    void
    SetPredecessorOf(const SharedPtr<Pipeline>& successor);

    const Vector<SharedPtr<Pipeline>>&
    predecessors() const {
        return predecessors_;
    }

    const Vector<Pipeline*>&
    successors() const {
        return successors_;
    }

    void
    Schedule();

    void
    Execute(QueryContext* query_context);

    TaskState
    state() const {
        return state_;
    }
    
    u64
    Id() const {
        return id_;
    }

    virtual SharedPtr<Table>
    GetResult() = 0;

protected:
    virtual void
    OnExecute(QueryContext* query_context) = 0;

private:
    void
    OnPredecessorDone();

    // Task state
    TaskState state_{TaskState::kCreated};

    // Task dependencies
    std::atomic<u64> pending_predecessors_{0};

    // TODO: leaf node will be freed due to weak ptr;
    Vector<SharedPtr<Pipeline>> predecessors_;

    Vector<Pipeline*> successors_;

    u64 id_{0};
};


}
