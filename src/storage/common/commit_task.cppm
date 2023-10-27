//
// Created by jinhai on 23-7-21.
//

module;

import stl;
import async_task;

export module commit_task;

namespace infinity {

export class CommitTask final : public AsyncTask {
public:
    explicit CommitTask(SizeT size) : AsyncTask(AsyncTaskType::kCommit) { tasks_.reserve(size); }

    inline String ToString() override { return "CommitTask"; }

    inline void Prepare() final {}

    inline void Commit() final {
        SizeT task_count = tasks_.size();
        for(SizeT task_idx = 0; task_idx < task_count; ++ task_idx) {
            auto &task = tasks_[task_idx];
            task->Commit();
            task->Notify();
        }
    };

    void Append(AsyncTask *task) { tasks_.emplace_back(task); }

public:
    Vector<AsyncTask *> tasks_;
};

} // namespace infinity
