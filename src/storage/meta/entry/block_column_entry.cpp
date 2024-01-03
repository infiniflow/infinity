// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <string>

module catalog;

import stl;
import buffer_manager;
import outline_info;
import parser;
import column_buffer;
import buffer_obj;
import buffer_handle;
import column_vector;
import default_values;
import third_party;
import vector_buffer;
import local_file_system;
import infinity_exception;
import varchar_layout;
import logger;
import data_file_worker;

namespace infinity {

UniquePtr<BlockColumnEntry>
BlockColumnEntry::MakeNewBlockColumnEntry(const BlockEntry *block_entry, u64 column_id, BufferManager *buffer_manager, bool is_replay) {
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id, block_entry->base_dir());

    block_column_entry->file_name_ = MakeShared<String>(ToStr(column_id) + ".col");

    block_column_entry->column_type_ = block_entry->GetColumnType(column_id);
    DataType *column_type = block_column_entry->column_type_.get();

    SizeT row_capacity = block_entry->row_capacity();
    SizeT total_data_size = row_capacity * column_type->Size();

    auto file_worker = MakeUnique<DataFileWorker>(block_column_entry->base_dir_, block_column_entry->file_name_, total_data_size);
    if (!is_replay) {
        block_column_entry->buffer_ = buffer_manager->Allocate(Move(file_worker));
    } else {
        block_column_entry->buffer_ = buffer_manager->Get(Move(file_worker));
    }

    if (block_column_entry->column_type_->type() == kVarchar) {
        block_column_entry->outline_info_ = MakeUnique<OutlineInfo>(buffer_manager);
    }

    return block_column_entry;
}

ColumnBuffer BlockColumnEntry::GetColumnData(BufferManager *buffer_manager) {
    if (this->buffer_ == nullptr) {
        // Get buffer handle from buffer manager
        auto file_worker = MakeUnique<DataFileWorker>(this->base_dir_, this->file_name_, 0);
        this->buffer_ = buffer_manager->Get(Move(file_worker));
    }

    bool outline = this->column_type_->type() == kVarchar;
    return outline ? ColumnBuffer(this->column_id_, this->buffer_, buffer_manager, this->base_dir_) : ColumnBuffer(this->column_id_, this->buffer_);
}

void BlockColumnEntry::Append(BlockColumnEntry *column_entry,
                              u16 column_entry_offset,
                              ColumnVector *input_column_vector,
                              u16 input_column_vector_offset,
                              SizeT append_rows) {
    if (column_entry->buffer_ == nullptr) {
        Error<StorageException>("Not initialize buffer handle");
    }
    SizeT data_type_size = input_column_vector->data_type_size_;

    ptr_t src_ptr = input_column_vector->data() + input_column_vector_offset * data_type_size;
    SizeT data_size = append_rows * data_type_size;
    SizeT dst_offset = column_entry_offset * data_type_size;
    column_entry->AppendRaw(dst_offset, src_ptr, data_size, input_column_vector->buffer_);
}

