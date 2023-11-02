//
// Created by jinhai on 23-10-4.
//

module;

import stl;

export module outline_info;

namespace infinity {

class BufferObj;

class BufferManager;

export struct OutlineInfo {
    Vector<Pair<BufferObj *, SizeT>> written_buffers_{};

    BufferManager *buffer_mgr_{};

    SizeT next_file_idx{};

    inline explicit OutlineInfo(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

} // namespace infinity
