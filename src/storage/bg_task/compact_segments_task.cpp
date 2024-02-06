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

#include <algorithm>
#include <vector>

module compact_segments_task;

import stl;
import catalog;
import default_values;
import infinity_exception;
import parser;
import data_access_state;
import column_vector;
import buffer_manager;
import txn;
import txn_state;
import txn_manager;
import infinity_exception;
import bg_task;
import wal;
import global_block_id;
import block_index;
import catalog_iterator;

namespace infinity {

RowID RowIDRemapper::GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const {
    auto &block_vec = row_id_map_.at(GlobalBlockID(segment_id, block_id));
    auto iter = std::upper_bound(block_vec.begin(),
                                 block_vec.end(),
                                 block_offset,
                                 [](BlockOffset block_offset, const Pair<BlockOffset, RowID> &pair) { return block_offset < pair.first; } // NOLINT
    );
    if (iter == block_vec.begin()) {
        UnrecoverableError("RowID not found");
    }
    --iter;
    RowID rtn = iter->second;
    rtn.segment_offset_ += iter->first - block_offset;
    return rtn;
}

class GreedyCompactableSegmentsGenerator {
public:
    GreedyCompactableSegmentsGenerator(const Vector<SegmentEntry *> &segments, SizeT max_segment_size) : max_segment_size_(max_segment_size) {
        for (auto *segment_entry : segments) {
            segments_.emplace(segment_entry->actual_row_count(), segment_entry);
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
        return result;
    }

private:
    MultiMap<SizeT, SegmentEntry *> segments_; // TODO(opt): use Map<Vector> replace MultiMap

    const SizeT max_segment_size_;
};

CompactSegmentsTask::CompactSegmentsTask(BaseTableRef *table_ref, Txn *txn)
    : BGTask(BGTaskType::kCompactSegments, false), table_ref_(table_ref), txn_(txn) {}

void CompactSegmentsTask::Execute() {
    auto state = CompactSegments();
    CreateNewIndex(state.new_table_ref_.get());
    SaveSegmentsData(std::move(state.segment_data_));
    ApplyDeletes(state.remapper_);
}

CompactSegmentsTaskState CompactSegmentsTask::CompactSegments() {
    CompactSegmentsTaskState state(table_ref_);
    auto block_index = MakeShared<BlockIndex>();

    Vector<SegmentEntry *> segments = PickSegmentsToCompact();
    GreedyCompactableSegmentsGenerator generator(segments, DEFAULT_SEGMENT_CAPACITY);

    while (true) {
        Vector<SegmentEntry *> to_compact_segments = generator.generate();
        if (to_compact_segments.empty()) {
            break;
        }
        auto new_segment = CompactSegmentsToOne(state.remapper_, to_compact_segments);
        block_index->Insert(new_segment.get(), UNCOMMIT_TS, false);
        state.segment_data_.emplace_back(new_segment, std::move(to_compact_segments));
    }

    // TODO: BaseTableRef has no copy constructor
    *state.new_table_ref_ = BaseTableRef(table_ref_->table_entry_ptr_,
                                         table_ref_->column_ids_,
                                         block_index,
                                         table_ref_->alias_,
                                         table_ref_->table_index_,
                                         table_ref_->column_names_,
                                         table_ref_->column_types_);
    return state;
}

void CompactSegmentsTask::CreateNewIndex(BaseTableRef *new_table_ref) {
    auto *table_entry = new_table_ref->table_entry_ptr_;
    TransactionID txn_id = txn_->TxnID();
    TxnTimeStamp begin_ts = txn_->BeginTS();

    for (auto &[index_name, table_index_meta] : table_entry->index_meta_map()) {
        auto [table_index_entry, status] = table_index_meta->GetEntry(txn_id, begin_ts);
        if (!status.ok()) {
            // Table index entry isn't found
            RecoverableError(status);
        }
        txn_->CreateIndexPrepare(table_index_entry, new_table_ref, false, false);
    }
}

void CompactSegmentsTask::SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data) {
    Vector<WalSegmentInfo> segment_infos;
    Vector<SegmentID> old_segment_ids;

    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (auto &[new_segment, old_segments] : segment_data) {
        new_segment->FlushNewData();
        for (auto *old_segment : old_segments) {
            old_segment->SetNoDelete(begin_ts);
        }

        const auto [block_cnt, last_block_row_count] = new_segment->GetWalInfo();
        segment_infos.emplace_back(WalSegmentInfo{*new_segment->segment_dir(),
                                                  new_segment->segment_id(),
                                                  static_cast<u16>(block_cnt),
                                                  DEFAULT_BLOCK_CAPACITY, // TODO: store block capacity in segment entry
                                                  last_block_row_count});
        for (auto *old_segment : old_segments) {
            old_segment_ids.push_back(old_segment->segment_id());
        }
    }
    auto *table_entry = table_ref_->table_entry_ptr_;
    String db_name = *table_entry->GetDBName();
    String table_name = *table_entry->GetTableName();
    txn_->Compact(db_name, table_name, std::move(segment_data));
    txn_->AddWalCmd(MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(segment_infos), std::move(old_segment_ids)));
}

