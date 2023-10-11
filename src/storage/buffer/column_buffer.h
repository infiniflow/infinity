#pragma once

#include "common/types/alias/containers.h"
#include "storage/buffer/object_handle.h"
#include <limits>

namespace infinity {

class BufferManager;
class DataType;

struct OutlineBuffer {
    BufferManager *buffer_mgr_{};

    SizeT current_file_idx_{std::numeric_limits<SizeT>::max()};

    ObjectHandle outline_ele_{};

    OutlineBuffer(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

class ColumnBuffer {
    ObjectHandle inline_col_{};

    // is null, if the column is inline
    UniquePtr<OutlineBuffer> outline_buffer_{};

public:
    explicit ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool is_outline);

    const ptr_t GetAll();

    Pair<const ptr_t, SizeT> GetVarcharAt(SizeT row_idx);

    Pair<const ptr_t, SizeT> GetVarcharAtPrefix(SizeT row_idx);

    ptr_t GetValueAt(SizeT row_idx, const DataType &data_type);
};

} // namespace infinity
