//
// Created by jinhai on 23-7-1.
//

#pragma once

#include "data_access_state.h"
#include "column_data.h"
#include "parser/statement/extra/create_table_info.h"

namespace infinity {

struct ColumnChunk {
    String data_location_;

};

struct DataChunk {

};

enum class DataSegmentStatus {
    kOpen,
    kClosed,
    kFlushing,
};

struct BlockVersion {
    explicit
    BlockVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};
};

class DataSegment {
public:
    explicit
    DataSegment(u64 segment_id, SizeT segment_row = DEFAULT_SEGMENT_ROW)
        : segment_id_(segment_id), row_capacity_(segment_row), block_verions_(segment_row) {}

    void
    Init(const Vector<SharedPtr<ColumnDef>>& column_defs, const SharedPtr<String>& dir, void* buffer_mgr);

    void
    Append(void* txn_ptr, AppendState* append_state);

    UniquePtr<String>
    Delete(void* txn_ptr, DeleteState& delete_state);

    UniquePtr<String>
    InitScan(void* txn_ptr, ScanState& scan_state);

    UniquePtr<String>
    Scan(void* txn_ptr, ScanState scan_state);

    void
    CommitAppend(void* txn_ptr, u64 start_pos, u64 row_count);

    UniquePtr<String>
    RevertAppend(void* ptr, u64 start_pos, u64 row_count);

    UniquePtr<String>
    CommitDelete(void* txn_ptr, const Vector<RowID>& row_ids);

    bool
    PrepareFlush();

    UniquePtr<String>
    Flush();

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

    inline ColumnData*
    GetColumnDataByID(u64 column_id) {
        return columns_[column_id].get();
    }

private:
    RWMutex rw_locker_{};

    SharedPtr<String> dir_{};
//    SizeT segment_capacity_{};
    SizeT row_capacity_{};
    SizeT current_row_{};
    u64 segment_id_{};
    std::atomic<DataSegmentStatus> status_{DataSegmentStatus::kOpen};

    Vector<SharedPtr<ColumnData>> columns_;

    BlockVersion block_verions_;
};

}




