//
// Created by jinhai on 23-7-2.
//

#include "column_data.h"

namespace infinity {

ColumnData::~ColumnData() {
    buffer_handle_ = nullptr;
}

void
ColumnData::Init(const ColumnDef* column_def, SizeT row_capacity) {
    row_capacity_ = row_capacity;
    buffer_handle_ = buffer_mgr_->AllocateBufferHandle(*dir_, column_def->column_type_.Size() * row_capacity_);
}

ObjectHandle
ColumnData::GetColumnData() {
    if(buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        buffer_handle_ = buffer_mgr_->GetBufferHandle(*dir_, BufferType::kFile);
    }

    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle((BufferHandle*)ptr);
}

ObjectHandle
ColumnData::GetExtraColumnData(u64 extra_block_id) {
    BufferHandle* extra_buffer = extra_buffer_handles_[extra_block_id];
    if(extra_buffer == nullptr) {
        // Construct file path
        String file_path = *dir_ + '.' + std::to_string(extra_block_id);

        // Get buffer handle from buffer manager
        buffer_handle_ = buffer_mgr_->GetBufferHandle(file_path, BufferType::kExtraBlock);
    }

    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle((BufferHandle*)ptr);
}

void
ColumnData::Append(const SharedPtr<ColumnVector>& column_vector,
                   SizeT block_start_offset,
                   SizeT column_start_offset,
                   SizeT rows) {
    ptr_t ptr = buffer_handle_->LoadData();
    switch(column_vector->data_type().type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            ptr_t src_ptr = column_vector->data() + block_start_offset * column_vector->data_type_size_;
            ptr_t dst_ptr = ptr + column_start_offset * column_vector->data_type_size_;
            memcpy(dst_ptr, src_ptr, rows *  column_vector->data_type_size_);
            break;
        }

        case kVarchar:
        case kArray:
        case kTuple:
        case kPath:
        case kPolygon:
        case kBlob:
        case kMixed:
        case kNull: {
            LOG_ERROR("{} isn't supported", column_vector->data_type().ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_vector->data_type().ToString())
            StorageError("Invalid data type")
        }
    }
}

void
ColumnData::Flush() {

}

void
ColumnData::CopyFromVariable() {

}

}
