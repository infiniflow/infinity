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

#include <utility>
#include <vector>

module DBT_compaction_alg;

import stl;
import segment_entry;
import infinity_exception;
import txn;
import compaction_alg;

namespace infinity {

Vector<SegmentEntry *> SegmentLayer::SetAllCompacting(TransactionID txn_id) {
    Vector<SegmentEntry *> ret = segments_; // copy here
    compacting_segments_map_.emplace(txn_id, std::move(segments_));
    return ret;
}

void SegmentLayer::SetOneCompacting(SegmentEntry *segment_entry, TransactionID txn_id) {
    SegmentID segment_id = segment_entry->segment_id();
    auto erase_begin = std::remove_if(segments_.begin(), segments_.end(), [&](SegmentEntry *segment) { return segment->segment_id() == segment_id; });
    if (segments_.end() - erase_begin != 1) {
        UnrecoverableError("Segment not found in layer");
    }
    segments_.erase(erase_begin, segments_.end());
    compacting_segments_map_.emplace(txn_id, Vector<SegmentEntry *>{segment_entry});
}

void SegmentLayer::RollbackCompact(TransactionID txn_id) {
    auto range = compacting_segments_map_.equal_range(txn_id);
    for (auto iter = range.first; iter != range.second; ++iter) {
        auto &compacting_segments = iter->second;
        for (auto *rollback_segment : compacting_segments) {
            AddSegmentInfo(rollback_segment);
        }
    }
    compacting_segments_map_.erase(txn_id);
}

void DBTCompactionAlg::AddInitSegments(Vector<SegmentEntry *> segment_entries) {
    for (auto *segment : segment_entries) {
        if (!segment->sealed()) {
            UnrecoverableError("Segment must be sealed before compaction");
        }
        AddSegmentInner(segment); // TODO: check if the segment will trigger compaction
    }
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> DBTCompactionAlg::AddSegment(SegmentEntry *new_segment, StdFunction<Txn *()> generate_txn) {
    std::unique_lock lock(mtx_);
    auto layer_opt = AddSegmentInner(new_segment);
    if (!layer_opt.has_value()) {
        return None;
    }
    int layer = *layer_opt;

    Txn *txn = generate_txn();
    TransactionID txn_id = txn->TxnID();

    Vector<SegmentEntry *> compact_segments = segment_layers_[layer].SetAllCompacting(txn_id);
    AddSegmentToHigher(compact_segments, layer + 1, txn_id);

    if (compact_segments.size() <= 1) {
        UnrecoverableError("Algorithm bug.");
    }
    return MakePair(std::move(compact_segments), std::move(txn)); // FIXME: MakePair is implemented incorrectly
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> DBTCompactionAlg::DeleteInSegment(SegmentID segment_id, StdFunction<Txn *()> generate_txn) {
    std::unique_lock lock(mtx_);
    auto iter = segment_layer_map_.find(segment_id);
    if (iter == segment_layer_map_.end()) { // the segment has been compacted and not committed
        return None;
    }
    auto [shrink_segment, old_layer] = iter->second;
    SegmentOffset new_row_cnt = shrink_segment->actual_row_count();
    int new_layer = config_.CalculateLayer(new_row_cnt);
    if (old_layer == new_layer) {
        return None;
    }
    if (new_layer >= old_layer) {
        UnrecoverableError("Shrink segment has less rows than before");
    }

    Txn *txn = generate_txn();
    TransactionID txn_id = txn->TxnID();

    SegmentLayer &old_segment_layer = segment_layers_[old_layer];
    old_segment_layer.SetOneCompacting(shrink_segment, txn_id);

    Vector<SegmentEntry *> compact_segments{shrink_segment};
    AddSegmentToHigher(compact_segments, new_layer, txn_id);
    lock.unlock();

    if (compact_segments.size() == 1) {
        this->CommitCompact(compact_segments, txn_id);
        return MakePair(Vector<SegmentEntry *>(), std::move(txn));
    } else if (compact_segments.empty()) {
        UnrecoverableError("Algorithm bug.");
    }
    return MakePair(std::move(compact_segments), std::move(txn)); // FIXME: MakePair is implemented incorrectly
}

void DBTCompactionAlg::CommitCompact(const Vector<SegmentEntry *> &new_segments, TransactionID commit_txn_id) {
    std::unique_lock lock(mtx_);
    for (auto &segment_layer : segment_layers_) {
        segment_layer.CommitCompact(commit_txn_id);
    }
    for (auto *new_segment : new_segments) {
        AddSegmentInner(new_segment);
    }
}

void DBTCompactionAlg::RollbackCompact(TransactionID rollback_txn_id) {
    std::unique_lock lock(mtx_);
    for (auto &segment_layer : segment_layers_) {
        segment_layer.RollbackCompact(rollback_txn_id);
    }
}

// Add the segment in layer, return the layer if it triggers compaction
Optional<int> DBTCompactionAlg::AddSegmentInner(SegmentEntry *new_segment) {
    SegmentOffset new_row_cnt = new_segment->actual_row_count();
    int layer = config_.CalculateLayer(new_row_cnt);
    if (layer >= (int)segment_layers_.size()) {
        segment_layers_.resize(layer + 1);
    }
    SegmentLayer &segment_layer = segment_layers_[layer];
    segment_layer_map_.emplace(new_segment->segment_id(), Pair<SegmentEntry *, int>{new_segment, layer});
    segment_layer.AddSegmentInfo(new_segment);

    // Now: prohibit the top layer to merge
    // TODO: merge the top layer if possible
    if (layer == max_layer_ || segment_layer.LayerSize() < config_.m_) {
        return None;
    }
    return layer;
}

void DBTCompactionAlg::AddSegmentToHigher(Vector<SegmentEntry *> &compact_segments, int layer, TransactionID txn_id) {
    while (true) {
        if (layer >= (int)segment_layers_.size()) {
            segment_layers_.emplace_back();
        }
        SegmentLayer &segment_layer = segment_layers_[layer];
        if (segment_layer.LayerSize() + 1 < config_.m_) {
            break;
        }
        auto compact_segments1 = segment_layer.SetAllCompacting(txn_id);
        compact_segments.insert(compact_segments.end(), compact_segments1.begin(), compact_segments1.end());
        ++layer;
    }
}

} // namespace infinity
