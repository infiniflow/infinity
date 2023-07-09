//
// Created by jinhai on 23-6-29.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/data_block.h"

namespace infinity {

struct BlockScanState {

};

struct TableScanState {
    SizeT block_count_{};
    SizeT current_block_id_{};
    DataBlock* current_block_{};
    SizeT row_count_{};
    Vector<SizeT> column_ids_{};
};

}

