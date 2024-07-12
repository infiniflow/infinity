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

#include <cmath>
#include <utility>

export module DBT_compaction_alg;

import stl;
import segment_entry;
import infinity_exception;
import txn;
import compaction_alg;
import table_entry;
import logger;

namespace infinity {

class DBTConfig {
public:
    DBTConfig(SizeT m, SizeT c, SizeT s) : m_(m), c_(c), s_(s) {
        if (m <= 0 || c < m || s <= 0) {
            String error_message = "Invalid compaction parameters";
            UnrecoverableError(error_message);
        }
    }

    int CalculateLayer(SizeT row_cnt) const { return std::max(0, static_cast<int>(std::log(row_cnt / s_) / std::log(c_))); }

    SizeT Upperbound(int layer) const { return s_ * std::pow(c_, layer) - 1; }

    // SizeT LowerBound(int layer) const { return layer >= 1 ? s_ * std::pow(c_, layer - 1) : 0; }

    const SizeT m_; // the max node cnt in one layer
private:
    const SizeT c_; // the exponent of capacity in one layer
    const SizeT s_; // the size of the first layer
    // layer i has capacity: c^i * s, (i >= 0)
};

class SegmentLayer {
public:
    SegmentLayer() {}

    void AddSegment(SegmentEntry *segment_entry);

    void RemoveSegment(SegmentEntry *shrink_segment);

    Vector<SegmentEntry *> PickCompacting(TransactionID txn_id, SizeT M);

    void CommitCompact(TransactionID txn_id);

    void RollbackCompact(TransactionID txn_id);

    SizeT LayerSize() const { return segments_.size(); }

    SegmentEntry *FindSegment(SegmentID segment_id);

private:
    HashMap<SegmentID, SegmentEntry *> segments_;
    HashMap<TransactionID, Vector<SegmentEntry *>> compacting_segments_map_;
};

export class DBTCompactionAlg final : public CompactionAlg {
public:
    DBTCompactionAlg(int m, int c, int s, SizeT max_segment_capacity, TableEntry *table_entry = nullptr)
        : CompactionAlg(), config_(m, c, s), max_layer_(config_.CalculateLayer(max_segment_capacity)), table_entry_(table_entry), running_task_n_(0) {
    }

    virtual Vector<SegmentEntry *> CheckCompaction(TransactionID txn_id) override;

    virtual void AddSegment(SegmentEntry *new_segment) override;

    virtual void DeleteInSegment(SegmentID segment_id) override;

    virtual void CommitCompact(TransactionID commit_txn_id) override;

    virtual void RollbackCompact(TransactionID rollback_txn_id) override;

    virtual void Enable(const Vector<SegmentEntry *> &segment_entries) override;

    virtual void Disable() override;

private:
    // return layer
    int AddSegmentInner(SegmentEntry *new_segment);

    Pair<SegmentEntry *, int> FindSegmentAndLayer(SegmentID segment_id);

private:
    const DBTConfig config_;
    const int max_layer_;
    TableEntry *table_entry_;

    std::mutex mtx_;
    Vector<SegmentLayer> segment_layers_;

    std::condition_variable cv_;

    int running_task_n_;

    HashMap<TransactionID, i32> txn_2_layer_;
};

} // namespace infinity
