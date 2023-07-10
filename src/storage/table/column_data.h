//
// Created by jinhai on 23-7-2.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/buffer/buffer_manager.h"

namespace infinity {

class ColumnData {
public:
    explicit
    ColumnData(String dir, u64 column_id, BufferManager* buffer_mgr)
        : dir_(std::move(dir)), column_id_(column_id), buffer_mgr_(buffer_mgr) {}

    ~ColumnData();

    // location and file name
    // file type
    // fd
    // size
    // index
    // object handle
    ObjectHandle
    GetColumnData();

private:
    String dir_{};
    u64 column_id_{};

    BufferHandle* buffer_handle_{};
    BufferManager* buffer_mgr_{};
};

}