void BlockColumnEntry::AppendRaw(SizeT dst_offset, const_ptr_t src_p, SizeT data_size, SharedPtr<VectorBuffer> vector_buffer) {
    BufferHandle buffer_handle = this->buffer_->Load();
    ptr_t dst_p = static_cast<ptr_t>(buffer_handle.GetDataMut()) + dst_offset;
    // ptr_t dst_ptr = column_data_entry->buffer_handle_->LoadData() + dst_offset;
    DataType *column_type = this->column_type_.get();
    switch (column_type->type()) {
        case kBoolean:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kFloat:
        case kDouble:
        case kEmbedding: {
            Memcpy(dst_p, src_p, data_size);
            break;
        }
        case kVarchar: {
            auto inline_p = reinterpret_cast<VarcharLayout *>(dst_p);
            auto src_ptr = reinterpret_cast<const VarcharT *>(src_p);
            SizeT row_n = data_size / sizeof(VarcharT);
            for (SizeT row_idx = 0; row_idx < row_n; row_idx++) {
                auto varchar_type = src_ptr + row_idx;
                VarcharLayout *varchar_layout = inline_p + row_idx;
                if (varchar_type->IsInlined()) {
                    auto &short_info = varchar_layout->u.short_info_;
                    varchar_layout->length_ = varchar_type->length_;
                    Memcpy(short_info.data.data(), varchar_type->short_.data_, varchar_type->length_);
                } else {
                    auto &long_info = varchar_layout->u.long_info_;
                    auto outline_info = this->outline_info_.get();
                    auto base_file_size =
                        Min(DEFAULT_BASE_FILE_SIZE * Pow(DEFAULT_BASE_NUM, outline_info->next_file_idx), DEFAULT_OUTLINE_FILE_MAX_SIZE);

                    if (outline_info->written_buffers_.empty() ||
                        outline_info->written_buffers_.back().second + varchar_type->length_ > base_file_size) {

                        auto file_name = BlockColumnEntry::OutlineFilename(this->column_id_, outline_info->next_file_idx++);
                        auto file_worker =
                            MakeUnique<DataFileWorker>(this->base_dir_,
                                                       file_name,
                                                       DEFAULT_BASE_NUM * Max(base_file_size, static_cast<SizeT>(varchar_type->length_)));

                        BufferObj *buffer_obj = outline_info->buffer_mgr_->Allocate(Move(file_worker));
                        outline_info->written_buffers_.emplace_back(buffer_obj, 0);
                    }
                    auto &[current_buffer_obj, current_buffer_offset] = outline_info->written_buffers_.back();
                    BufferHandle out_buffer_handle = current_buffer_obj->Load();
                    ptr_t outline_dst_ptr = static_cast<ptr_t>(out_buffer_handle.GetDataMut()) + current_buffer_offset;
                    if (varchar_type->IsValue()) {
                        u32 outline_data_size = varchar_type->length_;
                        ptr_t outline_src_ptr = varchar_type->value_.ptr_;
                        Memcpy(outline_dst_ptr, outline_src_ptr, outline_data_size);
                    } else {
                        vector_buffer->fix_heap_mgr_->ReadFromHeap(outline_dst_ptr,
                                                                   varchar_type->vector_.chunk_id_,
                                                                   varchar_type->vector_.chunk_offset_,
                                                                   varchar_type->length_);
                    }

                    varchar_layout->length_ = varchar_type->length_;
                    Memcpy(long_info.prefix_.data(), varchar_type->value_.prefix_, VARCHAR_PREFIX_LEN);
                    long_info.file_idx_ = outline_info->next_file_idx - 1;
                    long_info.file_offset_ = current_buffer_offset;
                    current_buffer_offset += varchar_type->length_;
                }
            }
            break;
        }
        case kNull:
        case kMissing:
        case kInvalid: {
            Error<StorageException>("AppendRaw: Error type.");
        }
        default: {
            Error<NotImplementException>("AppendRaw: Not implement the type.");
        }
    }
}

void BlockColumnEntry::Flush(BlockColumnEntry *block_column_entry, SizeT) {
    DataType *column_type = block_column_entry->column_type_.get();
    switch (column_type->type()) {
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
            //        case kBitmap:
        case kUuid:
        case kEmbedding:
        case kRowID: {
            //            SizeT buffer_size = row_count * column_type->Size();
            if (block_column_entry->buffer_->Save()) {
                block_column_entry->buffer_->Sync();
                block_column_entry->buffer_->CloseFile();
            }

            break;
        }
        case kVarchar: {
            //            SizeT buffer_size = row_count * column_type->Size();
            if (block_column_entry->buffer_->Save()) {
                block_column_entry->buffer_->Sync();
                block_column_entry->buffer_->CloseFile();
            }
            auto outline_info = block_column_entry->outline_info_.get();
            for (auto [outline_buffer_handle, outline_size] : outline_info->written_buffers_) {
                if (outline_buffer_handle->Save()) {
                    outline_buffer_handle->Sync();
                    outline_buffer_handle->CloseFile();
                }
            }
            break;
        }
        case kArray:
        case kTuple:
            //        case kPath:
            //        case kPolygon:
            //        case kBlob:
        case kMixed:
        case kNull: {
            LOG_ERROR(Format("{} isn't supported", column_type->ToString()));
            Error<NotImplementException>("Invalid data type.");
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR(Format("Invalid data type {}", column_type->ToString()));
            Error<StorageException>("Invalid data type.");
        }
    }
}

Json BlockColumnEntry::Serialize(BlockColumnEntry *block_column_entry) {
    Json json_res;
    json_res["column_id"] = block_column_entry->column_id_;
    if (block_column_entry->outline_info_) {
        auto &outline_info = block_column_entry->outline_info_;
        json_res["next_outline_idx"] = outline_info->next_file_idx;
    }
    return json_res;
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::Deserialize(const Json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr) {
    u64 column_id = column_data_json["column_id"];
    UniquePtr<BlockColumnEntry> block_column_entry = MakeNewBlockColumnEntry(block_entry, column_id, buffer_mgr, true);
    if (block_column_entry->outline_info_.get() != nullptr) {
        auto outline_info = block_column_entry->outline_info_.get();
        outline_info->next_file_idx = column_data_json["next_outline_idx"];
    }
    return block_column_entry;
}

Vector<String> BlockColumnEntry::OutlinePaths() const {
    Vector<String> outline_paths;

    if (outline_info_.get() != nullptr) {
        for (SizeT i = 0; i < outline_info_->next_file_idx; ++i) {
            auto outline_file = BlockColumnEntry::OutlineFilename(column_id_, i);

            outline_paths.push_back(Move(LocalFileSystem::ConcatenateFilePath(*base_dir_, *outline_file)));
        }
    }
    return outline_paths;
}

} // namespace infinity
