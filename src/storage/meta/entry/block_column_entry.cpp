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
import virtual_store;
import infinity_exception;
import logger;
import data_file_worker;
import var_file_worker;
import catalog_delta_entry;
import internal_types;
import data_type;
import logical_type;
import infinity_context;
import meta_info;

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

BlockColumnEntry::BlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id)
    : BaseEntry(EntryType::kBlockColumn, false, BlockColumnEntry::EncodeIndex(column_id, block_entry)), block_entry_(block_entry),
      column_id_(column_id) {}

BlockColumnEntry::~BlockColumnEntry() {}

BlockColumnEntry::BlockColumnEntry(const BlockColumnEntry &other)
    : BaseEntry(other), block_entry_(other.block_entry_), column_id_(other.column_id_), column_type_(other.column_type_), buffer_(other.buffer_),
      filename_(other.filename_) {
    std::shared_lock lock(other.mutex_);
    outline_buffers_ = other.outline_buffers_;
    last_chunk_offset_ = other.last_chunk_offset_;

    buffer_->AddObjRc();
    for (auto *outline_buffer : outline_buffers_) {
        outline_buffer->AddObjRc();
    }
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::Clone(BlockEntry *block_entry) const {
    auto ret = UniquePtr<BlockColumnEntry>(new BlockColumnEntry(*this));

    ret->block_entry_ = block_entry;
    return ret;
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::NewBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, Txn *txn) {
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id);

    auto begin_ts = txn->BeginTS();
    block_column_entry->begin_ts_ = begin_ts;

    block_column_entry->filename_ = MakeShared<String>(std::to_string(column_id) + ".col");
    block_column_entry->column_type_ = block_entry->GetColumnType(column_id);

    DataType *column_type = block_column_entry->column_type_.get();
    SizeT row_capacity = block_entry->row_capacity();
    SizeT total_data_size = row_capacity * column_type->Size();
    if (column_type->type() == LogicalType::kBoolean) {
        // TODO
        total_data_size = (row_capacity + 7) / 8;
    }

    auto *buffer_mgr = txn->buffer_mgr();
    auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                  MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                  block_entry->block_dir(),
                                                  block_column_entry->filename_,
                                                  total_data_size,
                                                  buffer_mgr->persistence_manager());

    block_column_entry->buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    block_column_entry->buffer_->AddObjRc();
    return block_column_entry;
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::NewReplayBlockColumnEntry(const BlockEntry *block_entry,
                                                                        ColumnID column_id,
                                                                        BufferManager *buffer_manager,
                                                                        const u32 next_outline_idx,
                                                                        const u64 last_chunk_offset,
                                                                        const TxnTimeStamp commit_ts) {
    UniquePtr<BlockColumnEntry> column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id);
    column_entry->filename_ = MakeShared<String>(std::to_string(column_id) + ".col");
    column_entry->column_type_ = block_entry->GetColumnType(column_id);
    column_entry->commit_ts_ = commit_ts;

    DataType *column_type = column_entry->column_type_.get();
    const LogicalType column_data_logical_type = column_type->type();
    const SizeT row_capacity = block_entry->row_capacity();
    const SizeT total_data_size =
        (column_data_logical_type == LogicalType::kBoolean) ? ((row_capacity + 7) / 8) : (row_capacity * column_type->Size());
    auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                  MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                  block_entry->block_dir(),
                                                  column_entry->filename_,
                                                  total_data_size,
                                                  buffer_manager->persistence_manager());

    column_entry->buffer_ = buffer_manager->GetBufferObject(std::move(file_worker), true /*restart*/);
    column_entry->buffer_->AddObjRc();

    if (next_outline_idx > 0) {
        SizeT buffer_size = last_chunk_offset;
        auto outline_buffer_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                    MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                    block_entry->block_dir(),
                                                                    column_entry->OutlineFilename(0),
                                                                    buffer_size,
                                                                    buffer_manager->persistence_manager());
        auto *buffer_obj = buffer_manager->GetBufferObject(std::move(outline_buffer_file_worker), true /*restart*/);
        buffer_obj->AddObjRc();
        column_entry->outline_buffers_.push_back(buffer_obj);
    }
    column_entry->last_chunk_offset_ = last_chunk_offset;

    if (block_entry->row_count() >= block_entry->row_capacity()) {
        column_entry->ToMmap();
        LOG_TRACE(fmt::format("Column {} to mmap success", column_entry->encode()));
    }

    return column_entry;
}

