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
    auto column_id = col_def->id();

    auto block_dir = block_meta_.GetBlockDir();
    auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    {
        auto file_name = fmt::format("{}.col", column_id);
        size_t total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir, file_name));
        auto file_worker = std::make_unique<DataFileWorker>(rel_file_path, total_data_size);
        column_buffer_ = fileworker_mgr->EmplaceFileWorker(std::move(file_worker));
    }
    auto buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto file_name = fmt::format("col_{}_out", column_id);
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir, file_name));
        auto outline_file_worker = std::make_unique<VarFileWorker>(rel_file_path, 0 /*buffer_size*/);
        outline_buffer_ = fileworker_mgr->EmplaceFileWorker(std::move(outline_file_worker));
    }
    return Status::OK();
}

Status ColumnMeta::LoadSet() {
    std::shared_ptr<ColumnDef> col_def;
    {
        auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx_];
    }

    auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    std::shared_ptr<std::string> block_dir_ptr = block_meta_.GetBlockDir();
    {
        auto filename = fmt::format("{}.col", col_def->id());
        size_t total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (block_meta_.block_capacity() + 7) / 8;
        } else {
            total_data_size = block_meta_.block_capacity() * col_def->type()->Size();
        }
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir_ptr, filename));
        auto file_worker = std::make_unique<DataFileWorker>(rel_file_path, total_data_size);
        column_buffer_ = fileworker_mgr->EmplaceFileWorker(std::move(file_worker));
    }
    auto buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = fmt::format("col_{}_out", col_def->id());

        size_t chunk_offset = 0;
        auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir_ptr, filename));
        auto outline_file_worker = std::make_unique<VarFileWorker>(rel_file_path, chunk_offset);
        outline_buffer_ = fileworker_mgr->EmplaceFileWorker(std::move(outline_file_worker));
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
    auto *col_def = (*column_defs_ptr)[column_idx_].get();
    auto column_id = col_def->id();

    auto col_filename = fmt::format("{}.col", column_id);
    paths.push_back(*block_meta_.GetBlockDir() + "/" + col_filename);

    {
        auto buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            auto outline_filename = fmt::format("col_{}_out", column_id);
            paths.push_back(*block_meta_.GetBlockDir() + "/" + outline_filename);
        }
    }

    return Status::OK();
}

Status ColumnMeta::GetColumnBuffer(FileWorker *&column_buffer, FileWorker *&outline_buffer, const std::shared_ptr<ColumnDef> &column_def) {
    if (!column_buffer_) {
        auto status = LoadColumnBuffer(column_def);
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

Status ColumnMeta::UninitSet(const std::shared_ptr<ColumnDef> &column_def, UsageFlag usage_flag) {

    if (usage_flag == UsageFlag::kOther) {
        auto status = GetColumnBuffer(column_buffer_, outline_buffer_, column_def);
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

Status ColumnMeta::LoadColumnBuffer(std::shared_ptr<ColumnDef> column_def) {
    auto block_dir = block_meta_.GetBlockDir();
    if (!column_def) {
        auto [column_defs_ptr, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        column_def = (*column_defs_ptr)[column_idx_];
    }
    ColumnID column_id = column_def->id();

    FileWorkerManager *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    {
        auto col_file_name = fmt::format("{}.col", column_id);
        auto col_file_path = fmt::format("{}/{}", *block_dir, col_file_name);
        column_buffer_ = fileworker_mgr->GetFileWorker(col_file_path);
        if (column_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_file_path));
        }
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*column_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto outline_file_name = fmt::format("col_{}_out", column_id);
        auto outline_file_path = fmt::format("{}/{}", *block_dir, outline_file_name);
        outline_buffer_ = fileworker_mgr->GetFileWorker(outline_file_path);
        if (outline_buffer_ == nullptr) {
            return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_file_path));
        }
    }
    return Status::OK();
}

std::tuple<std::shared_ptr<BlockColumnSnapshotInfo>, Status> ColumnMeta::MapMetaToSnapShotInfo() {
    auto block_column_snapshot_info = std::make_shared<BlockColumnSnapshotInfo>();
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
        const auto &outline_filename = column_file_paths[i];
        auto outline_snapshot_info = std::make_shared<OutlineSnapshotInfo>();
        outline_snapshot_info->filepath_ = outline_filename;
        outline_snapshots.push_back(outline_snapshot_info);
    }
    block_column_snapshot_info->outline_snapshots_ = outline_snapshots;
    return {block_column_snapshot_info, Status::OK()};
}

Status ColumnMeta::RestoreFromSnapshot(ColumnID column_id) {
    auto [column_defs, status] = block_meta_.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    auto col_def = (*column_defs)[column_id];
    status = InitSet(col_def);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

} // namespace infinity
