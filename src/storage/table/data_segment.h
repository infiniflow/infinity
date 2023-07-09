//
// Created by jinhai on 23-7-1.
//

#pragma once

#include "data_access_state.h"
#include "column_data.h"

namespace infinity {

struct ColumnChunk {
    String data_location_;

};

struct DataChunk {

};

struct BlockVersion {
    explicit
    BlockVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<void*> txn_ptr_{};
};

class DataSegment {
public:
    explicit
    DataSegment(u64 segment_id, SizeT segment_capacity = DEFAULT_SEGMENT_CAPACITY)
        : segment_id_(segment_id), segment_capacity_(segment_capacity) {}

    UniquePtr<String>
    Append(void* txn_ptr, AppendState& append_state);

    UniquePtr<String>
    Delete(void* txn_ptr, DeleteState& delete_state);

    UniquePtr<String>
    InitScan(void* txn_ptr, ScanState& scan_state);

    UniquePtr<String>
    Scan(void* txn_ptr, ScanState scan_state);

    UniquePtr<String>
    CommitAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count);

    UniquePtr<String>
    RevertAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count);

    UniquePtr<String>
    CommitDelete(void* txn_ptr, const Vector<RowID>& row_ids);

    [[nodiscard]] inline SizeT
    AvailableCapacity() const {
        return row_capacity_ - current_row_;
    }

    [[nodiscard]] inline u64
    SegmentID() const {
        return segment_id_;
    }

    [[nodiscard]] inline SizeT
    RowCount() const {
        return current_row_;
    }

private:
    RWMutex rw_locker_{};

    SizeT start_row_{};
    SizeT segment_capacity_{};
    SizeT row_capacity_{};
    SizeT current_row_{};
    u64 segment_id_{};

    Vector<SharedPtr<ColumnData>> column_vectors;

    Vector<BlockVersion> block_verions_{};
};

}