UniquePtr<BlockColumnEntry> BlockColumnEntry::ApplyBlockColumnSnapshot(BlockEntry *block_entry,
                                                                       BlockColumnSnapshotInfo *block_column_snapshot_info,
                                                                       TransactionID txn_id,
                                                                       TxnTimeStamp begin_ts) {
    ColumnID column_id = block_column_snapshot_info->column_id_;
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id);
    block_column_entry->filename_ = MakeShared<String>(block_column_snapshot_info->filename_);
    block_column_entry->column_type_ = block_entry->GetColumnType(column_id);

    // column_entry->buffer_
    // column_entry->outline need to be updated;

    block_column_entry->last_chunk_offset_ = block_column_snapshot_info->last_chunk_offset_;
    return block_column_entry;
}

SharedPtr<BlockColumnInfo> BlockColumnEntry::GetColumnInfo() const {
    SharedPtr<BlockColumnInfo> block_column_info = MakeShared<BlockColumnInfo>();
    block_column_info->column_id_ = column_id_;
    block_column_info->data_type_ = column_type_;
    block_column_info->filename_ = filename_;

    SizeT storage_size = 0;
    storage_size += buffer_->GetBufferSize();
    for (BufferObj *outline_buffer : outline_buffers_) {
        storage_size += outline_buffer->GetBufferSize();
    }

    block_column_info->storage_size_ = storage_size;
    SizeT outline_count = outline_buffers_.size();
    block_column_info->extra_file_count_ = outline_count;
    block_column_info->extra_file_names_.reserve(outline_count);
    for (SizeT file_idx = 0; file_idx < outline_count; ++file_idx) {
        block_column_info->extra_file_names_.emplace_back(OutlineFilename(file_idx));
    }
    return block_column_info;
}

SharedPtr<BlockColumnSnapshotInfo> BlockColumnEntry::GetSnapshotInfo() const {
    SharedPtr<BlockColumnSnapshotInfo> block_column_snapshot_info = MakeShared<BlockColumnSnapshotInfo>();
    block_column_snapshot_info->column_id_ = column_id_;
    block_column_snapshot_info->filename_ = *filename_;
    block_column_snapshot_info->last_chunk_offset_ = last_chunk_offset_;
    SizeT outline_count = outline_buffers_.size();
    for (SizeT file_idx = 0; file_idx < outline_count; ++file_idx) {
        String outline_file_path = *OutlineFilename(file_idx);
        SharedPtr<OutlineSnapshotInfo> outline_snapshot_info = MakeShared<OutlineSnapshotInfo>();
        outline_snapshot_info->filename_ = outline_file_path;
        block_column_snapshot_info->outline_snapshots_.emplace_back(outline_snapshot_info);
    }
    return block_column_snapshot_info;
}

String BlockColumnEntry::FilePath() const { return Path(*block_entry_->block_dir()) / *filename_; }

SharedPtr<String> BlockColumnEntry::FileDir() const { return block_entry_->block_dir(); }

ColumnVector BlockColumnEntry::GetColumnVector(BufferManager *buffer_mgr, SizeT row_count) {
    return GetColumnVectorInner(buffer_mgr, ColumnVectorTipe::kReadWrite, row_count);
}

ColumnVector BlockColumnEntry::GetConstColumnVector(BufferManager *buffer_mgr, SizeT row_count) {
    return GetColumnVectorInner(buffer_mgr, ColumnVectorTipe::kReadOnly, row_count);
}

ColumnVector BlockColumnEntry::GetColumnVectorInner(BufferManager *buffer_mgr, const ColumnVectorTipe tipe, SizeT row_count) {
    if (this->buffer_ == nullptr) {
        // Get buffer handle from buffer manager
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_entry_->block_dir(),
                                                      this->filename_,
                                                      0,
                                                      buffer_mgr->persistence_manager());
        this->buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
        buffer_->AddObjRc();
    }

    ColumnVector column_vector(column_type_);
    return column_vector;
}

Vector<String> BlockColumnEntry::FilePaths() const {
    Vector<String> res = {VirtualStore::ConcatenatePath(*FileDir(), *filename_)};
    for (SizeT file_idx = 0; file_idx < outline_buffers_.size(); ++file_idx) {
        String outline_file_path = *OutlineFilename(file_idx);
        res.push_back(VirtualStore::ConcatenatePath(*FileDir(), outline_file_path));
    }
    return res;
}