void CompactSegmentsTask::ApplyDeletes(const RowIDRemapper &remapper) {
    while (!ApplyToDelete(remapper)) {
        // TODO: wait some time
    }
}

void CompactSegmentsTask::AddToDelete(SegmentID segment_id, Vector<SegmentOffset> &&delete_offsets, SharedPtr<Txn> delete_txn) {
    std::unique_lock lock(mutex_);
    to_deletes_.emplace_back(ToDeleteInfo{segment_id, std::move(delete_offsets), delete_txn});
}

Vector<SegmentEntry *> CompactSegmentsTask::PickSegmentsToCompact() {
    Vector<SegmentEntry *> segments;
    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (auto *segment_entry : table_ref_->block_index_->segments_) {
        if (segment_entry->TrySetCompacting(this, begin_ts)) {
            segments.push_back(segment_entry);
        }
    }
    return segments;
}

SharedPtr<SegmentEntry> CompactSegmentsTask::CompactSegmentsToOne(RowIDRemapper &remapper, const Vector<SegmentEntry *> &segments) {
    auto *table_entry = table_ref_->table_entry_ptr_;
    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry, NewCatalog::GetNextSegmentID(table_entry), txn_, true);

    TxnTimeStamp begin_ts = txn_->BeginTS();
    SizeT column_count = table_entry->ColumnCount();
    BufferManager *buffer_mgr = txn_->buffer_manager();

    auto new_block = BlockEntry::NewBlockEntry(new_segment.get(), 0, 0, column_count, txn_);
    for (auto *old_segment : segments) {
        BlockEntryIter block_entry_iter(old_segment);
        for (auto *old_block = block_entry_iter.Next(); old_block; old_block = block_entry_iter.Next()) {
            Vector<ColumnVector> input_column_vectors;
            for (ColumnID column_id = 0; column_id < column_count; ++column_id) {
                auto *column_block_entry = old_block->GetColumnBlockEntry(column_id);
                input_column_vectors.emplace_back(column_block_entry->GetColumnVector(buffer_mgr));
            }
            SizeT read_offset = 0;
            while (true) {
                auto [row_begin, row_end] = old_block->GetVisibleRange(begin_ts, read_offset);
                SizeT read_size = row_end - row_begin;
                if (read_size == 0) {
                    break;
                }

                auto block_entry_append = [&](SizeT row_begin, SizeT read_size) {
                    new_block->AppendBlock(input_column_vectors, row_begin, read_size, buffer_mgr);
                    RowID new_row_id(new_segment->segment_id(), new_block->block_id() * DEFAULT_BLOCK_CAPACITY + new_block->row_count());
                    remapper.AddMap(old_segment->segment_id(), old_block->block_id(), row_begin, new_row_id);
                    read_offset = row_begin + read_size;
                };

                if (read_size + new_block->row_count() > new_block->row_capacity()) {
                    SizeT read_size1 = new_block->row_capacity() - new_block->row_count();
                    block_entry_append(row_begin, read_size1);
                    row_begin += read_size1;
                    read_size -= read_size1;
                    new_segment->AppendBlockEntry(std::move(new_block));

                    new_block = BlockEntry::NewBlockEntry(new_segment.get(), new_segment->GetNextBlockID(), 0, column_count, txn_);
                }
                block_entry_append(row_begin, read_size);
            }
        }
    }
    if (new_block->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(new_block));
    }

    return new_segment;
}

bool CompactSegmentsTask::ApplyToDelete(const RowIDRemapper &remapper) {
    if (to_deletes_.empty()) {
        return true;
    }
    bool all_delete_done = true;
    Vector<RowID> row_ids;
    for (const auto &delete_info : to_deletes_) {
        bool committed = false;
        switch (delete_info.delete_txn_->GetTxnState()) {
            case TxnState::kCommitted: {
                committed = true;
                break;
            }
            case TxnState::kRollbacked: {
                break;
            }
            default: {
                all_delete_done = false;
            }
        }
        if (!all_delete_done) {
            break;
        } else if (committed) {
            // remap the delete row id
            for (SegmentOffset offset : delete_info.delete_offsets_) {
                RowID old_row_id(delete_info.segment_id_, offset);
                RowID new_row_id = remapper.GetNewRowID(old_row_id);
                row_ids.push_back(new_row_id);
            }
        }
    }
    if (!all_delete_done) {
        return false;
    }
    txn_->Delete(*table_ref_->schema_name(), *table_ref_->table_name(), row_ids, false);
    return true;
}

} // namespace infinity