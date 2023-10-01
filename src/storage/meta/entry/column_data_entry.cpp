//
// Created by jinhai on 23-8-12.
//

#include "column_data_entry.h"
#include "common/default_values.h"
#include "common/types/data_type.h"

#include "common/types/logical_type.h"
#include "common/types/varchar_layout.h"
#include "common/utility/infinity_assert.h"
#include "segment_entry.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/column_buffer.h"
#include <cstring>
#include <string>

namespace infinity {

SharedPtr<ColumnDataEntry>
ColumnDataEntry::MakeNewColumnDataEntry(const SegmentEntry* segment_entry, u64 column_id, u64 row_capacity,
                                        const SharedPtr<DataType>& data_type, BufferManager* buffer_mgr) {
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
    if(data_type->type() == kVarchar) {
        column_data_entry->outline_info_ = MakeUnique<OutlineInfo>(buffer_mgr);
    }
    return column_data_entry;
}

ColumnBuffer
ColumnDataEntry::GetColumnData(ColumnDataEntry* column_data_entry, BufferManager* buffer_mgr) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        column_data_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(column_data_entry->base_dir_,
                                                                        column_data_entry->file_name_,
                                                                        BufferType::kFile);
    }

    bool outline = column_data_entry->column_type_->type() == kVarchar;
    return ColumnBuffer(column_data_entry->buffer_handle_, buffer_mgr, outline);
}

// Note!!!: caller of `Append` should guarantee the buffer_handler has enough place to append `row_n` rows.
void
ColumnDataEntry::Append(ColumnDataEntry* column_data_entry,
                        const SharedPtr<ColumnVector>& column_vector,
                        SizeT block_start_offset,
                        SizeT column_start_offset,
                        SizeT row_n) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle")
    }
    ptr_t src_ptr = column_vector->data() + block_start_offset * column_vector->data_type_size_;
    SizeT data_size = row_n * column_vector->data_type_size_;
    SizeT dst_offset = column_start_offset * column_vector->data_type_size_;
    ColumnDataEntry::AppendRaw(column_data_entry, dst_offset, src_ptr, data_size);
}

void
ColumnDataEntry::AppendRaw(ColumnDataEntry* column_data_entry, SizeT dst_offset, ptr_t src_p, SizeT data_size) {
    auto column_type = column_data_entry->column_type_;
    CommonObjectHandle object_handle(column_data_entry->buffer_handle_);
    ptr_t dst_ptr = object_handle.GetData() + dst_offset;
    // ptr_t dst_ptr = column_data_entry->buffer_handle_->LoadData() + dst_offset;

    switch(column_type->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kFloat:
        case kDouble:
        case kEmbedding: {
            memcpy(dst_ptr, src_p, data_size);
            break;
        }
        case kVarchar: {
            auto inline_p = reinterpret_cast<VarcharLayout*>(dst_ptr);
            auto src_ptr = reinterpret_cast<VarcharT*>(src_p);
            SizeT row_n = data_size / sizeof(VarcharT);
            for(SizeT row_idx = 0; row_idx < row_n; row_idx++) {
                auto varchar_type = src_ptr + row_idx;
                VarcharLayout* varchar_layout = inline_p + row_idx;
                if(varchar_type->IsInlined()) {
                    auto& short_info = varchar_layout->u.short_info_;
                    varchar_layout->length_ = varchar_type->length;
                    memcpy(short_info.data.data(), varchar_type->prefix, varchar_type->length);
                } else {
                    auto& long_info = varchar_layout->u.long_info_;
                    auto outline_info = column_data_entry->outline_info_.get();
                    if(outline_info->written_buffers_.empty() ||
                       outline_info->written_buffers_.back().second + varchar_type->length >
                       DEFAULT_OUTLINE_FILE_MAX_SIZE) {
                        auto file_name = ColumnDataEntry::OutlineFilename(outline_info->next_file_idx++);
                        auto buffer_handle = outline_info->buffer_mgr_->AllocateBufferHandle(column_data_entry->base_dir_,
                                                                                             file_name,
                                                                                             DEFAULT_OUTLINE_FILE_MAX_SIZE);
                        outline_info->written_buffers_.emplace_back(buffer_handle, 0);
                    }
                    auto& [current_buffer_handle, current_buffer_offset] = outline_info->written_buffers_.back();
                    CommonObjectHandle out_object_handle(current_buffer_handle);
                    ptr_t dst_ptr = out_object_handle.GetData() + current_buffer_offset;
                    SizeT data_size = varchar_type->length;
                    ptr_t src_ptr = varchar_type->ptr;
                    memcpy(dst_ptr, src_ptr, data_size);

                    varchar_layout->length_ = varchar_type->length;
                    memcpy(long_info.prefix_.data(), varchar_type->prefix, VarcharT::PREFIX_LENGTH);
                    long_info.file_idx_ = outline_info->next_file_idx - 1;
                    long_info.file_offset_ = current_buffer_offset;
                    current_buffer_offset += varchar_type->length;
                }
            }
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
        case kEmbedding:
        case kRowID: {
            SizeT buffer_size = row_count * column_data_entry->column_type_->Size();
            column_data_entry->buffer_handle_->WriteFile(buffer_size);
            column_data_entry->buffer_handle_->SyncFile();
            column_data_entry->buffer_handle_->CloseFile();
            break;
        }
        case kVarchar: {
            SizeT buffer_size = row_count * column_data_entry->column_type_->Size();
            column_data_entry->buffer_handle_->WriteFile(buffer_size);
            column_data_entry->buffer_handle_->SyncFile();
            column_data_entry->buffer_handle_->CloseFile();
            auto outline_info = column_data_entry->outline_info_.get();
            for(auto [outline_buffer_handle, outline_size]: outline_info->written_buffers_) {
                outline_buffer_handle->WriteFile(outline_size);
                outline_buffer_handle->SyncFile();
                outline_buffer_handle->CloseFile();
            }
            break;
        }
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
    if(column_data_entry->outline_info_) {
        auto& outline_info = column_data_entry->outline_info_;
        json_res["next_outline_idx"] = outline_info->next_file_idx;
    }
    return json_res;
}

SharedPtr<ColumnDataEntry>
ColumnDataEntry::Deserialize(const nlohmann::json& column_data_json,
                             SegmentEntry* segment_entry,
                             BufferManager* buffer_mgr) {
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

    if(column_data_entry->outline_info_ != nullptr) {
        auto outline_info = column_data_entry->outline_info_.get();
        outline_info->next_file_idx = column_data_json["next_outline_idx"];
    }
    return column_data_entry;
}
}