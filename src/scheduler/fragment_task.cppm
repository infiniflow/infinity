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

export module infinity_core:fragment_task;

import :stl;
import :profiler;
import :operator_state;
import global_resource_usage;

namespace infinity {

export class FragmentContext;

export enum class FragmentTaskStatus : i8 {
    kPending,
    kRunning,
    kFinished,
    kError,
};

export String FragmentTaskStatus2String(FragmentTaskStatus status) {
    switch (status) {
        case FragmentTaskStatus::kPending:
            return String("Pending");
        case FragmentTaskStatus::kRunning:
            return String("Running");
        case FragmentTaskStatus::kFinished:
            return String("Finished");
        case FragmentTaskStatus::kError:
            return String("Error");
    }
}

export class FragmentTask {
public:
    explicit FragmentTask(bool terminator = true) : is_terminator_(terminator) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("FragmentTask");
#endif
    }

    explicit FragmentTask(void *fragment_context, i64 task_id, i64 operator_count)
        : fragment_context_(fragment_context), task_id_(task_id), operator_count_(operator_count) {
        Init();
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("FragmentTask");
#endif
    }

    ~FragmentTask() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("FragmentTask");
#endif
    }

    [[nodiscard]] inline bool IsTerminator() const { return is_terminator_; }

    void Init();

    void OnExecute();

    inline void SetLastWorkID(i64 worker_id) { last_worker_id_ = worker_id; }

    [[nodiscard]] inline i64 LastWorkerID() const { return last_worker_id_; }

    u64 FragmentId() const;

    [[nodiscard]] inline i64 TaskID() const { return task_id_; }

    [[nodiscard]] bool IsComplete();

    // TryInto and QuitFrom race on `status_`
    bool TryIntoWorkerLoop();

    bool QuitFromWorkerLoop();

    [[nodiscard]] TaskBinding TaskBinding() const;

    bool CompleteTask();

    String PhysOpsToString();

    // for test.
    [[nodiscard]] inline FragmentTaskStatus status() const { return status_; }

    FragmentContext *fragment_context() const;

public:
    UniquePtr<SourceState> source_state_{};

    Vector<UniquePtr<OperatorState>> operator_states_{};

    UniquePtr<SinkState> sink_state_{};

private:
    std::mutex mutex_;

    FragmentTaskStatus status_{FragmentTaskStatus::kPending};

    void *fragment_context_{};
    bool is_terminator_{false};
    i64 last_worker_id_{-1};
    i64 task_id_{-1};
    i64 operator_count_{0};
};

} // namespace infinity
