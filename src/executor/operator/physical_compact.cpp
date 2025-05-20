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

#include <vector>

module physical_compact;

import stl;
import internal_types;
import operator_state;
import base_table_ref;
import block_index;
import column_vector;
import table_entry;
import segment_entry;
import block_entry;
import compact_state_data;
import default_values;
import logger;
import infinity_exception;
import third_party;
import status;
import wal_entry;
import wal_manager;
import infinity_context;

import new_txn;
import db_meeta;
import table_meeta;
import segment_meta;
import bg_task;
import compaction_process;

namespace infinity {

class GreedyCompactableSegmentsGenerator {
public:
    GreedyCompactableSegmentsGenerator(const BaseTableRef *base_table_ref, SizeT max_segment_size) : max_segment_size_(max_segment_size) {
        const auto &block_index = *base_table_ref->block_index_;
        for (const auto &[segment_id, segment_snapshot] : block_index.segment_block_index_) {
            SegmentEntry *segment_entry = segment_snapshot.segment_entry_;
            if (segment_entry->status() == SegmentStatus::kSealed) {
                SizeT row_count = segment_entry->actual_row_count();
                segments_.emplace(row_count, segment_entry);
            }
        }
    }

    // find the largest segment to fill the free space
    Vector<SegmentEntry *> generate() {
        Vector<SegmentEntry *> result;
        do {
            result.clear();
            SizeT segment_size = max_segment_size_;

            while (true) {
                auto iter = segments_.upper_bound(segment_size);
                if (iter == segments_.begin()) {
                    break;
                }
                --iter;
                auto [row_count, segment_entry] = *iter;
                segments_.erase(iter);
                result.push_back(segment_entry);
                segment_size -= row_count;
            }
        } while (result.size() == 1 && (result[0]->actual_row_count() == result[0]->row_count()));
        // FIXME: compact single segment with too much delete row
        return result;
    }

private:
    MultiMap<SizeT, SegmentEntry *> segments_; // TODO(opt): use Map<Vector> replace MultiMap

    const SizeT max_segment_size_;
};

void PhysicalCompact::Init(QueryContext *query_context) {
}

bool PhysicalCompact::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *compact_operator_state = static_cast<CompactOperatorState *>(operator_state);

    const String &db_name = *base_table_ref_->table_info_->db_name_;
    const String &table_name = *base_table_ref_->table_info_->table_name_;
    NewTxn *new_txn = query_context->GetNewTxn();

    auto compact_task = MakeShared<NewCompactTask>(new_txn, db_name, table_name);
    auto *compaction_processor = InfinityContext::instance().storage()->compaction_processor();
    compaction_processor->Submit(compact_task);
    compact_task->Wait();

    compact_operator_state->status_ = compact_task->result_status_;
    compact_operator_state->SetComplete();
    return true;
}

Vector<Vector<Vector<SegmentEntry *>>> PhysicalCompact::PlanCompact(SizeT parallel_count) {
    Vector<Vector<Vector<SegmentEntry *>>> result(parallel_count);
    for (SizeT i = 0; i < compactible_segments_group_.size(); ++i) {
        result[i % parallel_count].push_back(compactible_segments_group_[i]);
    }
    return result;
}

} // namespace infinity