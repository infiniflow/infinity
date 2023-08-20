//
// Created by jinhai on 23-8-12.
//

#include "column_data_entry.h"
#include "segment_entry.h"
#include "storage/buffer/buffer_manager.h"

namespace infinity {

SharedPtr<ColumnDataEntry>
ColumnDataEntry::MakeNewColumnDataEntry(const void *segment_entry,
                                        TxnContext *txn_context,
                                        u64 column_id,
                                        u64 row_capacity,
                                        const SharedPtr<DataType>& data_type,
                                        BufferManager* buffer_mgr) {
    SharedPtr<ColumnDataEntry> column_data_entry = MakeShared<ColumnDataEntry>(segment_entry, txn_context);
    const auto* segment_ptr = (const SegmentEntry*)segment_entry;
    column_data_entry->base_dir_ = segment_ptr->base_dir_;
    column_data_entry->row_capacity_ = row_capacity;
    column_data_entry->column_type_ = data_type;
    column_data_entry->column_id_ = column_id;
    column_data_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");
    column_data_entry->buffer_handle_ = buffer_mgr->AllocateBufferHandle(column_data_entry->base_dir_,
                                                                         column_data_entry->file_name_,
                                                                         data_type->Size() * row_capacity);
    return column_data_entry;
}

ObjectHandle
ColumnDataEntry::GetColumnData(ColumnDataEntry* column_data_entry, void* buf_mgr_ptr) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        BufferManager* buffer_mgr = (BufferManager*)buf_mgr_ptr;
        column_data_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(column_data_entry->base_dir_,
                                                                        column_data_entry->file_name_,
                                                                        BufferType::kFile);
    }

//    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle(column_data_entry->buffer_handle_);
}

void
ColumnDataEntry::Append(ColumnDataEntry* column_data_entry,
       const SharedPtr<ColumnVector>& column_vector,
       SizeT block_start_offset,
       SizeT column_start_offset,
       SizeT rows) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle")
    }
    ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
    switch(column_vector->data_type()->type()) {
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
            LOG_ERROR("{} isn't supported", column_vector->data_type()->ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_vector->data_type()->ToString())
            StorageError("Invalid data type")
        }
    }
}

void
ColumnDataEntry::Flush(ColumnDataEntry* column_data_entry,
                       SizeT row_count) {
    SizeT buffer_size{0};
    switch(column_data_entry->column_type_->type()) {
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
            buffer_size = row_count * column_data_entry->column_type_->Size();
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
            LOG_ERROR("{} isn't supported", column_data_entry->column_type_->ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_data_entry->column_type_->ToString())
            StorageError("Invalid data type")
        }
    }

    column_data_entry->buffer_handle_->WriteFile(buffer_size);
    column_data_entry->buffer_handle_->SyncFile();
    column_data_entry->buffer_handle_->CloseFile();
}

}