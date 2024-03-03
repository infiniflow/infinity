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
import catalog_delta_entry;
import internal_types;
import data_type;

namespace infinity {

BlockColumnEntry::BlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, const SharedPtr<String> &base_dir_ref)
    : BaseEntry(EntryType::kBlockColumn), block_entry_(block_entry), column_id_(column_id), base_dir_(base_dir_ref) {}

UniquePtr<BlockColumnEntry> BlockColumnEntry::NewBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, Txn *txn) {
    UniquePtr<BlockColumnEntry> block_column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id, block_entry->base_dir());
    if (txn != nullptr) {
        auto operation = MakeUnique<AddColumnEntryOp>(block_column_entry.get());
        txn->AddCatalogDeltaOperation(std::move(operation));
    }
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

    auto buffer_manager = txn->buffer_manager();
    block_column_entry->buffer_ = buffer_manager->Allocate(std::move(file_worker));

    return block_column_entry;
}

UniquePtr<BlockColumnEntry>
BlockColumnEntry::NewReplayBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, BufferManager *buffer_manager) {
    UniquePtr<BlockColumnEntry> column_entry = MakeUnique<BlockColumnEntry>(block_entry, column_id, block_entry->base_dir());
    column_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");
    column_entry->column_type_ = block_entry->GetColumnType(column_id);

    DataType *column_type = column_entry->column_type_.get();
    SizeT row_capacity = block_entry->row_capacity();
    SizeT total_data_size = (column_type->type() == kBoolean) ? ((row_capacity + 7) / 8) : (row_capacity * column_type->Size());
    auto file_worker = MakeUnique<DataFileWorker>(column_entry->base_dir_, column_entry->file_name_, total_data_size);

    column_entry->buffer_ = buffer_manager->Get(std::move(file_worker));

    return column_entry;
}

ColumnVector BlockColumnEntry::GetColumnVector(BufferManager *buffer_mgr) {
    if (this->buffer_ == nullptr) {
        // Get buffer handle from buffer manager
        auto file_worker = MakeUnique<DataFileWorker>(this->base_dir_, this->file_name_, 0);
        this->buffer_ = buffer_mgr->Get(std::move(file_worker));
    }

    ColumnVector column_vector(column_type_);
    column_vector.Initialize(buffer_mgr, this, block_entry_->row_count());
    return column_vector;
}

void BlockColumnEntry::Append(const ColumnVector *input_column_vector, u16 input_column_vector_offset, SizeT append_rows, BufferManager *buffer_mgr) {
    if (buffer_ == nullptr) {
        UnrecoverableError("Not initialize buffer handle");
    }
    ColumnVector &&column_vector = GetColumnVector(buffer_mgr);
    column_vector.AppendWith(*input_column_vector, input_column_vector_offset, append_rows);
}

void BlockColumnEntry::Flush(BlockColumnEntry *block_column_entry, SizeT checkpoint_row_count) {
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
            std::shared_lock lock(block_column_entry->mutex_);
            for (auto *outline_buffer : block_column_entry->outline_buffers_) {
                if (outline_buffer && outline_buffer->Save()) {
                    outline_buffer->Sync();
                    outline_buffer->CloseFile();
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
            LOG_ERROR(fmt::format("{} isn't supported", column_type->ToString()));
            UnrecoverableError("Not implement: Invalid data type.");
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR(fmt::format("Invalid data type {}", column_type->ToString()));
            UnrecoverableError("Invalid data type.");
        }
    }
}

void BlockColumnEntry::Cleanup() {
    if (buffer_ != nullptr) {
        buffer_->Cleanup();
    }
    for (auto *outline_buffer : outline_buffers_) {
        if (outline_buffer) {
            outline_buffer->Cleanup();
        }
    }
}

nlohmann::json BlockColumnEntry::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = this->column_id_;
    {
        std::shared_lock lock(mutex_);
        json_res["next_outline_idx"] = outline_buffers_.size();
    }

    json_res["commit_ts"] = TxnTimeStamp(this->commit_ts_);
    json_res["begin_ts"] = TxnTimeStamp(this->begin_ts_);
    json_res["txn_id"] = TransactionID(this->txn_id_);
    return json_res;
}

UniquePtr<BlockColumnEntry>
BlockColumnEntry::Deserialize(const nlohmann::json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr) {
    ColumnID column_id = column_data_json["column_id"];
    UniquePtr<BlockColumnEntry> block_column_entry = NewReplayBlockColumnEntry(block_entry, column_id, buffer_mgr);
    block_column_entry->outline_buffers_.assign(SizeT(column_data_json["next_outline_idx"]), nullptr);

    block_column_entry->commit_ts_ = column_data_json["commit_ts"];
    block_column_entry->begin_ts_ = column_data_json["begin_ts"];
    block_column_entry->txn_id_ = column_data_json["txn_id"];

    return block_column_entry;
}

Vector<String> BlockColumnEntry::OutlinePaths() const {
    Vector<String> outline_paths;
    SizeT outline_file_count = 0;
    {
        std::shared_lock lock(mutex_);
        outline_file_count = outline_buffers_.size();
    }
    for (SizeT i = 0; i < outline_file_count; ++i) {
        auto outline_file = OutlineFilename(i);

        outline_paths.push_back(LocalFileSystem::ConcatenateFilePath(*base_dir_, *outline_file));
    }
    return outline_paths;
}

} // namespace infinity
