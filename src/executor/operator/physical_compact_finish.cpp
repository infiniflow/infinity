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

#include <sstream>
#include <vector>

module physical_compact_finish;

import stl;
import operator_state;
import table_entry;
import segment_entry;
import base_table_ref;
import compact_state_data;
import wal_manager;
import wal_entry;
import logger;
import txn;
import internal_types;

namespace infinity {

bool PhysicalCompactFinish::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *compact_finish_operator_state = static_cast<CompactFinishOperatorState *>(operator_state);
    const CompactStateData *compact_state_data = compact_finish_operator_state->compact_state_data_.get();
    if (!ApplyDeletes(query_context, compact_state_data)) {
        LOG_WARN("Failed to apply deletes in compact finish");
        return true;
    }
    SaveSegmentData(query_context, compact_state_data);
    compact_finish_operator_state->SetComplete();
    return true;
}

void PhysicalCompactFinish::SaveSegmentData(QueryContext *query_context, const CompactStateData *compact_state_data) {
    auto *txn = query_context->GetTxn();
    auto *table_entry = base_table_ref_->table_entry_ptr_;

    Vector<WalSegmentInfo> segment_infos;
    Vector<SegmentID> old_segment_ids;
    Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> segment_data;

    std::stringstream ss;
    for (const auto &compact_segment_data : compact_state_data->segment_data_list_) {
        auto *new_segment = compact_segment_data.new_segment_.get();
        ss << "Compact ";
        new_segment->FlushNewData();
        segment_infos.emplace_back(new_segment);

        for (const auto *old_segment : compact_segment_data.old_segments_) {
            ss << "old segment " << old_segment->segment_id() << ", ";
            old_segment_ids.push_back(old_segment->segment_id());
        }
        ss << "to new segment " << new_segment->segment_id();
        segment_data.emplace_back(compact_segment_data.new_segment_, compact_segment_data.old_segments_);
    }
    LOG_DEBUG(ss.str());

    txn->Compact(table_entry, std::move(segment_data), compact_type_);
    String db_name = *table_entry->GetDBName();
    String table_name = *table_entry->GetTableName();
    txn->AddWalCmd(MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(segment_infos), std::move(old_segment_ids)));
}

bool PhysicalCompactFinish::ApplyDeletes(QueryContext *query_context, const CompactStateData *compact_state_data) {
    auto *txn = query_context->GetTxn();
    auto *table_entry = base_table_ref_->table_entry_ptr_;

    for (const auto &compact_segment_data : compact_state_data->segment_data_list_) {
        for (auto *old_segment : compact_segment_data.old_segments_) {
            if (!old_segment->SetNoDelete()) {
                return false;
            }
        }
    }
    const Vector<Pair<SegmentID, Vector<SegmentOffset>>> &to_delete = compact_state_data->GetToDelete();
    Vector<RowID> row_ids;
    for (const auto &[segment_id, delete_offsets] : to_delete) {
        for (SegmentOffset offset : delete_offsets) {
            RowID old_row_id(segment_id, offset);
            RowID new_row_id = compact_state_data->remapper_.GetNewRowID(old_row_id);
            row_ids.push_back(new_row_id);
        }
    }
    txn->Delete(table_entry, row_ids, false);
    return true;
}

} // namespace infinity