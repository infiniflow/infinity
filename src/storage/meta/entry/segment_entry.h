//
// Created by jinhai on 23-8-10.
//


#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "column_data_entry.h"

namespace infinity {

struct SegmentVersion {
    explicit
    SegmentVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};
};

struct SegmentEntry : public BaseEntry {
public:
    explicit
    SegmentEntry(SharedPtr<TableDef> table_def,
                void* table_entry,
                u64 txn_id,
                TxnTimeStamp begin_ts,
                TxnContext* txn_context,
                void* buffer_mgr)
            : BaseEntry(EntryType::kTable, txn_context), column_def_(std::move(column_def)), table_entry_(table_entry) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    RWMutex rw_locker_{};

    Vector<SharedPtr<DataType>> data_type_{};

    void* table_entry_{};

    SharedPtr<String> dir_{};

    SizeT row_capacity_{};

    SizeT current_row_{};

    u64 segment_id_{};

    std::atomic<DataSegmentStatus> status_{DataSegmentStatus::kOpen};

    Vector<SharedPtr<ColumnDataEntry>> columns_;

    UniquePtr<SegmentVersion> segment_version_{};
};

}
