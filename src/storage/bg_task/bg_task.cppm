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
import txn;
import catalog;
import catalog_delta_entry;
import buffer_manager;
import third_party;

namespace infinity {

export enum class BGTaskType {
    kStopProcessor,
    kAddDeltaEntry,
    kCheckpoint,
    kForceCheckpoint, // Manually triggered by PhysicalFlush
    kNotifyCompact,
    kNotifyOptimize,
    kCleanup,
    kUpdateSegmentBloomFilterData, // Not used
    kDumpIndex,
    kDumpIndexByline,
    kInvalid
};

class BaseMemIndex;
class ChunkIndexEntry;

export struct BGTask {
    BGTask(BGTaskType type, bool async) : type_(type), async_(async) {}

    virtual ~BGTask() = default;

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

    virtual String ToString() const = 0;
};

export struct StopProcessorTask final : public BGTask {
    StopProcessorTask() : BGTask(BGTaskType::kStopProcessor, false) {}

    ~StopProcessorTask() = default;

    String ToString() const final { return "Stop Task"; }
};

export struct AddDeltaEntryTask final : public BGTask {
    AddDeltaEntryTask(UniquePtr<CatalogDeltaEntry> delta_entry) : BGTask(BGTaskType::kAddDeltaEntry, false), delta_entry_(std::move(delta_entry)) {}

    String ToString() const final { return fmt::format("DeltaLog: {}", delta_entry_->ToString()); }

    UniquePtr<CatalogDeltaEntry> delta_entry_{};
};

export struct CheckpointTaskBase : public BGTask {
    CheckpointTaskBase(BGTaskType type, bool async) : BGTask(type, async) {}
};

export struct CheckpointTask final : public CheckpointTaskBase {
    CheckpointTask(bool full_checkpoint) : CheckpointTaskBase(BGTaskType::kCheckpoint, false), is_full_checkpoint_(full_checkpoint) {}

    String ToString() const final {
        if (is_full_checkpoint_) {
            return "Full checkpoint";
        } else {
            return "Delta checkpoint";
        }
    }

    bool is_full_checkpoint_{};
};

export struct ForceCheckpointTask final : public CheckpointTaskBase {
    explicit ForceCheckpointTask(Txn *txn, bool full_checkpoint = true, TxnTimeStamp cleanup_ts = 0)
        : CheckpointTaskBase(BGTaskType::kForceCheckpoint, false), txn_(txn), is_full_checkpoint_(full_checkpoint), cleanup_ts_(cleanup_ts) {}

    ~ForceCheckpointTask() = default;

    String ToString() const override {
        if (is_full_checkpoint_) {
            return fmt::format("Force full checkpoint, txn: ", txn_->TxnID());
        } else {
            return fmt::format("Force delta checkpoint, txn: ", txn_->TxnID());
        }
    }

    Txn *txn_{};
    bool is_full_checkpoint_{};
    TxnTimeStamp cleanup_ts_ = 0;
};

export class CleanupTask final : public BGTask {
public:
    // Try clean up is async task?
    CleanupTask(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr)
        : BGTask(BGTaskType::kCleanup, true), catalog_(catalog), visible_ts_(visible_ts), buffer_mgr_(buffer_mgr) {}

public:
    ~CleanupTask() override = default;

    String ToString() const override { return fmt::format("CleanupTask, visible timestamp: {}", visible_ts_); }

    void Execute();

private:
    Catalog *const catalog_;

    const TxnTimeStamp visible_ts_;

    BufferManager *buffer_mgr_;
};

export class NotifyCompactTask final : public BGTask {
public:
    NotifyCompactTask() : BGTask(BGTaskType::kNotifyCompact, true) {}

    ~NotifyCompactTask() override = default;

    String ToString() const override { return "NotifyCompactTask"; }
};

export class NotifyOptimizeTask final : public BGTask {
public:
    NotifyOptimizeTask() : BGTask(BGTaskType::kNotifyOptimize, true) {}

    ~NotifyOptimizeTask() override = default;

    String ToString() const override { return "NotifyOptimizeTask"; }
};

export class DumpIndexTask final : public BGTask {
public:
    DumpIndexTask(BaseMemIndex *mem_index, Txn *txn);

    ~DumpIndexTask() override = default;

    String ToString() const override { return "DumpIndexTask"; }

public:
    BaseMemIndex *mem_index_{};
    Txn *txn_{};
};

export class DumpIndexBylineTask final : public BGTask {
public:
    DumpIndexBylineTask(SharedPtr<String> db_name,
                        SharedPtr<String> table_name,
                        SharedPtr<String> index_name,
                        SegmentID segment_id,
                        SharedPtr<ChunkIndexEntry> dumped_chunk);

    ~DumpIndexBylineTask() override = default;

    String ToString() const override { return "DumpIndexBylineTask"; }

public:
    SharedPtr<String> db_name_;
    SharedPtr<String> table_name_;
    SharedPtr<String> index_name_;
    SegmentID segment_id_;
    SharedPtr<ChunkIndexEntry> dumped_chunk_;
};

} // namespace infinity
