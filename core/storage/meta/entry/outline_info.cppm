//
// Created by jinhai on 23-10-4.
//

module;

import stl;
import buffer_handle;

export module outline_info;

namespace infinity {

class BufferManager;

export struct OutlineInfo {
    Vector<Pair<BufferHandle *, SizeT>> written_buffers_{};

    BufferManager *buffer_mgr_{};

    SizeT next_file_idx{};

    OutlineInfo(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

} // namespace infinity