void BlockColumnEntry::Flush(BlockColumnEntry *block_column_entry, SizeT start_row_count, SizeT checkpoint_row_count) {
    // TODO: Opt, Flush certain row_count content
    DataType *column_type = block_column_entry->column_type_.get();
    switch (column_type->type()) {
        case LogicalType::kBoolean:
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kFloat:
        case LogicalType::kDouble:
        case LogicalType::kFloat16:
        case LogicalType::kBFloat16:
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kDateTime:
        case LogicalType::kTimestamp:
        case LogicalType::kInterval:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
            //        case kBitmap:
        case LogicalType::kUuid:
        case LogicalType::kEmbedding:
        case LogicalType::kRowID: {
            //            SizeT buffer_size = row_count * column_type->Size();
            LOG_TRACE(fmt::format("Saving {}", block_column_entry->column_id()));
            block_column_entry->buffer_->Save();
            LOG_TRACE(fmt::format("Saved {}", block_column_entry->column_id()));

            break;
        }
        case LogicalType::kArray:
        case LogicalType::kTensor:
        case LogicalType::kSparse:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kVarchar: {
            //            SizeT buffer_size = row_count * column_type->Size();
            LOG_TRACE(fmt::format("Saving column {}", block_column_entry->column_id()));
            block_column_entry->buffer_->Save();
            LOG_TRACE(fmt::format("Saved column {}", block_column_entry->column_id()));

            std::shared_lock lock(block_column_entry->mutex_);
            for (auto &outline_buffer : block_column_entry->outline_buffers_) {
                if (outline_buffer != nullptr) {
                    outline_buffer->Save();
                }
            }
            break;
        }
        case LogicalType::kTuple:
            //        case kPath:
            //        case kPolygon:
            //        case kBlob:
        case LogicalType::kEmptyArray:
        case LogicalType::kMixed:
        case LogicalType::kNull: {
            LOG_ERROR(fmt::format("{} isn't supported", column_type->ToString()));
            String error_message = "Not implement: Invalid data type.";
            UnrecoverableError(error_message);
        }
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            LOG_ERROR(fmt::format("Invalid data type {}", column_type->ToString()));
            String error_message = "Invalid data type.";
            UnrecoverableError(error_message);
        }
    }
}

void BlockColumnEntry::FlushColumnCheck(TxnTimeStamp checkpoint_ts) {
    if (deleted_) {
        return;
    }
    bool flush_column = false;
    bool flush_version = false;
    block_entry_->CheckFlush(checkpoint_ts, flush_column, flush_version, true, true);
    if (!flush_column) {
        return;
    }
    this->FlushColumn(checkpoint_ts);
}

void BlockColumnEntry::FlushColumn(TxnTimeStamp checkpoint_ts) {
    if (deleted_) {
        return;
    }
    SizeT row_cnt = block_entry_->row_count(checkpoint_ts);
    BlockColumnEntry::Flush(this, 0, row_cnt);
}

void BlockColumnEntry::DropColumn() {
    buffer_->SubObjRc();
    for (auto *outline_buffer : outline_buffers_) {
        outline_buffer->SubObjRc();
    }
    deleted_ = true;
}

void BlockColumnEntry::Cleanup(CleanupInfoTracer *info_tracer, [[maybe_unused]] bool dropped) {
    if (buffer_ != nullptr) {
        buffer_->PickForCleanup();
        if (info_tracer != nullptr) {
            String file_path = buffer_->GetFilename();
            info_tracer->AddCleanupInfo(std::move(file_path));
        }
    }
    for (auto &outline_buffer : outline_buffers_) {
        if (outline_buffer != nullptr) {
            outline_buffer->PickForCleanup();
            if (info_tracer != nullptr) {
                String file_path = outline_buffer->GetFilename();
                info_tracer->AddCleanupInfo(std::move(file_path));
            }
            outline_buffer = nullptr;
        }
    }
}

Vector<String> BlockColumnEntry::GetFilePath(Txn *txn) const { return FilePaths(); }

nlohmann::json BlockColumnEntry::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = this->column_id_;
    {
        std::shared_lock lock(mutex_);
        json_res["next_outline_idx"] = outline_buffers_.size();
        json_res["last_chunk_offset"] = this->LastChunkOff();
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
        return;
    }
    this->txn_id_ = txn_id;
    this->Commit(commit_ts);
}

void BlockColumnEntry::FillWithDefaultValue(SizeT row_count, const Value *default_value, BufferManager *buffer_mgr) {
    ColumnVector column_vector = this->GetColumnVector(buffer_mgr, row_count);

    for (SizeT i = 0; i < row_count; ++i) {
        column_vector.SetValue(i, *default_value);
    }
}

SizeT BlockColumnEntry::GetStorageSize() const {
    SizeT result = 0;

    {
        std::shared_lock lock(mutex_);
        result += buffer_->GetBufferSize();
        for (BufferObj *outline_buffer : outline_buffers_) {
            result += outline_buffer->GetBufferSize();
        }
    }

    return result;
}

void BlockColumnEntry::ToMmap() {
    buffer_->ToMmap();
    for (auto *outline_buffer : outline_buffers_) {
        outline_buffer->ToMmap();
    }
}

} // namespace infinity
