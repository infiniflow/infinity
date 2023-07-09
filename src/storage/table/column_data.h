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
    ColumnData(String file_path, BufferManager* buffer_mgr) : file_path_(std::move(file_path)), buffer_mgr_(buffer_mgr) {}

    ~ColumnData();

    // location and file name
    // file type
    // fd
    // size
    // index
    // object handle
    ObjectHandle
    Get();

private:
    String file_path_{};
    SizeT file_size_{};
    BufferHandle* buffer_handle_{};
    BufferManager* buffer_mgr_{};
};

}

