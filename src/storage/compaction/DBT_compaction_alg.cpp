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
import third_party;
import logger;
import table_entry;

namespace infinity {

Vector<SegmentEntry *> SegmentLayer::SetAllCompacting(TransactionID txn_id) {
    Vector<SegmentEntry *> ret = segments_;                         // copy here
    compacting_segments_map_.emplace(txn_id, std::move(segments_)); // move makes segment_ empty
    return ret;
}

void SegmentLayer::SetOneCompacting(SegmentEntry *segment_entry, TransactionID txn_id) {
    SegmentID segment_id = segment_entry->segment_id();
    auto erase_begin = std::remove_if(segments_.begin(), segments_.end(), [&](SegmentEntry *segment) { return segment->segment_id() == segment_id; });
    if (int find_n = segments_.end() - erase_begin; find_n != 1) {
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

SegmentEntry *SegmentLayer::FindSegment(SegmentID segment_id) {
    SegmentEntry *segment = nullptr;
    for (auto *segment : segments_) {
        if (segment->segment_id() == segment_id) {
            return segment;
        }
    }
    return segment;
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> DBTCompactionAlg::AddSegment(SegmentEntry *new_segment, std::function<Txn *()> generate_txn) {
    std::unique_lock lock(mtx_);
    int layer = AddSegmentNoCheckInner(new_segment);
    // Now: prohibit the top layer to merge
    // TODO: merge the top layer if possible
    if (layer == max_layer_ || segment_layers_[layer].LayerSize() < config_.m_) {
        return None;
    }
    if (status_ == CompactionStatus::kDisable) {
        // If is disable, manual compaction is going
        // new segment will be add after manual compaction is committed/rollback
        return None;
    }
    // trigger compaction
    if (++running_task_n_ == 1) {
        status_ = CompactionStatus::kRunning;
    }

    Txn *txn = generate_txn();
    TransactionID txn_id = txn->TxnID();

    Vector<SegmentEntry *> compact_segments = segment_layers_[layer].SetAllCompacting(txn_id);
    AddSegmentToHigher(compact_segments, layer + 1, txn_id);

    if (compact_segments.size() <= 1) {
        UnrecoverableError("Algorithm bug.");
    }
    return MakePair(std::move(compact_segments), std::move(txn)); // FIXME: MakePair is implemented incorrectly
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> DBTCompactionAlg::DeleteInSegment(SegmentID segment_id, std::function<Txn *()> generate_txn) {
    std::unique_lock lock(mtx_);

    auto [shrink_segment, old_layer] = FindSegmentAndLayer(segment_id);
    if (shrink_segment == nullptr) {
        return None; // this segment is compacting, ignore it
    }
    SegmentOffset new_row_cnt = shrink_segment->actual_row_count();
    int new_layer = config_.CalculateLayer(new_row_cnt);
    if (old_layer == new_layer) {
        return None;
    }
    if (status_ == CompactionStatus::kDisable) {
        // If is disable, manual compaction is going
        return None;
    }
    // trigger compaction
    if (++running_task_n_ == 1) {
        status_ = CompactionStatus::kRunning;
    }

    if (new_layer >= old_layer) {
        UnrecoverableError("Shrink segment should has less rows than before");
    }

    Txn *txn = generate_txn();
    TransactionID txn_id = txn->TxnID();

    SegmentLayer &old_segment_layer = segment_layers_[old_layer];
    old_segment_layer.SetOneCompacting(shrink_segment, txn_id);

    Vector<SegmentEntry *> compact_segments{shrink_segment};
    AddSegmentToHigher(compact_segments, new_layer, txn_id);
    lock.unlock();

    if (compact_segments.empty()) {
        UnrecoverableError("Algorithm bug.");
    }
    return MakePair(std::move(compact_segments), std::move(txn)); // FIXME: MakePair is implemented incorrectly
}

void DBTCompactionAlg::CommitCompact(TransactionID commit_txn_id) {
    std::unique_lock lock(mtx_);
    if (status_ != CompactionStatus::kRunning) {
        UnrecoverableError(fmt::format("Wrong status of compaction alg: {}", (u8)status_));
    }

    for (auto &segment_layer : segment_layers_) {
        segment_layer.CommitCompact(commit_txn_id);
    }
    if (--running_task_n_ == 0) {
        status_ = CompactionStatus::kEnable;
        cv_.notify_one();
    }
}

void DBTCompactionAlg::RollbackCompact(TransactionID rollback_txn_id) {
    std::unique_lock lock(mtx_);
    if (status_ != CompactionStatus::kRunning) {
        UnrecoverableError(fmt::format("Rollback compact when compaction not running, {}", (u8)status_));
    }

    for (auto &segment_layer : segment_layers_) {
        segment_layer.RollbackCompact(rollback_txn_id);
    }
    if (--running_task_n_ == 0) {
        status_ = CompactionStatus::kEnable;
    }
}

int DBTCompactionAlg::AddSegmentNoCheckInner(SegmentEntry *new_segment) {
    SegmentOffset new_row_cnt = new_segment->actual_row_count();
    int layer = config_.CalculateLayer(new_row_cnt);
    if (layer >= (int)segment_layers_.size()) {
        segment_layers_.resize(layer + 1);
    }
    SegmentLayer &segment_layer = segment_layers_[layer];
    // segment_layer_map_.emplace(new_segment->segment_id(), Pair<SegmentEntry *, int>{new_segment, layer});
    segment_layer.AddSegmentInfo(new_segment);
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

// FIXME: use brute force to find the segment, for simple reason.
// NOTE: When implement a more efficient way, such as hashmap, you should rollback the hashmap when rollback compact
Pair<SegmentEntry *, int> DBTCompactionAlg::FindSegmentAndLayer(SegmentID segment_id) {
    for (int layer = 0; layer < (int)segment_layers_.size(); ++layer) {
        auto &segment_layer = segment_layers_[layer];
        if (auto segment = segment_layer.FindSegment(segment_id); segment != nullptr) {
            return std::make_pair(segment, layer);
        }
    }
    // the may happen because the segment is compacting. Ignore it
    // FIXME: do something
    return {nullptr, -1};
}

// Must be called when all segments are not compacting
void DBTCompactionAlg::Enable(const Vector<SegmentEntry *> &segment_entries) {
    std::unique_lock lock(mtx_);
    if (status_ != CompactionStatus::kDisable) {
        UnrecoverableError(fmt::format("Enable compaction when compaction not disable, {}", (u8)status_));
    }
    for (auto *segment_entry : segment_entries) {
        this->AddSegmentNoCheckInner(segment_entry);
    }
    status_ = CompactionStatus::kEnable;
    cv_.notify_one();
}

void DBTCompactionAlg::Disable() {
    std::unique_lock lock(mtx_);
    cv_.wait(lock, [this]() {
        bool res = (status_ == CompactionStatus::kEnable);
        if (!res) {
            LOG_WARN(fmt::format("table {} is auto compacting now. wait", *(table_entry_->TableEntryDir())));
        }
        return res;
    });
    segment_layers_.clear();
    status_ = CompactionStatus::kDisable;
}

void DBTCompactionAlg::AddSegmentNoCheck(SegmentEntry *new_segment) {
    if (status_ != CompactionStatus::kEnable) {
        UnrecoverableError(fmt::format("Called when compaction not enable, {}", (u8)status_));
    }
    AddSegmentNoCheckInner(new_segment);
}

} // namespace infinity
