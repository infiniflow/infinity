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
};


class ColumnBuffer {
    ObjectHandle inline_col_;
    UniquePtr<OutlineBuffer> outline_buffer_; // is null, if the column is inline

public:
    explicit ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool outline);

    ptr_t GetAll();

    Pair<ptr_t, SizeT> GetAt(SizeT row_idx);

    Pair<ptr_t, SizeT> GetAtPrefix(SizeT row_idx);

};
}