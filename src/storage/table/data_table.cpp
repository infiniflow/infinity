//
// Created by jinhai on 23-7-1.
//
#if 0
#include "data_table.h"
#include "storage/txn/txn_store.h"

namespace infinity {

void
DataTable::Append(void* txn_ptr, void* txn_store) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    if(unsealed_segment_ == nullptr) {
        // No segment at all.
        std::unique_lock<RWMutex> rw_locker(rw_locker_); // prevent another read conflict with this append operation

        // Need double-check
        if(unsealed_segment_ == nullptr) {
            SharedPtr<DataSegment> new_segment = MakeShared<DataSegment>(GetNextSegmentID());
            segments_.emplace(new_segment->SegmentID(), new_segment);
            unsealed_segment_ = new_segment.get();
            unsealed_segment_->Init(this->definition_ptr_->columns(), dir_, buffer_mgr_);
            LOG_TRACE("Add a new segment");
        }
    }

    while(!append_state_ptr->Finished()) {
        SizeT current_row = append_state_ptr->current_count_;

        if(unsealed_segment_->AvailableCapacity() == 0) {
            // uncommitted_segment is full
            std::unique_lock<RWMutex> rw_locker(rw_locker_); // prevent another read conflict with this append operation
            // Need double-check
            if(unsealed_segment_->AvailableCapacity() == 0) {
                SharedPtr<DataSegment> new_segment = MakeShared<DataSegment>(GetNextSegmentID());
                segments_.emplace(new_segment->SegmentID(), new_segment);
                unsealed_segment_ = new_segment.get();
                unsealed_segment_->Init(this->definition_ptr_->columns(), dir_, buffer_mgr_);
                LOG_TRACE("Add a new segment");
            }
        }

        unsealed_segment_->Append(txn_ptr, append_state_ptr);
        LOG_TRACE("Segment is appended with {} rows", append_state_ptr->current_count_ - current_row);
    }
}

UniquePtr<String>
DataTable::Delete(void* txn_ptr, DeleteState& delete_state) {
    // Check all these row_ids belong to which segment.
}

UniquePtr<String>
DataTable::InitScan(void* txn_ptr, ScanState& scan_state) {

}

UniquePtr<String>
DataTable::Scan(void* txn_ptr, ScanState scan_state) {

}

void
DataTable::CommitAppend(void* txn_ptr, const AppendState* append_state_ptr) {

    HashSet<u64> new_segments;
    for(const auto& range: append_state_ptr->append_ranges_) {
        LOG_TRACE("Commit, segment: {}, start: {}, count: {}", range.segment_id_, range.start_pos_, range.row_count_);
        DataSegment* segment_ptr = segments_[range.segment_id_].get();
        segment_ptr->CommitAppend(txn_ptr, range.start_pos_, range.row_count_);
        new_segments.insert(range.segment_id_);
    }

    // FIXME: now all commit will trigger flush
    for(u64 segment_id: new_segments) {
        segments_[segment_id]->PrepareFlush();
        segments_[segment_id]->Flush();
    }
}

void
DataTable::RollbackAppend(void* txn_ptr, void* txn_store) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    NotImplementError("Not implemented");
}

UniquePtr<String>
DataTable::CommitDelete(void* txn_ptr, DeleteState& append_state) {

}

UniquePtr<String>
DataTable::RollbackDelete(void* txn_ptr, DeleteState& append_state) {

}

UniquePtr<String>
DataTable::ImportAppendSegment(void* txn_ptr, SharedPtr<DataSegment> segment, AppendState& append_state) {
    AddAppendState(append_state, segment.get());
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
    segments_.emplace(segment->SegmentID(), std::move(segment));
    return nullptr;
}

}
#endif