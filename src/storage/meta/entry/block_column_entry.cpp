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

module block_column_entry;

import stl;
import buffer_manager;

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
import var_file_worker;
import catalog_delta_entry;
import internal_types;
import data_type;
import logical_type;

namespace infinity {

Vector<std::string_view> BlockColumnEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid block column entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = BlockEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String BlockColumnEntry::EncodeIndex(const ColumnID column_id, const BlockEntry *block_entry) {
    return fmt::format("{}#{}", block_entry->encode(), column_id);
}

BlockColumnEntry::BlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, const SharedPtr<String> &base_dir_ref)
    : BaseEntry(EntryType::kBlockColumn, false, block_entry->base_dir_, BlockColumnEntry::EncodeIndex(column_id, block_entry)),
      block_entry_(block_entry), column_id_(column_id), base_dir_(base_dir_ref) {}

UniquePtr<BlockColumnEntry> BlockColumnEntry::NewBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, Txn *txn) {
    SharedPtr<String> full_path = MakeShared<String>(fmt::format("{}/{}", *block_entry->base_dir_, *block_entry->block_dir()));
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id, full_path);

    auto begin_ts = txn->BeginTS();
    block_column_entry->begin_ts_ = begin_ts;

    block_column_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");
    block_column_entry->column_type_ = block_entry->GetColumnType(column_id);

    DataType *column_type = block_column_entry->column_type_.get();
    SizeT row_capacity = block_entry->row_capacity();
    SizeT total_data_size = row_capacity * column_type->Size();
    if (column_type->type() == kBoolean) {
        // TODO
        total_data_size = (row_capacity + 7) / 8;
    }

    auto file_worker = MakeUnique<DataFileWorker>(block_column_entry->base_dir_, block_column_entry->file_name_, total_data_size);

    auto *buffer_mgr = txn->buffer_mgr();
    block_column_entry->buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));

    return block_column_entry;
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::NewReplayBlockColumnEntry(const BlockEntry *block_entry,
                                                                        ColumnID column_id,
                                                                        BufferManager *buffer_manager,
                                                                        const u32 next_outline_idx,
                                                                        const u64 last_chunk_offset,
                                                                        const TxnTimeStamp commit_ts) {
    SharedPtr<String> full_path = MakeShared<String>(fmt::format("{}/{}", *block_entry->base_dir_, *block_entry->block_dir()));
    UniquePtr<BlockColumnEntry> column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id, full_path);
    column_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");
    column_entry->column_type_ = block_entry->GetColumnType(column_id);
    column_entry->commit_ts_ = commit_ts;

    DataType *column_type = column_entry->column_type_.get();
    SizeT row_capacity = block_entry->row_capacity();
    SizeT total_data_size = (column_type->type() == kBoolean) ? ((row_capacity + 7) / 8) : (row_capacity * column_type->Size());
    auto file_worker = MakeUnique<DataFileWorker>(column_entry->base_dir_, column_entry->file_name_, total_data_size);

    column_entry->buffer_ = buffer_manager->GetBufferObject(std::move(file_worker));

    if (next_outline_idx > 0) {
        SizeT buffer_size = last_chunk_offset;
        auto outline_buffer_file_worker = MakeUnique<VarFileWorker>(column_entry->base_dir_, column_entry->OutlineFilename(0, 0), buffer_size);
        auto *buffer_obj = buffer_manager->GetBufferObject(std::move(outline_buffer_file_worker));
        column_entry->outline_buffers_.push_back(buffer_obj);
    }
    column_entry->last_chunk_offset_ = last_chunk_offset;

    return column_entry;
}

ColumnVector BlockColumnEntry::GetColumnVector(BufferManager *buffer_mgr) { return GetColumnVectorInner(buffer_mgr, ColumnVectorTipe::kReadWrite); }

ColumnVector BlockColumnEntry::GetConstColumnVector(BufferManager *buffer_mgr) {
    return GetColumnVectorInner(buffer_mgr, ColumnVectorTipe::kReadOnly);
}

ColumnVector BlockColumnEntry::GetColumnVectorInner(BufferManager *buffer_mgr, const ColumnVectorTipe tipe) {
    if (this->buffer_ == nullptr) {
        // Get buffer handle from buffer manager
        auto file_worker = MakeUnique<DataFileWorker>(this->base_dir_, this->file_name_, 0);
        this->buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
    }

    ColumnVector column_vector(column_type_);
    column_vector.Initialize(buffer_mgr, this, block_entry_->row_count(), tipe);
    return column_vector;
}

SharedPtr<String> BlockColumnEntry::OutlineFilename(const u32 buffer_group_id, const SizeT file_idx) const {
    if (buffer_group_id == 0) {
        return MakeShared<String>(fmt::format("col_{}_out_{}", column_id_, file_idx));
    } else if (buffer_group_id == 1) {
        return MakeShared<String>(fmt::format("col_{}_out1_{}", column_id_, file_idx));
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
        return nullptr;
    }
}

void BlockColumnEntry::AppendOutlineBuffer(const u32 buffer_group_id, BufferObj *buffer) {
    std::unique_lock lock(mutex_);
    if (buffer_group_id == 0) {
        outline_buffers_.push_back(buffer);
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
    }
}

