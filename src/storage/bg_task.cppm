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
import parser;

export module bg_task;

namespace infinity {

export enum class BGTaskType {
    kTryCheckpoint,   // Periodically triggered by timer
    kForceCheckpoint, // Manually triggered by PhysicalImport
    kStopProcessor,
    kCatalogDeltaOpsMerge, // Merge
    kCompactSegments,
    kInvalid
};

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

export struct TryCheckpointTask final : public BGTask {
    explicit TryCheckpointTask(bool async) : BGTask(BGTaskType::kTryCheckpoint, async) {}

    ~TryCheckpointTask() = default;

    String ToString() const final { return "Checkpoint Task"; }
};

export struct ForceCheckpointTask final : public BGTask {
    explicit ForceCheckpointTask(Txn *txn) : BGTask(BGTaskType::kForceCheckpoint, false), txn_(txn) {}

    ~ForceCheckpointTask() = default;

    String ToString() const final { return "Force Checkpoint Task"; }

    Txn *txn_{};
};

export struct CatalogDeltaOpsMergeTask final : public BGTask {

    explicit CatalogDeltaOpsMergeTask(UniquePtr<CatalogDeltaEntry> local_catalog_delta_entry, NewCatalog *catalog)
        : BGTask(BGTaskType::kCatalogDeltaOpsMerge, true), local_catalog_delta_entry_(std::move(local_catalog_delta_entry)), catalog_(catalog) {}

    ~CatalogDeltaOpsMergeTask() = default;

    String ToString() const final { return "Catalog delta operation merge task"; }

    UniquePtr<CatalogDeltaEntry> local_catalog_delta_entry_{};
    NewCatalog *catalog_{};
};

export class CompactSegmentsTask final : public BGTask {
public:
    explicit CompactSegmentsTask(TableEntry *table_entry);

    String ToString() const override { return "Compact segments task"; }

    void Execute();

    void AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> block_row_hashmap);

private:
    SharedPtr<SegmentEntry> CompactSegments(Vector<SegmentID> segment_ids);

    void ApplyAllToDelete();

    void ApplyOneToDelete(SegmentID segment_id, const HashMap<BlockID, Vector<BlockOffset>> &to_delete);

private:
    TableEntry *const table_entry_{};
    const SegmentID max_segment_id_{};

    // A timestamp
    
    Vector<SegmentEntry *> segment_entries_;

    List<Pair<SegmentID, HashMap<BlockID, Vector<BlockOffset>>>> to_deletes_;

    std::mutex mtx_{};
};

} // namespace infinity
