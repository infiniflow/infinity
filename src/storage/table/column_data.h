//
// Created by jinhai on 23-7-2.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "parser/statement/extra/create_table_info.h"
#include "storage/data_block.h"

namespace infinity {

class ColumnData {
public:
    explicit
    ColumnData(const SharedPtr<String>& base_dir, u64 column_id, BufferManager* buffer_mgr)
        : column_id_(column_id), buffer_mgr_(buffer_mgr) {
        dir_ = MakeShared<String>(*base_dir + "_col" + std::to_string(column_id));
    }

    ~ColumnData();

    void
    Init(const ColumnDef* column_def, SizeT row_capacity);

    // location and file name
    // file type
    // fd
    // size
    // index
    // object handle
    ObjectHandle
    GetColumnData();

    ObjectHandle
    GetExtraColumnData(u64 extra_block_id);

    void
    Append(const SharedPtr<ColumnVector>& column_vector,
           SizeT block_start_offset,
           SizeT column_start_offset,
           SizeT rows);

    void
    Flush();

private:
    static inline void
    CopyFromPlain(const void* src_ptr,
                  SizeT row_count,
                  SizeT data_size,
                  void* dst_ptr) {

    }

    void
    CopyFromVariable();

private:
    SharedPtr<String> dir_{};
    u64 column_id_{};
    u64 max_extra_block_{}; // Used in dynamic data type column
    SizeT start_row_{};
    SizeT row_capacity_{};

    BufferHandle* buffer_handle_{};
    BufferManager* buffer_mgr_{};
    Vector<BufferHandle*> extra_buffer_handles_{};
};

}

