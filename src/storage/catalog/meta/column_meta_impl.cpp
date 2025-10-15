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

module infinity_core:column_meta.impl;

import :column_meta;
import :kv_store;
import :kv_code;
import :block_meta;
import :segment_meta;
import :table_meta;
import :infinity_context;
import :fileworker_manager;
import :data_file_worker;
import :var_file_worker;
import :vector_buffer;
import :column_vector;

import std;
import third_party;

import logical_type;
import column_def;

namespace infinity {

ColumnMeta::ColumnMeta(size_t column_idx, BlockMeta &block_meta)
    : kv_instance_(block_meta.kv_instance()), block_meta_(block_meta), column_idx_(column_idx) {}

std::shared_ptr<ColumnDef> ColumnMeta::get_column_def() const {
    auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
    std::shared_ptr<ColumnDef> &column_def = column_defs_ptr->at(column_idx_);
    return column_def;
}

Status ColumnMeta::InitSet(const std::shared_ptr<ColumnDef> &col_def) {
    Status status;
    ColumnID column_id = col_def->id();

    std::shared_ptr<std::string> block_dir_ptr = block_meta_.GetBlockDir();
    FileWorkerManager *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    {
        auto filename = std::make_shared<std::string>(fmt::format("{}.col", column_id));
        size_t total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto file_worker = std::make_unique<DataFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                            std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                            block_dir_ptr,
                                                            filename,
                                                            total_data_size,
                                                            fileworker_mgr->persistence_manager());
        column_buffer_ = file_worker.get();
        fileworker_mgr->EmplaceFileWorker(std::move(file_worker));
        if (!column_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = std::make_shared<std::string>(fmt::format("col_{}_out", column_id));
        auto outline_file_worker = std::make_unique<VarFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                                   std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                                   block_dir_ptr,
                                                                   filename,
                                                                   0, /*buffer_size*/
                                                                   fileworker_mgr->persistence_manager());
        fileworker_mgr->EmplaceFileWorker(std::move(outline_file_worker));
        outline_buffer_ = outline_file_worker.get();
        if (!outline_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
        }
    }
    return Status::OK();
}

Status ColumnMeta::LoadSet() {
    Status status;
    std::shared_ptr<ColumnDef> col_def;
    {
        std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_];
    }

    auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    std::shared_ptr<std::string> block_dir_ptr = block_meta_.GetBlockDir();
    {
        auto filename = std::make_shared<std::string>(fmt::format("{}.col", col_def->id()));
        size_t total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto file_worker = std::make_unique<DataFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                            std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                            block_dir_ptr,
                                                            filename,
                                                            total_data_size,
                                                            fileworker_mgr->persistence_manager());
        column_buffer_ = file_worker.get();
        if (!column_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = std::make_shared<std::string>(fmt::format("col_{}_out", col_def->id()));

        size_t chunk_offset = 0;

        auto outline_file_worker = std::make_unique<VarFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                                   std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                                   block_dir_ptr,
                                                                   filename,
                                                                   chunk_offset,
                                                                   fileworker_mgr->persistence_manager());
        outline_buffer_ = outline_file_worker.get();
        if (!outline_buffer_) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
        }
    }
    return Status::OK();
}

Status ColumnMeta::RestoreSet(const ColumnDef *column_def) {
    Status status;

    auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    std::shared_ptr<std::string> block_dir_ptr = block_meta_.GetBlockDir();
    {
        auto filename = std::make_shared<std::string>(fmt::format("{}.col", column_def->id()));
        size_t total_data_size = 0;
        if (column_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * column_def->type()->Size();
        }
        auto file_worker = std::make_unique<DataFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                            std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                            block_dir_ptr,
                                                            filename,
                                                            total_data_size,
                                                            fileworker_mgr->persistence_manager());
        auto *buffer_obj = fileworker_mgr->GetFileWorker(file_worker->GetFilePath());
        if (buffer_obj == nullptr) {
            column_buffer_ = file_worker.get();
            if (!column_buffer_) {
                return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", file_worker->GetFilePath()));
            }
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*column_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = std::make_shared<std::string>(fmt::format("col_{}_out", column_def->id()));

        // check if 0 is the right buffer size
        // follow loadset
        auto outline_file_worker = std::make_unique<VarFileWorker>(std::make_shared<std::string>(InfinityContext::instance().config()->DataDir()),
                                                                   std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                                   block_dir_ptr,
                                                                   filename,
                                                                   0, /*buffer_size*/
                                                                   fileworker_mgr->persistence_manager());
        auto *buffer_obj = fileworker_mgr->GetFileWorker(outline_file_worker->GetFilePath());
        if (buffer_obj == nullptr) {
            outline_buffer_ = outline_file_worker.get();
            if (!outline_buffer_) {
                return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", outline_file_worker->GetFilePath()));
            }
        }
    }
    return Status::OK();
}

