// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module column_meta;

import kv_store;
import kv_code;
import block_meta;
import segment_meta;
import table_meeta;
import third_party;

import infinity_context;
import buffer_manager;
import data_file_worker;
import var_file_worker;
import vector_buffer;
import column_vector;

import logical_type;

namespace infinity {

ColumnMeta::ColumnMeta(SizeT column_idx, BlockMeta &block_meta)
    : kv_instance_(block_meta.kv_instance()), block_meta_(block_meta), column_idx_(column_idx) {}

Status ColumnMeta::SetChunkOffset(SizeT chunk_offset) {
    chunk_offset_ = chunk_offset;
    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    Status status = kv_instance_.Put(chunk_offset_key, fmt::format("{}", *chunk_offset_));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status ColumnMeta::InitSet() {
    // Status status = SetChunkOffset(0);
    // if (!status.ok()) {
    //     return status;
    // }

    Status status;
    SharedPtr<ColumnDef> col_def;
    {
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_];
    }
    ColumnID column_id = col_def->id();

    SharedPtr<String> block_dir_ptr = block_meta_.GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    {
        auto filename = MakeShared<String>(fmt::format("{}.col", column_id));
        SizeT total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_dir_ptr,
                                                      filename,
                                                      total_data_size,
                                                      buffer_mgr->persistence_manager());
        column_buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        if (!column_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
        }
        column_buffer_->AddObjRc();
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_id));
        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             filename,
                                                             0, /*buffer_size*/
                                                             buffer_mgr->persistence_manager());
        outline_buffer_ = buffer_mgr->AllocateBufferObject(std::move(outline_file_worker));
        if (!outline_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
        }
        outline_buffer_->AddObjRc();
    }
    return Status::OK();
}

Status ColumnMeta::LoadSet() {
    Status status;
    SharedPtr<ColumnDef> col_def;
    {
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_];
    }

    auto *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    SharedPtr<String> block_dir_ptr = block_meta_.GetBlockDir();
    {
        auto filename = MakeShared<String>(fmt::format("{}.col", col_def->id()));
        SizeT total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_dir_ptr,
                                                      filename,
                                                      total_data_size,
                                                      buffer_mgr->persistence_manager());
        column_buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
        if (!column_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
        }
        column_buffer_->AddObjRc();
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", col_def->id()));

        SizeT chunk_offset = 0;
        // status = this->GetChunkOffset(chunk_offset);
        // if (!status.ok()) {
        //     return status;
        // }

        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             filename,
                                                             chunk_offset,
                                                             buffer_mgr->persistence_manager());
        outline_buffer_ = buffer_mgr->GetBufferObject(std::move(outline_file_worker));
        if (!outline_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
        }
        outline_buffer_->AddObjRc();
    }
    return Status::OK();
}

Status ColumnMeta::RestoreSet(const ColumnDef *column_def) {
    Status status;

    auto *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    SharedPtr<String> block_dir_ptr = block_meta_.GetBlockDir();
    {
        auto filename = MakeShared<String>(fmt::format("{}.col", column_def->id()));
        SizeT total_data_size = 0;
        if (column_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * column_def->type()->Size();
        }
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_dir_ptr,
                                                      filename,
                                                      total_data_size,
                                                      buffer_mgr->persistence_manager());
        auto *buffer_obj = buffer_mgr->GetBufferObject(file_worker->GetFilePath());
        if (buffer_obj == nullptr) {
            column_buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            if (!column_buffer_) {
                return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
            }
            column_buffer_->AddObjRc();
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*column_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_def->id()));

        // NO LONGER USING CHUNK OFFSET
        // SizeT chunk_offset = 0;
        // status = this->GetChunkOffset(chunk_offset);
        // if (!status.ok()) {
        //     return status;
        // }

        // check if 0 is the right buffer size
        // follow loadset
        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             filename,
                                                             0, /*buffer_size*/
                                                             buffer_mgr->persistence_manager());
        auto *buffer_obj = buffer_mgr->GetBufferObject(outline_file_worker->GetFilePath());
        if (buffer_obj == nullptr) {
            outline_buffer_ = buffer_mgr->GetBufferObject(std::move(outline_file_worker));
            if (!outline_buffer_) {
                return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
            }
            outline_buffer_->AddObjRc();
        }
    }
    return Status::OK();
}

Status ColumnMeta::GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer) {
    return GetColumnBuffer(column_buffer, outline_buffer, nullptr);
}

