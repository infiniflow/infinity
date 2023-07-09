//
// Created by jinhai on 23-7-2.
//

#include "column_data.h"

namespace infinity {

ColumnData::~ColumnData() {
    buffer_handle_ = nullptr;
}

ObjectHandle
ColumnData::Get() {
    if(buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        buffer_handle_ = buffer_mgr_->GetBufferHandle(file_path_, BufferType::kFile);
    }

    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle((BufferHandle*)ptr);
}

}
