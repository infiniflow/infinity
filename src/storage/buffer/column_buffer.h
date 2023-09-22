#pragma once

#include "common/types/internal_types.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include <limits>

namespace infinity {

struct OutlineBuffer {
    SharedPtr<String> file_dir_{};
    BufferManager *buffer_mgr_{};
    SizeT current_file_idx_{std::numeric_limits<SizeT>::max()};
    ObjectHandle outline_ele_{};

    OutlineBuffer(BufferManager *buffer_mgr, SharedPtr<String> file_dir) : buffer_mgr_(buffer_mgr), file_dir_(file_dir) {}
};


class ColumnBuffer {
    ObjectHandle inline_col_;
    UniquePtr<OutlineBuffer> outline_buffer_; // is null, if the column is inline

public:
    explicit ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool outline);

    ptr_t GetAll();

    Pair<ptr_t, SizeT> GetVarcharAt(SizeT row_idx);

    Pair<ptr_t, SizeT> GetVarcharAtPrefix(SizeT row_idx);

};
}