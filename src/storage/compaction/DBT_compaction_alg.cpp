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

void SegmentLayer::AddSegment(SegmentEntry *segment_entry) {
    SegmentID segment_id = segment_entry->segment_id();
    auto [iter, insert_ok] = segments_.emplace(segment_id, segment_entry);
    if (!insert_ok) {
        UnrecoverableError(fmt::format("SegmentID conflict: {}", segment_id));
    }
}

void SegmentLayer::RemoveSegment(SegmentEntry *shrink_segment) {
    SegmentID shrink_id = shrink_segment->segment_id();
    SizeT remove_n = segments_.erase(shrink_id);
    if (remove_n != 1) {
        UnrecoverableError(fmt::format("Segment not found in layer: {}", shrink_id));
    }
}

Vector<SegmentEntry *> SegmentLayer::PickCompacting(TransactionID txn_id, SizeT M) {
    Vector<SegmentEntry *> ret;
    SizeT pick_n = std::min(M, segments_.size());
    for (SizeT i = 0; i < pick_n; ++i) {
        auto iter = segments_.begin();
        ret.push_back(iter->second);
        segments_.erase(iter);
    }
    auto [iter, insert_ok] = compacting_segments_map_.emplace(txn_id, ret); // copy here
    if (!insert_ok) {
        UnrecoverableError(fmt::format("TransactionID conflict: {}", txn_id));
    }
    return ret;
}

void SegmentLayer::CommitCompact(TransactionID txn_id) {
    SizeT remove_n = compacting_segments_map_.erase(txn_id);
    if (remove_n != 1) {
        UnrecoverableError(fmt::format("TransactionID not found in layer: {}", txn_id));
    }
}

void SegmentLayer::RollbackCompact(TransactionID txn_id) {
    if (auto iter = compacting_segments_map_.find(txn_id); iter != compacting_segments_map_.end()) {
        for (auto *rollback_segment : iter->second) {
            AddSegment(rollback_segment);
        }
        compacting_segments_map_.erase(iter);
    } else {
        UnrecoverableError(fmt::format("TransactionID not found in layer: {}", txn_id));
    }
}

SegmentEntry *SegmentLayer::FindSegment(SegmentID segment_id) {
    if (auto iter = segments_.find(segment_id); iter != segments_.end()) {
        return iter->second;
    }
    return nullptr;
}

Vector<SegmentEntry *> DBTCompactionAlg::CheckCompaction(TransactionID txn_id) {
    std::unique_lock lock(mtx_);

    if (status_ == CompactionStatus::kDisable) {
        return {};
    }

    int cur_layer_n = segment_layers_.size();
    for (int layer = cur_layer_n - 1; layer >= 0; --layer) {
        auto &segment_layer = segment_layers_[layer];
        if (segment_layer.LayerSize() >= config_.m_) {
            if (++running_task_n_ == 1) {
                status_ = CompactionStatus::kRunning;
            }
            Vector<SegmentEntry *> compact_segments = segment_layer.PickCompacting(txn_id, config_.m_);

            txn_2_layer_.emplace(txn_id, layer);
            return compact_segments;
        }
    }
    return {};
}

void DBTCompactionAlg::AddSegment(SegmentEntry *new_segment) {
    std::unique_lock lock(mtx_);
    AddSegmentInner(new_segment);
}

void DBTCompactionAlg::DeleteInSegment(SegmentID segment_id) {
    std::unique_lock lock(mtx_);
    auto [shrink_segment, old_layer] = FindSegmentAndLayer(segment_id);
    if (shrink_segment == nullptr) {
        return; // this segment is compacting, ignore it
    }

    SegmentOffset new_row_cnt = shrink_segment->actual_row_count();
    int new_layer = config_.CalculateLayer(new_row_cnt);
    if (new_layer == old_layer) {
        return;
    }
    if (new_layer > old_layer) {
        UnrecoverableError("Shrink segment should has less rows than before");
    }
    segment_layers_[old_layer].RemoveSegment(shrink_segment);
    segment_layers_[new_layer].AddSegment(shrink_segment);

    // old_segment_layer.SetOneCompacting(shrink_segment, txn_id);
}

void DBTCompactionAlg::CommitCompact(TransactionID commit_txn_id) {
    std::unique_lock lock(mtx_);
    if (status_ != CompactionStatus::kRunning) {
        UnrecoverableError(fmt::format("Wrong status of compaction alg: {}", (u8)status_));
    }

    if (auto iter = txn_2_layer_.find(commit_txn_id); iter != txn_2_layer_.end()) {
        segment_layers_[iter->second].CommitCompact(commit_txn_id);
        txn_2_layer_.erase(iter);
    } else {
        UnrecoverableError(fmt::format("TransactionID not found in layer: {}", commit_txn_id));
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

    if (auto iter = txn_2_layer_.find(rollback_txn_id); iter != txn_2_layer_.end()) {
        segment_layers_[iter->second].RollbackCompact(rollback_txn_id);
        txn_2_layer_.erase(iter);
    } else {
        UnrecoverableError(fmt::format("TransactionID not found in layer: {}", rollback_txn_id));
    }
    if (--running_task_n_ == 0) {
        status_ = CompactionStatus::kEnable;
    }
}

// Must be called when all segments are not compacting
void DBTCompactionAlg::Enable(const Vector<SegmentEntry *> &segment_entries) {
    std::unique_lock lock(mtx_);
    if (status_ != CompactionStatus::kDisable) {
        UnrecoverableError(fmt::format("Enable compaction when compaction not disable, {}", (u8)status_));
    }
    for (auto *segment_entry : segment_entries) {
        this->AddSegmentInner(segment_entry);
    }
    if (running_task_n_ != 0) {
        UnrecoverableError(fmt::format("Running task is not 0 when enable compaction, table dir: {}, table_ptr: {}",
                                       *(table_entry_->TableEntryDir()),
                                       (u64)table_entry_));
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

int DBTCompactionAlg::AddSegmentInner(SegmentEntry *new_segment) {
    SegmentOffset new_row_cnt = new_segment->actual_row_count();
    int layer = config_.CalculateLayer(new_row_cnt);
    if (layer >= (int)segment_layers_.size()) {
        segment_layers_.resize(layer + 1);
    }
    SegmentLayer &segment_layer = segment_layers_[layer];
    // segment_layer_map_.emplace(new_segment->segment_id(), Pair<SegmentEntry *, int>{new_segment, layer});
    segment_layer.AddSegment(new_segment);
    return layer;
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

} // namespace infinity
