//
// Created by jinhai on 23-8-12.
//

#include "column_data_entry.h"
#include "common/types/complex/embedding_type.h"
#include "common/types/data_type.h"
#include "common/types/info/embedding_info.h"
#include "common/types/internal_types.h"
#include "common/types/logical_type.h"
#include "common/utility/infinity_assert.h"
#include "segment_entry.h"
#include "storage/buffer/buffer_manager.h"
#include <cstddef>
#include <cstring>
#include <string>

namespace infinity {

SharedPtr<ColumnDataEntry>
ColumnDataEntry::MakeNewColumnDataEntry(const SegmentEntry *segment_entry,
                                        u64 column_id,
                                        u64 row_capacity,
                                        const SharedPtr<DataType>& data_type,
                                        BufferManager* buffer_mgr) {
    SharedPtr<ColumnDataEntry> column_data_entry = MakeShared<ColumnDataEntry>(segment_entry);
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
ColumnDataEntry::GetColumnData(ColumnDataEntry* column_data_entry, BufferManager* buffer_mgr) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        column_data_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(column_data_entry->base_dir_,
                                                                        column_data_entry->file_name_,
                                                                        BufferType::kFile);
    }

//    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle(column_data_entry->buffer_handle_);
}

// copy data *from* `column_vector` (offset `block_start_offset`).
// *to* `column_data_entry`'s buffer (offset column_start_offset).
void
ColumnDataEntry::Append(ColumnDataEntry* column_data_entry,
       const SharedPtr<ColumnVector>& column_vector,
       SizeT block_start_offset,
       SizeT column_start_offset,
       SizeT row_n) {
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
        case kFloat:
        case kDouble:
        case kEmbedding: {
            ptr_t src_ptr = column_vector->data() + block_start_offset * column_vector->data_type_size_;
            // ColumnDataEntry::AppendRaw(column_data_entry, column_start_offset, src_ptr, row_n, 0);
            SizeT data_size = row_n * column_vector->data_type_size_;
            ColumnDataEntry::AppendRaw(column_data_entry, column_start_offset, 0, src_ptr, data_size);
            break;
        }
        case kVarchar: {
            // TODO shenyushi
            NotImplementError("AppendRaw: Not implement the varchar.")
            break;
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_vector->data_type()->ToString())
            StorageError("Invalid data type")
        }
        default: {
            LOG_ERROR("{} isn't supported", column_vector->data_type()->ToString())
            NotImplementError("Not supported now in append data in column")
        }
    }
}

void
ColumnDataEntry::AppendRaw(ColumnDataEntry* column_data_entry, SizeT dst_row_offset, SizeT dst_ele_offset, ptr_t src_ptr, SizeT data_size) {
    auto column_type = column_data_entry->column_type_;
    switch (column_type->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kFloat:
        case kDouble: {
            if (dst_ele_offset != 0) {
                StorageError("AppendRaw: Not support set offset in simple type");
            }
            ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
            SizeT type_size = column_type->Size(); // Is it right?
            ptr_t dst_ptr = ptr + dst_row_offset * type_size;
            memcpy(dst_ptr, src_ptr, data_size);
            break;
        }
        case kEmbedding: {
            ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
            auto embedding_info = dynamic_cast<EmbeddingInfo *>(column_type->type_info().get());
            ptr_t dst_ptr = ptr + dst_row_offset * embedding_info->Size() + dst_ele_offset * embedding_info->DataSize();
            memcpy(dst_ptr, src_ptr, data_size);
            break;
        }
        case kVarchar: {
            // TODO shenyushi
            NotImplementError("AppendRaw: Not implement the varchar.")
            break;
        }
        case kNull:
        case kMissing:
        case kInvalid: {
            StorageError("AppendRaw: Error type.")
        }
        default: {
            NotImplementError("AppendRaw: Not implement the type.")
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

nlohmann::json
ColumnDataEntry::Serialize(const ColumnDataEntry* column_data_entry) {
    nlohmann::json json_res;
    json_res["column_type"] = column_data_entry->column_type_->Serialize();
    json_res["base_dir"] = *column_data_entry->base_dir_;
    json_res["file_name"] = *column_data_entry->file_name_;
    json_res["column_id"] = column_data_entry->column_id_;
    json_res["row_capacity"] = column_data_entry->row_capacity_;
    json_res["begin_ts"] = column_data_entry->begin_ts_;
    json_res["commit_ts"] = column_data_entry->commit_ts_.load();
    json_res["txn_id"] = column_data_entry->txn_id_.load();
    json_res["deleted"] = column_data_entry->deleted_;
    return json_res;
}

SharedPtr<ColumnDataEntry>
ColumnDataEntry::Deserialize(const nlohmann::json& column_data_json, SegmentEntry* segment_entry, BufferManager* buffer_mgr) {
    SharedPtr<ColumnDataEntry> column_data_entry = MakeShared<ColumnDataEntry>(segment_entry);
    column_data_entry->column_type_ = DataType::Deserialize(column_data_json["column_type"]);
    column_data_entry->base_dir_ = MakeShared<String>(column_data_json["base_dir"]);
    column_data_entry->file_name_ = MakeShared<String>(column_data_json["file_name"]);
    column_data_entry->column_id_ = column_data_json["column_id"];
    column_data_entry->row_capacity_ = column_data_json["row_capacity"];
    column_data_entry->begin_ts_ = column_data_json["begin_ts"];
    column_data_entry->commit_ts_ = column_data_json["commit_ts"];
    column_data_entry->txn_id_ = column_data_json["txn_id"];
    column_data_entry->deleted_ = column_data_json["deleted"];

//    column_data_entry->buffer_handle_ = buffer_mgr->AllocateBufferHandle(column_data_entry->base_dir_,
//                                                                         column_data_entry->file_name_,
//                                                                         column_data_entry->column_type_ * column_data_entry->row_capacity_);

    return column_data_entry;
}

}