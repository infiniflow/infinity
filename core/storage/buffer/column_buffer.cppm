
module ;

import stl;
import parser;
import buffer_manager;
import object_handle;
import buffer_handle;

export module column_buffer;

namespace infinity {

struct OutlineBuffer {
    BufferManager *buffer_mgr_{};

    SizeT current_file_idx_{u64_max};

    ObjectHandle outline_ele_{};

    OutlineBuffer(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

export class ColumnBuffer {
    ObjectHandle inline_col_{};

    // is null, if the column is inline
    UniquePtr<OutlineBuffer> outline_buffer_{};

public:
    explicit ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool is_outline);

    ptr_t GetAll();

    Pair<const_ptr_t, SizeT> GetVarcharAt(SizeT row_idx);

    Pair<const_ptr_t, SizeT> GetVarcharAtPrefix(SizeT row_idx);

    ptr_t GetValueAt(SizeT row_idx, const DataType &data_type);
};

} // namespace infinity
