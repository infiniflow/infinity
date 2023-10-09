//
// Created by jinhai on 23-10-4.
//

#pragma once

#include "storage/buffer/buffer_handle.h"

namespace infinity {

class BufferManager;

struct OutlineInfo {
    Vector<Pair<BufferHandle *, SizeT>> written_buffers_{};

    BufferManager *buffer_mgr_{};

    SizeT next_file_idx{};

    OutlineInfo(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

} // namespace infinity