Status ColumnMeta::GetColumnBuffer(FileWorker *&column_buffer, FileWorker *&outline_buffer) {
    return GetColumnBuffer(column_buffer, outline_buffer, nullptr);
}

Status ColumnMeta::FilePaths(std::vector<std::string> &paths) {
    auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    ColumnDef *col_def = (*column_defs_ptr)[column_idx_].get();
    ColumnID column_id = col_def->id();

    std::string col_filename = std::to_string(column_id) + ".col";
    paths.push_back(*block_meta_.GetBlockDir() + "/" + col_filename);

    {
        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            std::string outline_filename = fmt::format("col_{}_out", column_id);
            paths.push_back(*block_meta_.GetBlockDir() + "/" + outline_filename);
        }
    }

    return Status::OK();
}

Status ColumnMeta::GetColumnBuffer(FileWorker *&column_buffer, FileWorker *&outline_buffer, const ColumnDef *column_def) {
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

std::tuple<size_t, Status> ColumnMeta::GetColumnSize(size_t row_cnt, const std::shared_ptr<ColumnDef> &col_def) const {

    size_t total_data_size = 0;
    if (col_def->type()->type() == LogicalType::kBoolean) {
        total_data_size = (row_cnt + 7) / 8;
    } else {
        total_data_size = row_cnt * col_def->type()->Size();
    }
    return {total_data_size, Status::OK()};
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

    return Status::OK();
}

Status ColumnMeta::LoadColumnBuffer(const ColumnDef *col_def) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta_.GetBlockDir();
    if (!col_def) {
        auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_].get();
    }
    ColumnID column_id = col_def->id();

    FileWorkerManager *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    {
        std::string col_filename = std::to_string(column_id) + ".col";
        std::string col_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + col_filename;
        column_buffer_ = fileworker_mgr->GetFileWorker(col_filepath);
        if (column_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_filepath));
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        std::string outline_filename = fmt::format("col_{}_out", column_id);
        std::string outline_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + outline_filename;
        outline_buffer_ = fileworker_mgr->GetFileWorker(outline_filepath);
        if (outline_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_filepath));
        }
    }
    return Status::OK();
}

std::tuple<std::shared_ptr<BlockColumnSnapshotInfo>, Status> ColumnMeta::MapMetaToSnapShotInfo() {
    std::shared_ptr<BlockColumnSnapshotInfo> block_column_snapshot_info = std::make_shared<BlockColumnSnapshotInfo>();
    block_column_snapshot_info->column_id_ = column_idx_;
    std::vector<std::string> column_file_paths;
    auto status = FilePaths(column_file_paths);
    if (!status.ok()) {
        return {nullptr, status};
    }
    block_column_snapshot_info->filepath_ = column_file_paths[0];

    std::vector<std::shared_ptr<OutlineSnapshotInfo>> outline_snapshots;
    // start at the second column file path
    for (size_t i = 1; i < column_file_paths.size(); ++i) {
        const std::string &outline_filename = column_file_paths[i];
        std::shared_ptr<OutlineSnapshotInfo> outline_snapshot_info = std::make_shared<OutlineSnapshotInfo>();
        outline_snapshot_info->filepath_ = outline_filename;
        outline_snapshots.push_back(outline_snapshot_info);
    }
    block_column_snapshot_info->outline_snapshots_ = outline_snapshots;
    return {block_column_snapshot_info, Status::OK()};
}

Status ColumnMeta::RestoreFromSnapshot(ColumnID column_id) {
    // TODO: figure out whether we are still using chunkoffset
    Status status;
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
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
