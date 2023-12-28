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
import profiler;
import operator_state;

export module fragment_task;

namespace infinity {

class FragmentContext;

// Task type:
// DDL task
// DML task
// Query task
// Read data from queue or file system

enum class FragmentSourceType {
    kNone,
    kScan,
    kQueue,
};

enum class FragmentSinkType {
    kGlobalMaterialize,
    kLocalMaterialize,
    kStream,
};

enum class FragmentTaskStatus {
    kRunning,
    kCancelled,
    kFinished,
    kReady,
    kPending,
    kError,
};

export class FragmentTask {
public:
    explicit FragmentTask(bool terminator = true) : is_terminator_(terminator) {}

    explicit FragmentTask(void *fragment_context, i64 task_id, i64 operator_count)
        : fragment_context_(fragment_context), task_id_(task_id), operator_count_(operator_count) {
        Init();
    }

    inline void SetTerminator() { is_terminator_ = true; }

    [[nodiscard]] inline bool IsTerminator() const { return is_terminator_; }

    void Init();

    void OnExecute(i64 worker_id);

    inline void SetLastWorkID(i64 worker_id) { last_worker_id_ = worker_id; }

    // [[nodiscard]] inline i64 LastWorkerID() const { return last_worker_id_; }

    u64 FragmentId() const;

    [[nodiscard]] inline i64 TaskID() const { return task_id_; }

    [[nodiscard]] bool Ready() const;

    [[nodiscard]] bool IsComplete() const;

    [[nodiscard]] TaskBinding TaskBinding() const;

    void CompleteTask();

    String PhysOpsToString();

    inline void set_status(FragmentTaskStatus new_status) { status_ = new_status; }

    [[nodiscard]] inline FragmentTaskStatus status() const { return status_; }

    FragmentContext *fragment_context() const;

public:
    FragmentTaskStatus status_{FragmentTaskStatus::kReady};

    UniquePtr<SourceState> source_state_{};

    Vector<UniquePtr<OperatorState>> operator_states_{};

    UniquePtr<SinkState> sink_state_{};

private:
    void *fragment_context_{};
    bool is_terminator_{false};
    [[maybe_unused]] i64 last_worker_id_{-1};
    i64 task_id_{-1};
    i64 operator_count_{0};
};

} // namespace infinity
