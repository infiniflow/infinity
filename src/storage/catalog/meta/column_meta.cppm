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

export module infinity_core:column_meta;

import :status;
import :new_catalog;
import :snapshot_info;

import column_def;

namespace infinity {

class BlockMeta;
class KVInstance;

export class ColumnMeta {
public:
    ColumnMeta(size_t column_idx, BlockMeta &block_meta);

    KVInstance &kv_instance() const { return kv_instance_; }

    BlockMeta &block_meta() const { return block_meta_; }

    size_t column_idx() const { return column_idx_; }

    std::shared_ptr<ColumnDef> get_column_def() const;

    Status InitSet(const std::shared_ptr<ColumnDef> &col_def);

    Status LoadSet();

    Status UninitSet(const std::shared_ptr<ColumnDef>& column_def, UsageFlag usage_flag);

    Status GetColumnBuffer(FileWorker *&column_buffer, FileWorker *&outline_buffer);

    std::tuple<size_t, Status> GetColumnSize(size_t row_cnt, const std::shared_ptr<ColumnDef> &col_def) const;

    Status FilePaths(std::vector<std::string> &paths);

    std::tuple<std::shared_ptr<BlockColumnSnapshotInfo>, Status> MapMetaToSnapShotInfo();

    Status RestoreFromSnapshot(ColumnID column_id);

private:
    Status GetColumnBuffer(FileWorker *&column_buffer, FileWorker *&outline_buffer, const std::shared_ptr<ColumnDef>& column_def);

    Status LoadColumnBuffer(std::shared_ptr<ColumnDef> column_def);

private:
    KVInstance &kv_instance_;
    BlockMeta &block_meta_;
    size_t column_idx_;

    std::optional<size_t> chunk_offset_;

    FileWorker *column_buffer_ = nullptr;
    FileWorker *outline_buffer_ = nullptr;
};

} // namespace infinity
