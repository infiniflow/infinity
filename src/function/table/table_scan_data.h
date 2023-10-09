//
// Created by jinhai on 23-9-17.
//

#pragma once

#include "function/function_data.h"
#include "function/table_function.h"
#include "storage/common/global_block_id.h"
#include "storage/txn/txn_state.h"

namespace infinity {

class TableScanFunctionData : public TableFunctionData {
public:
    TableScanFunctionData(const BlockIndex *block_index, const SharedPtr<Vector<GlobalBlockID>> &global_block_ids, const Vector<SizeT> &column_ids)
        : block_index_(block_index), global_block_ids_(global_block_ids), column_ids_(column_ids) {}

    const BlockIndex *block_index_{};
    const SharedPtr<Vector<GlobalBlockID>> &global_block_ids_{};
    const Vector<SizeT> &column_ids_{};

    i64 current_block_ids_idx_{0};
    SizeT current_read_offset_{0};
};

} // namespace infinity
