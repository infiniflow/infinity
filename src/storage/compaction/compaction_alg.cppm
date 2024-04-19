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

export module compaction_alg;

import stl;
import segment_entry;
import txn;

namespace infinity {

export enum class CompactionStatus : u8 {
    kDisable,
    kEnable,
    kRunning,
};

/*
    This is the algorithm to select segments to compact.
    When booting the system, call init to construct the algorithm structure
    After a new segment is committed, call `AddSegment` to check whether a compaction is needed
    If a compaction is needed, DO IT.
    Call `CommitCompact` after compaction is done.
    Or the compaction failed, call `RollbackCompact` to restore the structure.

    `AddSegment` and `DeleteInSegment` is called when adding segments or delete rows in commit thread
    `CommitCompact` and `RollbackCompact` is called in background thread
    So lock is needed
*/

export struct CompactionInfo {
public:
    CompactionInfo(Vector<SegmentEntry *> &&segments, Txn *txn) : segments_(std::move(segments)), txn_(txn) {}

    Vector<SegmentEntry *> segments_;
    Txn *txn_;
};

export class CompactionAlg {
public:
    CompactionAlg() : status_(CompactionStatus::kDisable) {}

public:
    virtual ~CompactionAlg() = default;

    virtual Optional<CompactionInfo> CheckCompaction(std::function<Txn *()> generate_txn) = 0;

    virtual void AddSegment(SegmentEntry *new_segment) = 0;

    virtual void DeleteInSegment(SegmentID segment_id) = 0;

    // After finish compaction, call this to add the compacted segment.
    // TODO: when compacting, some row may be deleted and triggered compaction condition again, ignroe it now
    virtual void CommitCompact(TransactionID commit_txn_id) = 0;

    // Rollback the compaction
    virtual void RollbackCompact(TransactionID rollback_txn_id) = 0;

    // ReInitialize segments and enable auto compaction
    virtual void Enable(const Vector<SegmentEntry *> &segment_entries) = 0;

    // Wait for all compaction to finish and disable auto compaction
    virtual void Disable() = 0;

    CompactionStatus status() const { return status_; }

protected:
    CompactionStatus status_;
};

} // namespace infinity
