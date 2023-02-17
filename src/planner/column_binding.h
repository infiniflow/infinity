//
// Created by jinhai on 23-2-17.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct ColumnBinding {

    explicit
    ColumnBinding(SizeT tbl_idx, SizeT col_idx) : table_idx(tbl_idx), column_idx(col_idx) {
    }

    inline bool
    operator==(const ColumnBinding &other) const {
        return table_idx == other.table_idx && column_idx == other.column_idx;
    }

    SizeT table_idx;
    SizeT column_idx;

};

}