Status ColumnMeta::FilePaths(Vector<String> &paths) {
    auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    ColumnDef *col_def = (*column_defs_ptr)[column_idx_].get();
    ColumnID column_id = col_def->id();

    String col_filename = std::to_string(column_id) + ".col";
    paths.push_back(*block_meta_.GetBlockDir() + "/" + col_filename);

    {
        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            String outline_filename = fmt::format("col_{}_out_0", column_id);
            paths.push_back(*block_meta_.GetBlockDir() + "/" + outline_filename);
        }
    }

    return Status::OK();
}

Status ColumnMeta::GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer, const ColumnDef *column_def) {
    if (!column_buffer_) {
        Status status = LoadColumnBuffer(column_def);
        if (!status.ok()) {
            return status;
        }
    }
    column_buffer = column_buffer_;
    outline_buffer = outline_buffer_;
    return Status::OK();
}

Status ColumnMeta::UninitSet(const ColumnDef *column_def, UsageFlag usage_flag) {
    Status status;

    if (usage_flag == UsageFlag::kOther) {
        status = this->GetColumnBuffer(column_buffer_, outline_buffer_, column_def);
        if (!status.ok()) {
            return status;
        }
        column_buffer_->PickForCleanup();
        if (outline_buffer_) {
            outline_buffer_->PickForCleanup();
        }
    }

    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    status = kv_instance_.Delete(chunk_offset_key);
    if (!status.ok()) {
        return status;
    }
    chunk_offset_.reset();
    return Status::OK();
}

Status ColumnMeta::LoadChunkOffset() {
    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    String chunk_offset_str;
    Status status = kv_instance_.Get(chunk_offset_key, chunk_offset_str);
    if (!status.ok()) {
        return status;
    }
    chunk_offset_ = std::stoull(chunk_offset_str);
    return Status::OK();
}

Status ColumnMeta::LoadColumnBuffer(const ColumnDef *col_def) {
    SharedPtr<String> block_dir_ptr = block_meta_.GetBlockDir();
    if (!col_def) {
        auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_].get();
    }
    ColumnID column_id = col_def->id();

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    {
        String col_filename = std::to_string(column_id) + ".col";
        String col_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + col_filename;
        column_buffer_ = buffer_mgr->GetBufferObject(col_filepath);
        if (column_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_filepath));
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        String outline_filename = fmt::format("col_{}_out_0", column_id);
        String outline_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + outline_filename;
        outline_buffer_ = buffer_mgr->GetBufferObject(outline_filepath);
        if (outline_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_filepath));
        }
    }
    return Status::OK();
}

String ColumnMeta::GetColumnTag(const String &tag) const {
    SegmentMeta &segment_meta = block_meta_.segment_meta();
    TableMeeta &table_meta = segment_meta.table_meta();
    return KeyEncode::CatalogTableSegmentBlockColumnTagKey(table_meta.db_id_str(),
                                                           table_meta.table_id_str(),
                                                           segment_meta.segment_id(),
                                                           block_meta_.block_id(),
                                                           column_idx_,
                                                           tag);
}

Tuple<SharedPtr<BlockColumnSnapshotInfo>, Status> ColumnMeta::MapMetaToSnapShotInfo(){
    SharedPtr<BlockColumnSnapshotInfo> block_column_snapshot_info = MakeShared<BlockColumnSnapshotInfo>();
    block_column_snapshot_info->column_id_ = column_idx_;
    Vector<String> column_file_paths;
    auto status = FilePaths(column_file_paths);
    if (!status.ok()) {
        return {nullptr, status};
    }
    block_column_snapshot_info->filepath_ = column_file_paths[0];
    SizeT last_chunk_offset;
    status = this->GetChunkOffset(last_chunk_offset);
    block_column_snapshot_info->last_chunk_offset_ = last_chunk_offset;

    Vector<SharedPtr<OutlineSnapshotInfo>> outline_snapshots;
    // start at the second column file path
    for (SizeT i = 1; i < column_file_paths.size(); ++i) {
        const String &outline_filename = column_file_paths[i];
        SharedPtr<OutlineSnapshotInfo> outline_snapshot_info = MakeShared<OutlineSnapshotInfo>();
        outline_snapshot_info->filepath_ = outline_filename;
        outline_snapshots.push_back(outline_snapshot_info);
    }
    block_column_snapshot_info->outline_snapshots_ = outline_snapshots;
    return {block_column_snapshot_info, Status::OK()};
}

Status ColumnMeta::RestoreFromSnapshot(ColumnID column_id){
    // TODO: figure out whether we are still using chunkoffset
    Status status;
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = block_meta_.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    const ColumnDef *col_def = (*column_defs)[column_id].get();
    status = RestoreSet(col_def);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}





} // namespace infinity
