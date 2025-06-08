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

export module bg_task;

import stl;
import third_party;
import global_resource_usage;
import status;
import bg_task_type;

namespace infinity {

struct MemIndex;
struct ColumnVector;
class BaseMemIndex;
struct ChunkIndexEntry;
class NewTxn;

export struct BGTask {
    BGTask(BGTaskType type, bool async) : type_(type), async_(async) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BGTask");
#endif
    }

    virtual ~BGTask() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BGTask");
#endif
    }

    BGTaskType type_{BGTaskType::kInvalid};
    bool async_{false};

    bool complete_{false};
    std::mutex mutex_{};
    std::condition_variable cv_{};

    void Wait() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        cv_.wait(locker, [this] { return complete_; });
    }

    void Complete() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        complete_ = true;
        cv_.notify_one();
    }

    Status result_status_{};

    virtual String ToString() const = 0;
};

export struct StopProcessorTask final : public BGTask {
    StopProcessorTask() : BGTask(BGTaskType::kStopProcessor, false) {}

    ~StopProcessorTask() = default;

    String ToString() const final { return "Stop Task"; }
};

export struct CheckpointTaskBase : public BGTask {
    CheckpointTaskBase(BGTaskType type, bool async) : BGTask(type, async) {}
};

export struct NewCheckpointTask final : public CheckpointTaskBase {
    NewCheckpointTask(i64 wal_size) : CheckpointTaskBase(BGTaskType::kNewCheckpoint, false), wal_size_(wal_size) {}

    String ToString() const final { return "New catalog"; }

    Status ExecuteWithinTxn();
    Status ExecuteWithNewTxn();

    NewTxn *new_txn_{};
    i64 wal_size_{};
};

export class NewCleanupTask final : public BGTask {
public:
    NewCleanupTask() : BGTask(BGTaskType::kNewCleanup, false) {}

    String ToString() const override { return "NewCleanupTask"; }

    Status Execute(TxnTimeStamp last_cleanup_ts, TxnTimeStamp &cur_cleanup_ts);

private:
};

export class NotifyCompactTask final : public BGTask {
public:
    NotifyCompactTask() : BGTask(BGTaskType::kNotifyCompact, true) {}

    ~NotifyCompactTask() override = default;

    String ToString() const override { return "NotifyCompactTask"; }
};

export class NewCompactTask final : public BGTask {
public:
    NewCompactTask(NewTxn *new_txn, String db_name, String table_name);

    String ToString() const override { return "NewCompactTask"; }

    NewTxn *new_txn_ = nullptr;
    String db_name_;
    String table_name_;
};

export class NotifyOptimizeTask final : public BGTask {
public:
    NotifyOptimizeTask(bool new_optimize = false) : BGTask(BGTaskType::kNotifyOptimize, true), new_optimize_(new_optimize) {}

    ~NotifyOptimizeTask() override = default;

    String ToString() const override { return "NotifyOptimizeTask"; }

public:
    bool new_optimize_ = false;
};

export class DumpIndexTask final : public BGTask {
public:
    DumpIndexTask(BaseMemIndex *mem_index, NewTxn *new_txn);

    ~DumpIndexTask() override = default;

    String ToString() const override { return "DumpIndexTask"; }

public:
    BaseMemIndex *mem_index_{};
    NewTxn *new_txn_{};
};

export class AppendMemIndexTask final : public BGTask {
public:
    AppendMemIndexTask(const SharedPtr<MemIndex> &mem_index, const SharedPtr<ColumnVector> &input_column, BlockOffset offset, BlockOffset row_cnt);

    ~AppendMemIndexTask() override = default;

    String ToString() const override { return "AppendMemIndexTask"; }

public:
    SharedPtr<MemIndex> mem_index_{};
    SharedPtr<ColumnVector> input_column_{};
    BlockOffset offset_{};
    BlockOffset row_cnt_{};
    u64 seq_inserted_{};
    u32 doc_count_{};
};

export struct AppendMemIndexBatch {
    void InsertTask(AppendMemIndexTask *);
    void WaitForCompletion();

    Vector<AppendMemIndexTask *> append_tasks_{};
    u64 task_count_{};
    mutable std::mutex mtx_{};
    std::condition_variable cv_{};
};

export class TestCommandTask final : public BGTask {
public:
    TestCommandTask(String command_content);

    ~TestCommandTask() override = default;

    String ToString() const override { return "TestCommandTask"; }

public:
    String command_content_{};
};

export struct BGTaskInfo {
    explicit BGTaskInfo(BGTaskType type);
    Vector<String> task_info_list_{};
    Vector<Status> status_list_{};
    BGTaskType type_{BGTaskType::kInvalid};
};

} // namespace infinity
