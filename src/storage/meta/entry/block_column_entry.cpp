//
// Created by jinhai on 23-10-2.
//

#include "block_column_entry.h"
#include "storage/meta/entry/block_entry.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "common/types/varchar_layout.h"

namespace infinity {

UniquePtr<BlockColumnEntry>
BlockColumnEntry::MakeNewBlockColumnEntry(const BlockEntry* block_entry,
                                          u64 column_id,
                                          BufferManager* buffer_manager) {
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry,
                                                                                  column_id,
                                                                                  block_entry->base_dir_);

    block_column_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");

    block_column_entry->column_type_ = block_entry->segment_entry_->table_entry_->columns_[column_id]->type();
    DataType* column_type = block_column_entry->column_type_.get();

    SizeT row_capacity = block_entry->row_capacity_;
    SizeT total_data_size = row_capacity * column_type->Size();

    block_column_entry->buffer_handle_ = buffer_manager->AllocateBufferHandle(block_column_entry->base_dir_,
                                                                              block_column_entry->file_name_,
                                                                              total_data_size);

    if(block_column_entry->column_type_->type() == kVarchar) {
        block_column_entry->outline_info_ = MakeUnique<OutlineInfo>(buffer_manager);
    }

    return block_column_entry;
}

ColumnBuffer
BlockColumnEntry::GetColumnData(BlockColumnEntry* block_column_entry,
                                BufferManager* buffer_manager) {
    if(block_column_entry->buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        block_column_entry->buffer_handle_ = buffer_manager->GetBufferHandle(block_column_entry->base_dir_,
                                                                             block_column_entry->file_name_,
                                                                             BufferType::kFile);
    }

    bool outline = block_column_entry->column_type_->type() == kVarchar;
    return ColumnBuffer(block_column_entry->buffer_handle_, buffer_manager, outline);
}

void
BlockColumnEntry::Append(BlockColumnEntry* column_entry,
                         offset_t column_entry_offset,
                         ColumnVector* input_column_vector,
                         offset_t input_offset,
                         SizeT append_rows) {
    if(column_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle")
    }
    SizeT data_type_size = input_column_vector->data_type_size_;

    ptr_t src_ptr = input_column_vector->data() + input_offset * data_type_size;
    SizeT data_size = append_rows * data_type_size;
    SizeT dst_offset = column_entry_offset * data_type_size;
    BlockColumnEntry::AppendRaw(column_entry, dst_offset, src_ptr, data_size);
}

void
BlockColumnEntry::AppendRaw(BlockColumnEntry* block_column_entry,
                            SizeT dst_offset,
                            ptr_t src_p,
                            SizeT data_size) {
    CommonObjectHandle object_handle(block_column_entry->buffer_handle_);
    ptr_t dst_p = object_handle.GetData() + dst_offset;
    // ptr_t dst_ptr = column_data_entry->buffer_handle_->LoadData() + dst_offset;
    DataType* column_type = block_column_entry->column_type_.get();
    switch(column_type->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kFloat:
        case kDouble:
        case kEmbedding: {
            memcpy(dst_p, src_p, data_size);
            break;
        }
        case kVarchar: {
            auto inline_p = reinterpret_cast<VarcharLayout*>(dst_p);
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
                    auto outline_info = block_column_entry->outline_info_.get();
                    if(outline_info->written_buffers_.empty() ||
                       outline_info->written_buffers_.back().second + varchar_type->length >
                       DEFAULT_OUTLINE_FILE_MAX_SIZE) {
                        auto file_name = SegmentColumnEntry::OutlineFilename(outline_info->next_file_idx++);
                        auto buffer_handle = outline_info->buffer_mgr_->AllocateBufferHandle(block_column_entry->base_dir_,
                                                                                             file_name,
                                                                                             DEFAULT_OUTLINE_FILE_MAX_SIZE);
                        outline_info->written_buffers_.emplace_back(buffer_handle, 0);
                    }
                    auto& [current_buffer_handle, current_buffer_offset] = outline_info->written_buffers_.back();
                    CommonObjectHandle out_object_handle(current_buffer_handle);
                    ptr_t outline_dst_ptr = out_object_handle.GetData() + current_buffer_offset;
                    u16 outline_data_size = varchar_type->length;
                    ptr_t outline_src_ptr = varchar_type->ptr;
                    memcpy(outline_dst_ptr, outline_src_ptr, outline_data_size);

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
BlockColumnEntry::Flush(BlockColumnEntry* block_column_entry,
                        SizeT row_count) {
    DataType* column_type = block_column_entry->column_type_.get();
    switch(column_type->type()) {
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
            SizeT buffer_size = row_count * column_type->Size();
            block_column_entry->buffer_handle_->WriteFile(buffer_size);
            block_column_entry->buffer_handle_->SyncFile();
            block_column_entry->buffer_handle_->CloseFile();
            break;
        }
        case kVarchar: {
            SizeT buffer_size = row_count * column_type->Size();
            block_column_entry->buffer_handle_->WriteFile(buffer_size);
            block_column_entry->buffer_handle_->SyncFile();
            block_column_entry->buffer_handle_->CloseFile();
            auto outline_info = block_column_entry->outline_info_.get();
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
            LOG_ERROR("{} isn't supported", column_type->ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_type->ToString())
            StorageError("Invalid data type")
        }
    }
}


nlohmann::json
BlockColumnEntry::Serialize(const BlockColumnEntry* block_column_entry) {
    nlohmann::json json_res;
    json_res["column_id"] = block_column_entry->column_id_;
//    json_res["base_dir"] = *block_column_entry->base_dir_;
//    json_res["file_name"] = *block_column_entry->file_name_;
    if(block_column_entry->outline_info_) {
        auto& outline_info = block_column_entry->outline_info_;
        json_res["next_outline_idx"] = outline_info->next_file_idx;
    }

    return json_res;
}

UniquePtr<BlockColumnEntry>
BlockColumnEntry::Deserialize(const nlohmann::json& column_data_json,
                              BlockEntry* block_entry,
                              BufferManager* buffer_mgr) {
    u64 column_id = column_data_json["column_id"];

//    SharedPtr<String> base_dir = MakeShared<String>(column_data_json["base_dir"]);
//    SharedPtr<String> file_name = MakeShared<String>(column_data_json["file_name"]);

    UniquePtr<BlockColumnEntry> block_column_entry = MakeNewBlockColumnEntry(block_entry,
                                                                             column_id,
                                                                             buffer_mgr);
    if(block_column_entry->outline_info_ != nullptr) {
        auto outline_info = block_column_entry->outline_info_.get();
        outline_info->next_file_idx = column_data_json["next_outline_idx"];
    }
    return block_column_entry;
}

}