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

export class CompactionAlg {
public:
    virtual ~CompactionAlg() = default;

    virtual void AddInitSegments(Vector<SegmentEntry *> segment_entries) = 0;

    // Add a new segment, return the segments to be compacted, and the transaction to commit or rollback
    virtual Optional<Pair<Vector<SegmentEntry *>, Txn *>> AddSegment(SegmentEntry *new_segment, std::function<Txn *()> generate_txn) = 0;

    virtual Optional<Pair<Vector<SegmentEntry *>, Txn *>> DeleteInSegment(SegmentID segment_id, std::function<Txn *()> generate_txn) = 0;

    // After finish compaction, call this to add the compacted segment.
    // TODO: when compacting, some row may be deleted and triggered compaction condition again, ignroe it now
    virtual void CommitCompact(const Vector<SegmentEntry *> &new_segments, TransactionID commit_txn_id) = 0;

    // Rollback the compaction
    virtual void RollbackCompact(TransactionID rollback_txn_id) = 0;
};

} // namespace infinity
