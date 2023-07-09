//
// Created by jinhai on 23-7-1.
//

#include "data_table.h"

namespace infinity {

UniquePtr<String>
DataTable::Append(void* txn_ptr, AppendState& append_state) {
    if(unsealed_segment_ == nullptr) {
        // No segment at all.
        std::unique_lock<RWMutex> rw_locker(rw_locker_);

        // Need double-check
        if(unsealed_segment_ == nullptr) {
            SharedPtr<DataSegment> new_segment = MakeShared<DataSegment>(GetNextSegmentID());
            segments_.emplace(new_segment->SegmentID(), new_segment);
            unsealed_segment_ = new_segment.get();
        }
    }

    while(!append_state.Finished()) {
        if(unsealed_segment_->AvailableCapacity() == 0) {
            // uncommitted_segment is full
            std::unique_lock<RWMutex> rw_locker(rw_locker_);
            // Need double-check
            if(unsealed_segment_->AvailableCapacity() == 0) {
                SharedPtr<DataSegment> new_segment = MakeShared<DataSegment>(GetNextSegmentID());
                segments_.emplace(new_segment->SegmentID(), new_segment);
                unsealed_segment_ = new_segment.get();
            }
        }

        unsealed_segment_->Append(txn_ptr, append_state);
    }
    return nullptr;
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

UniquePtr<String>
DataTable::CommitAppend(void* txn_ptr, AppendState& append_state) {

}

UniquePtr<String>
DataTable::RollbackAppend(void* txn_ptr, AppendState& append_state) {

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