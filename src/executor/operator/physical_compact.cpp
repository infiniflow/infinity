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
import catalog;
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

void PhysicalCompact::Init() {
    if (compact_type_ == CompactStatementType::kManual) {
        TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
        LOG_DEBUG(fmt::format("Manual compact {} start", *table_entry->GetTableName()));
        if (!table_entry->CompactPrepare()) {
            LOG_WARN(fmt::format("Table {} is not compactable.", *table_entry->GetTableName()));
            return;
        }
        GreedyCompactableSegmentsGenerator generator(base_table_ref_.get(), DEFAULT_SEGMENT_CAPACITY);
        while (true) {
            Vector<SegmentEntry *> compactible_segments = generator.generate();
            if (compactible_segments.empty()) {
                break;
            }
            compactible_segments_group_.push_back(compactible_segments);
        }
    } else {
        TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
        LOG_DEBUG(fmt::format("Auto compact {} start", *table_entry->GetTableName()));
        Vector<SegmentEntry *> compactible_segments;
        const auto &block_index = *base_table_ref_->block_index_;
        for (const auto &[segment_id, segment_snapshot] : block_index.segment_block_index_) {
            SegmentEntry *segment_entry = segment_snapshot.segment_entry_;
            if (segment_entry->status() == SegmentStatus::kSealed) {
                compactible_segments.push_back(segment_entry);
            }
        }
        compactible_segments_group_.push_back(compactible_segments);
    }
}

bool PhysicalCompact::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    auto *compact_operator_state = static_cast<CompactOperatorState *>(operator_state);
    SizeT group_idx = compact_operator_state->compact_idx_;
    CompactStateData *compact_state_data = compact_operator_state->compact_state_data_.get();
    RowIDRemap &remapper = compact_state_data->remapper_;
    if (group_idx == compact_operator_state->segment_groups_.size()) {
        compact_operator_state->SetComplete();
        return true;
    }
    const Vector<SegmentEntry *> &candidate_segments = compact_operator_state->segment_groups_[group_idx];
    Vector<SegmentEntry *> compactible_segments;
    {
        String log_str = fmt::format("PhysicalCompact::Execute: group_idx: {}, candidate_segments: ", group_idx);
        for (auto *candidate_segment : candidate_segments) {
            if (candidate_segment->TrySetCompacting(compact_state_data)) {
                compactible_segments.push_back(candidate_segment);
            }
            log_str += fmt::format("{}, ", candidate_segment->segment_id());
        }
        LOG_INFO(log_str);
    }

    auto *txn = query_context->GetTxn();
    if (compactible_segments.empty()) {
        RecoverableError(Status::TxnRollback(txn->TxnID(), "No segment to compact."));
    }
    auto *txn_mgr = txn->txn_mgr();
    auto *buffer_mgr = query_context->storage()->buffer_manager();
    TxnTimeStamp scan_ts = txn_mgr->GetNewTimeStamp();
    compact_state_data->scan_ts_ = scan_ts;

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    SizeT column_count = table_entry->ColumnCount();

    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry, Catalog::GetNextSegmentID(table_entry), txn);
    SegmentID new_segment_id = new_segment->segment_id();
    LOG_INFO(fmt::format("PhysicalCompact::Execute: txn_id: {}, scan_ts: {}, new segment id: {}", txn->TxnID(), scan_ts, new_segment_id));

    UniquePtr<BlockEntry> new_block =
        BlockEntry::NewBlockEntry(new_segment.get(), new_segment->GetNextBlockID(), 0 /*checkpoint_ts*/, column_count, txn);
    const SizeT block_capacity = new_block->row_capacity();

    Vector<SegmentID> old_segment_ids;
    for (SegmentEntry *segment : compactible_segments) {
        SegmentID segment_id = segment->segment_id();
        old_segment_ids.push_back(segment_id);
        const auto &segment_info = block_index->segment_block_index_.at(segment_id);
        for (const auto *block_entry : segment_info.block_map_) {
            BlockID block_id = block_entry->block_id();
            Vector<ColumnVector> input_column_vectors;
            for (ColumnID column_id = 0; column_id < column_count; ++column_id) {
                auto *column_block_entry = block_entry->GetColumnBlockEntry(column_id);
                input_column_vectors.emplace_back(column_block_entry->GetConstColumnVector(buffer_mgr));
            }
            SizeT read_offset = 0;
            while (true) {
                auto [row_begin, row_end] = block_entry->GetVisibleRange(scan_ts, read_offset);
                SizeT read_size = row_end - row_begin;
                if (read_size == 0) {
                    break;
                }

                auto block_entry_append = [&](SizeT row_begin, SizeT read_size1) {
                    if (read_size1 == 0) {
                        return;
                    }
                    RowID new_row_id(new_segment_id, new_block->block_id() * block_capacity + new_block->row_count());
                    new_block->AppendBlock(input_column_vectors, row_begin, read_size1, buffer_mgr);
                    remapper.AddMap(segment_id, block_id, row_begin, new_row_id);
                    read_offset = row_begin + read_size1;
                };

                if (read_size + new_block->row_count() > block_capacity) {
                    SizeT read_size1 = block_capacity - new_block->row_count();
                    block_entry_append(row_begin, read_size1);
                    row_begin += read_size1;
                    read_size -= read_size1;
                    new_segment->AppendBlockEntry(std::move(new_block));

                    new_block = BlockEntry::NewBlockEntry(new_segment.get(), new_segment->GetNextBlockID(), 0, column_count, txn);
                }
                block_entry_append(row_begin, read_size);
            }
        }
    }
    if (new_block->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(new_block));
    } else {
        std::move(*new_block).Cleanup();
    }
    if (new_segment->actual_row_count() > new_segment->row_capacity()) {
        UnrecoverableError(fmt::format("Compact segment {} error because of row count overflow.", new_segment_id));
    }
    compact_state_data->AddNewSegment(new_segment, std::move(compactible_segments), txn);
    compact_operator_state->compact_idx_ = ++group_idx;
    if (group_idx == compact_operator_state->segment_groups_.size()) {
        compact_operator_state->SetComplete();
    }
    compact_operator_state->SetComplete();

    String db_name = *table_entry->GetDBName();
    String table_name = *table_entry->GetTableName();
    Vector<WalSegmentInfo> new_segment_infos;
    new_segment_infos.emplace_back(new_segment.get());
    txn->AddWalCmd(MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(new_segment_infos), std::move(old_segment_ids)));

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