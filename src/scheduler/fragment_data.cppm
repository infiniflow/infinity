//
// Created by jinhai on 23-10-6.
//

module;

import stl;
import data_block;

export module fragment_data;

namespace infinity {

export struct FragmentData {
    SharedPtr<DataBlock> data_block_{};

    i64 task_id_{-1};
    i64 data_idx_{-1};
    i64 data_count_{-1};
};

} // namespace infinity
