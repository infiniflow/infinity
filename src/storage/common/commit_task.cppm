// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
