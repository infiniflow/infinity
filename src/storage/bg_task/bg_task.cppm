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
import txn;
import catalog;
import catalog_delta_entry;
import cleanup_scanner;
import buffer_manager;

export module bg_task;

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
    kInvalid
};

export String BGTaskTypeToString(BGTaskType type) {
    switch (type) {
        case BGTaskType::kStopProcessor:
            return "StopProcessor";
        case BGTaskType::kForceCheckpoint:
            return "ForceCheckpoint";
        case BGTaskType::kAddDeltaEntry:
            return "AddDeltaEntry";
        case BGTaskType::kCheckpoint:
            return "Checkpoint";
        case BGTaskType::kNotifyCompact:
            return "NotifyCompact";
        case BGTaskType::kNotifyOptimize:
            return "NotifyOptimize";
        case BGTaskType::kCleanup:
            return "Cleanup";
        case BGTaskType::kUpdateSegmentBloomFilterData:
            return "UpdateSegmentBloomFilterData";
        default:
            return "Invalid";
    }
}

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
    AddDeltaEntryTask(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size)
        : BGTask(BGTaskType::kAddDeltaEntry, false), delta_entry_(std::move(delta_entry)), wal_size_(wal_size) {}

    String ToString() const final { return "Add Delta Entry Task"; }

    UniquePtr<CatalogDeltaEntry> delta_entry_{};
    i64 wal_size_{};
};

export struct CheckpointTaskBase : public BGTask {
    CheckpointTaskBase(BGTaskType type, bool async) : BGTask(type, async) {}
};

export struct CheckpointTask final : public CheckpointTaskBase {
    CheckpointTask(bool full_checkpoint) : CheckpointTaskBase(BGTaskType::kCheckpoint, false), is_full_checkpoint_(full_checkpoint) {}

    String ToString() const final { return "Checkpoint Task"; }
    bool is_full_checkpoint_{};
};

export struct ForceCheckpointTask final : public CheckpointTaskBase {
    explicit ForceCheckpointTask(Txn *txn, bool full_checkpoint = true)
        : CheckpointTaskBase(BGTaskType::kForceCheckpoint, false), txn_(txn), is_full_checkpoint_(full_checkpoint) {}

    ~ForceCheckpointTask() = default;

    String ToString() const override { return "Force Checkpoint Task"; }

    Txn *txn_{};
    bool is_full_checkpoint_{};
};

export class CleanupTask final : public BGTask {
public:
    // Try clean up is async task?
    CleanupTask(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr)
        : BGTask(BGTaskType::kCleanup, true), catalog_(catalog), visible_ts_(visible_ts), buffer_mgr_(buffer_mgr) {}

public:
    ~CleanupTask() override = default;

    String ToString() const override { return "CleanupTask"; }

    void Execute() {
        CleanupScanner scanner(catalog_, visible_ts_, buffer_mgr_);
        scanner.Scan();
        // FIXME(sys): This is a blocking call, should it be async?
        std::move(scanner).Cleanup();
    }

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

} // namespace infinity
