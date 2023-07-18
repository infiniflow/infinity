//
// Created by jinhai on 23-7-2.
//

#include "column_data.h"

namespace infinity {

ColumnData::~ColumnData() {
    buffer_handle_ = nullptr;
}

ObjectHandle
ColumnData::GetColumnData() {
    if(buffer_handle_ == nullptr) {
        // Construct file path
        String file_path = dir_;
        file_path.append("/");
        file_path +=  std::to_string(column_id_);
        file_path.append(".col");

        // Get buffer handle from buffer manager
        buffer_handle_ = buffer_mgr_->GetBufferHandle(file_path, BufferType::kFile);
    }

    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle((BufferHandle*)ptr);
}

ObjectHandle
ColumnData::GetExtraColumnData(u64 extra_block_id) {
    BufferHandle* extra_buffer = extra_buffer_handles_[extra_block_id];
    if(extra_buffer == nullptr) {
        // Construct file path
        String file_path = dir_;
        file_path.append("/");
        file_path +=  std::to_string(column_id_) + ".col." + std::to_string(extra_block_id);

        // Get buffer handle from buffer manager
        buffer_handle_ = buffer_mgr_->GetBufferHandle(file_path, BufferType::kExtraBlock);
    }

    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle((BufferHandle*)ptr);
}

}
