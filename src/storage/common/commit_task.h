//
// Created by jinhai on 23-7-21.
//

#pragma once

#include <utility>


#include "common/types/data_type.h"
#include "storage/common/async_batch_processor.h"

namespace infinity {

class CommitTask final : public AsyncTask {
public:
    explicit
    CommitTask(SizeT size) : AsyncTask(AsyncTaskType::kCommit) {
        tasks_.reserve(size);
    }

    inline String
    ToString() override {
        return "CommitTask";
    }

    inline void
    Prepare() final {}

    inline void
    Commit() final {
        for(auto& task: tasks_) {
            task->Commit();
            task->Notify();
        }
    };

    void
    Append(AsyncTask* task) {
        tasks_.emplace_back(task);
    }

public:
    Vector<AsyncTask*> tasks_;
};

}
