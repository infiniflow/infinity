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

export module infinity_core:bg_task;

import :status;
import :bg_task_type;

import std;
import third_party;

import row_id;
import global_resource_usage;

namespace infinity {

struct MemIndex;
struct ColumnVector;
class BaseMemIndex;
class EMVBIndexInMem;
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
    mutable std::mutex mutex_{};
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

    bool IsComplete() const {
        std::unique_lock<std::mutex> locker(mutex_);
        return complete_;
    }

    Status result_status_{};

    virtual std::string ToString() const = 0;
};

export struct StopProcessorTask final : public BGTask {
    StopProcessorTask() : BGTask(BGTaskType::kStopProcessor, false) {}

    ~StopProcessorTask() = default;

    std::string ToString() const final { return "Stop Task"; }
};

export struct CheckpointTaskBase : public BGTask {
    CheckpointTaskBase(BGTaskType type, bool async) : BGTask(type, async) {}
};

export struct NewCheckpointTask final : public CheckpointTaskBase {
    NewCheckpointTask(i64 wal_size) : CheckpointTaskBase(BGTaskType::kCheckpoint, false), wal_size_(wal_size) {}

    std::string ToString() const final { return "New catalog"; }

    Status ExecuteWithinTxn();
    Status ExecuteWithNewTxn();

    NewTxn *new_txn_{};
    i64 wal_size_{};
};

export class CleanupTask final : public BGTask {
public:
    CleanupTask() : BGTask(BGTaskType::kCleanup, false) {}

    std::string ToString() const override { return "CleanupTask"; }

    Status Execute(TxnTimeStamp last_cleanup_ts, TxnTimeStamp &cur_cleanup_ts);

private:
};

export class NotifyCompactTask final : public BGTask {
public:
    NotifyCompactTask() : BGTask(BGTaskType::kNotifyCompact, true) {}

    ~NotifyCompactTask() override = default;

    std::string ToString() const override { return "NotifyCompactTask"; }
};

export class NewCompactTask final : public BGTask {
public:
    NewCompactTask(NewTxn *new_txn, std::string db_name, std::string table_name);

    std::string ToString() const override { return "NewCompactTask"; }

    NewTxn *new_txn_ = nullptr;
    std::string db_name_;
    std::string table_name_;
};

export class NotifyOptimizeTask final : public BGTask {
public:
    NotifyOptimizeTask(bool new_optimize = false) : BGTask(BGTaskType::kNotifyOptimize, true) {}

    ~NotifyOptimizeTask() override = default;

    std::string ToString() const override { return "NotifyOptimizeTask"; }
};

export class DumpMemIndexTask final : public BGTask {
public:
    DumpMemIndexTask(const std::string &db_name,
                     const std::string &table_name,
                     const std::string &index_name,
                     SegmentID segment_id,
                     RowID begin_row_id = RowID());

    ~DumpMemIndexTask() override = default;

    std::string ToString() const override {
        return fmt::format("DumpMemIndexTask: db_name={}, table_name={}, index_name={}, segment_id={}, begin_row_id=({},{})",
                           db_name_,
                           table_name_,
                           index_name_,
                           segment_id_,
                           begin_row_id_.segment_id_,
                           begin_row_id_.segment_offset_);
    }

public:
    std::string db_name_{};
    std::string table_name_{};
    std::string index_name_{};
    SegmentID segment_id_{};
    RowID begin_row_id_{};
};

export class AppendMemIndexTask final : public BGTask {
public:
    AppendMemIndexTask(const std::shared_ptr<MemIndex> &mem_index,
                       const std::shared_ptr<ColumnVector> &input_column,
                       BlockOffset offset,
                       BlockOffset row_cnt);

    ~AppendMemIndexTask() override = default;

    std::string ToString() const override { return "AppendMemIndexTask"; }

public:
    std::shared_ptr<MemIndex> mem_index_{};
    std::shared_ptr<ColumnVector> input_column_{};
    BlockOffset offset_{};
    BlockOffset row_cnt_{};
    u64 seq_inserted_{};
    u32 doc_count_{};
};

export struct AppendMemIndexBatch {
    void InsertTask(AppendMemIndexTask *);
    void WaitForCompletion();

    std::vector<AppendMemIndexTask *> append_tasks_{};
    u64 task_count_{};
    mutable std::mutex mtx_{};
    std::condition_variable cv_{};
};

export class TestCommandTask final : public BGTask {
public:
    TestCommandTask(std::string command_content);

    ~TestCommandTask() override = default;

    std::string ToString() const override { return "TestCommandTask"; }

public:
    std::string command_content_{};
};

export struct BGTaskInfo {
    explicit BGTaskInfo(BGTaskType type);
    std::vector<std::string> task_info_list_{};
    std::vector<std::string> status_list_{};
    BGTaskType type_{BGTaskType::kInvalid};
    std::chrono::system_clock::time_point task_time_{};
};

} // namespace infinity
