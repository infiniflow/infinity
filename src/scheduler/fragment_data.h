//
// Created by jinhai on 23-10-6.
//

#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/primitives.h"

namespace infinity {

class DataBlock;

struct FragmentData {
    SharedPtr<DataBlock> data_block_{};

    i64 task_id_{-1};
    i64 data_idx_{-1};
    i64 data_count_{-1};
};

}
