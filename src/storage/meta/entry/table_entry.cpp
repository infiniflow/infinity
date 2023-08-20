//
// Created by jinhai on 23-8-18.
//

#include "table_entry.h"
#include "storage/txn/txn_store.h"
#include "storage/txn/txn.h"


namespace infinity {

void
TableEntry::Append(TableEntry* table_entry, void* txn_ptr, void* txn_store, BufferManager* buffer_mgr) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    Txn* transaction_ptr = (Txn*)txn_ptr;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    if(table_entry->unsealed_segment_ == nullptr) {
        // No segment at all.
        std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation

        // Need double-check
        if(table_entry->unsealed_segment_ == nullptr) {
            u64 next_segment_id = TableEntry::GetNextSegmentID(table_entry);
            SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry,
                                                                                    table_entry->txn_id_,
                                                                                    table_entry->txn_context_,
                                                                                    next_segment_id,
                                                                                    buffer_mgr);
            table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
            table_entry->unsealed_segment_ = new_segment.get();
//            table_entry->unsealed_segment_->Init(this->definition_ptr_->columns(), dir_, buffer_mgr_);
            LOG_TRACE("Add a new segment");
        }
    }

    while(!append_state_ptr->Finished()) {
        SizeT current_row = append_state_ptr->current_count_;

        if(table_entry->unsealed_segment_->AvailableCapacity() == 0) {
            // uncommitted_segment is full
            std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation
            // Need double-check
            if(table_entry->unsealed_segment_->AvailableCapacity() == 0) {
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry,
                                                                                        table_entry->txn_id_,
                                                                                        table_entry->txn_context_,
                                                                                        TableEntry::GetNextSegmentID(table_entry),
                                                                                        buffer_mgr);
                table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
                table_entry->unsealed_segment_ = new_segment.get();
                LOG_TRACE("Add a new segment");
            }
        }

        SegmentEntry::AppendData(table_entry->unsealed_segment_, txn_ptr, append_state_ptr, buffer_mgr);
        LOG_TRACE("Segment is appended with {} rows", append_state_ptr->current_count_ - current_row);
    }
}

UniquePtr<String>
TableEntry::Delete(TableEntry* table_entry, void* txn_ptr, DeleteState& delete_state, BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::InitScan(TableEntry* table_entry, void* txn_ptr, ScanState& scan_state, BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::Scan(TableEntry* table_entry, void* txn_ptr, ScanState scan_state, BufferManager* buffer_mgr) {

}

void
TableEntry::CommitAppend(TableEntry* table_entry,
                         void* txn_ptr,
                         const AppendState* append_state_ptr,
                         BufferManager* buffer_mgr) {
    HashSet<u64> new_segments;
    for(const auto& range: append_state_ptr->append_ranges_) {
        LOG_TRACE("Commit, segment: {}, start: {}, count: {}", range.segment_id_, range.start_pos_, range.row_count_);
        SegmentEntry* segment_ptr = table_entry->segments_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_ptr, range.start_pos_, range.row_count_);
        new_segments.insert(range.segment_id_);
    }

    // FIXME: now all commit will trigger flush
    for(u64 segment_id: new_segments) {
        SegmentEntry::PrepareFlush(table_entry->segments_[segment_id].get());
        SegmentEntry::Flush(table_entry->segments_[segment_id].get());
    }
}

void
TableEntry::RollbackAppend(TableEntry* table_entry, void* txn_ptr, void* txn_store) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    NotImplementError("Not implemented");
}

UniquePtr<String>
TableEntry::CommitDelete(TableEntry* table_entry,
                         void* txn_ptr,
                         DeleteState& append_state,
                         BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::RollbackDelete(TableEntry* table_entry,
                           void* txn_ptr,
                           DeleteState& append_state,
                           BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::ImportAppendSegment(TableEntry* table_entry,
                                void* txn_ptr,
                                SharedPtr<SegmentEntry> segment,
                                AppendState& append_state,
                                BufferManager* buffer_mgr) {
    append_state.append_ranges_.emplace_back(segment->segment_id_, 0, segment->current_row_);
    std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_);
    table_entry->segments_.emplace(segment->segment_id_, std::move(segment));
    return nullptr;
}

DBEntry*
TableEntry::GetDBEntry(const TableEntry* table_entry) {
    TableMeta* table_meta = (TableMeta*)table_entry->table_meta_;
    return (DBEntry*)table_meta->db_entry_;
}

}