BufferObj *BlockColumnEntry::GetOutlineBuffer(const u32 buffer_group_id, const SizeT idx) const {
    std::shared_lock lock(mutex_);
    if (buffer_group_id == 0) {
        return outline_buffers_.empty() ? nullptr : outline_buffers_[idx];
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
        return nullptr;
    }
}

SizeT BlockColumnEntry::OutlineBufferCount(const u32 buffer_group_id) const {
    std::shared_lock lock(mutex_);
    if (buffer_group_id == 0) {
        return outline_buffers_.size();
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
        return 0;
    }
}

u64 BlockColumnEntry::LastChunkOff(const u32 buffer_group_id) const {
    if (buffer_group_id == 0) {
        return last_chunk_offset_;
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
        return 0;
    }
}

void BlockColumnEntry::SetLastChunkOff(const u32 buffer_group_id, const u64 offset) {
    if (buffer_group_id == 0) {
        last_chunk_offset_ = offset;
    } else {
        String error_message = "Invalid buffer group id";
        UnrecoverableError(error_message);
    }
}

void BlockColumnEntry::Append(const ColumnVector *input_column_vector, u16 input_column_vector_offset, SizeT append_rows, BufferManager *buffer_mgr) {
    if (buffer_ == nullptr) {
        String error_message = "Not initialize buffer handle";
        UnrecoverableError(error_message);
    }
    ColumnVector &&column_vector = GetColumnVector(buffer_mgr);
    column_vector.AppendWith(*input_column_vector, input_column_vector_offset, append_rows);
}

void BlockColumnEntry::Flush(BlockColumnEntry *block_column_entry, SizeT start_row_count, SizeT checkpoint_row_count) {
    // TODO: Opt, Flush certain row_count content
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
        case kFloat16:
        case kBFloat16:
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
            i64 address = (i64)(block_column_entry->buffer_);
            LOG_TRACE(fmt::format("Saving {} {}", block_column_entry->column_id(), address));
            block_column_entry->buffer_->Save();
            LOG_TRACE(fmt::format("Saved {}", block_column_entry->column_id()));

            break;
        }
        case kTensor:
        case kSparse:
        case kTensorArray:
        case kVarchar: {
            //            SizeT buffer_size = row_count * column_type->Size();
            LOG_TRACE(fmt::format("Saving column {}", block_column_entry->column_id()));
            block_column_entry->buffer_->Save();
            LOG_TRACE(fmt::format("Saved column {}", block_column_entry->column_id()));

            std::shared_lock lock(block_column_entry->mutex_);
            for (auto *outline_buffer : block_column_entry->outline_buffers_) {
                if (outline_buffer != nullptr) {
                    outline_buffer->Save();
                }
            }
            break;
        }
        case kArray:
        case kTuple:
            //        case kPath:
            //        case kPolygon:
            //        case kBlob:
        case kEmptyArray:
        case kMixed:
        case kNull: {
            LOG_ERROR(fmt::format("{} isn't supported", column_type->ToString()));
            String error_message = "Not implement: Invalid data type.";
            UnrecoverableError(error_message);
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR(fmt::format("Invalid data type {}", column_type->ToString()));
            String error_message = "Invalid data type.";
            UnrecoverableError(error_message);
        }
    }
}

void BlockColumnEntry::Cleanup() {
    if (buffer_ != nullptr) {
        buffer_->PickForCleanup();
    }
    for (auto *outline_buffer : outline_buffers_) {
        if (outline_buffer) {
            outline_buffer->PickForCleanup();
        }
    }
}

nlohmann::json BlockColumnEntry::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = this->column_id_;
    {
        std::shared_lock lock(mutex_);
        json_res["next_outline_idx"] = outline_buffers_.size();
        json_res["last_chunk_offset"] = this->LastChunkOff(0);
        json_res["last_chunk_offset_1"] = this->LastChunkOff(1);
    }

    json_res["commit_ts"] = TxnTimeStamp(this->commit_ts_);
    json_res["begin_ts"] = TxnTimeStamp(this->begin_ts_);
    json_res["txn_id"] = TransactionID(this->txn_id_);
    return json_res;
}

UniquePtr<BlockColumnEntry>
BlockColumnEntry::Deserialize(const nlohmann::json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr) {
    const ColumnID column_id = column_data_json["column_id"];
    const TxnTimeStamp commit_ts = column_data_json["commit_ts"];
    const u32 next_outline_idx = column_data_json["next_outline_idx"];
    const u64 last_chunk_offset = column_data_json["last_chunk_offset"];
    UniquePtr<BlockColumnEntry> block_column_entry =
        NewReplayBlockColumnEntry(block_entry, column_id, buffer_mgr, next_outline_idx, last_chunk_offset, commit_ts);
    block_column_entry->begin_ts_ = column_data_json["begin_ts"];
    block_column_entry->txn_id_ = column_data_json["txn_id"];

    return block_column_entry;
}

void BlockColumnEntry::CommitColumn(TransactionID txn_id, TxnTimeStamp commit_ts) {
    if (this->Committed()) {
        String error_message = "Column already committed";
        UnrecoverableError(error_message);
    }
    this->txn_id_ = txn_id;
    this->Commit(commit_ts);
}

} // namespace infinity
