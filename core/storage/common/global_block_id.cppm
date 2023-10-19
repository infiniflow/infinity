//
// Created by jinhai on 23-10-4.
//

module;

import stl;

export module global_block_id;

namespace infinity {

export struct GlobalBlockID {
    i32 segment_id_{};
    i16 block_id_{};
};

} // namespace infinity
