//
// Created by jinhai on 23-9-17.
//


#pragma once

#include "common/types/internal_types.h"
#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/txn/constants.h"

namespace infinity {

class TableScanFunctionData: public TableFunctionData {
public:
    TableScanFunctionData(const Vector<SegmentEntry*>* segment_entries_ptr,
                          const SharedPtr<Vector<u64>>& segment_indexes,
                          const Vector<SizeT>& column_ids)
            : segment_entries_ptr_(segment_entries_ptr),
              segment_indexes_(segment_indexes),
              column_ids_(column_ids)
    {}

    const Vector<SegmentEntry*>* segment_entries_ptr_{};
    const SharedPtr<Vector<u64>>& segment_indexes_{};
    const Vector<SizeT>& column_ids_{};

    i64 segment_idx_offset_{0};
    SizeT current_read_offset_{0};
};

}
