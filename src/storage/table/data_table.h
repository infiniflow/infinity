//
// Created by jinhai on 23-7-1.
//

#pragma once

#include "storage/base_table.h"
#include "storage/table_def.h"
#include "data_segment.h"

namespace infinity {

class DataTable : public BaseTable {
public:
    static inline SharedPtr<DataTable>
    Make(SharedPtr<TableDef> table_def_ptr) {
        return MakeShared<DataTable>(std::move(table_def_ptr));
    }

public:
    explicit
    DataTable(SharedPtr<TableDef> table_def_ptr)
        : BaseTable(BaseTableType::kTable, table_def_ptr->schema_name(), table_def_ptr->table_name()),
        definition_ptr_(std::move(table_def_ptr)), row_count_(0)
    {}

    UniquePtr<String>
    Append(void* txn_ptr, AppendState& append_state);

    UniquePtr<String>
    Delete(void* txn_ptr, DeleteState& delete_state);

    UniquePtr<String>
    InitScan(void* txn_ptr, ScanState& scan_state);

    UniquePtr<String>
    Scan(void* txn_ptr, ScanState scan_state);

    UniquePtr<String>
    CommitAppend(void* txn_ptr, AppendState& append_state);

    UniquePtr<String>
    RollbackAppend(void* txn_ptr, AppendState& append_state);

    UniquePtr<String>
    CommitDelete(void* txn_ptr, DeleteState& append_state);

    UniquePtr<String>
    RollbackDelete(void* txn_ptr, DeleteState& append_state);

    UniquePtr<String>
    ImportAppendSegment(void* txn_ptr, SharedPtr<DataSegment> segment, AppendState& append_state);

    inline u64
    GetNextSegmentID() {
        return next_segment_id_ ++;
    }

private:

    static inline void
    AddAppendState(AppendState& append_state, DataSegment* segment_ptr) {
        append_state.append_ranges_.emplace_back(segment_ptr->SegmentID(), 0, segment_ptr->RowCount());
    }


private:
    RWMutex rw_locker_{};

    SharedPtr<TableDef> definition_ptr_;
    SizeT row_count_{0};
    HashMap<u64, SharedPtr<DataSegment>> segments_{};
    DataSegment* unsealed_segment_{};
    au64 next_segment_id_{};
};

}
