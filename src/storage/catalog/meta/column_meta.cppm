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
class BufferObj;
// class ColumnDef;

export class ColumnMeta {
public:
    ColumnMeta(size_t column_idx, BlockMeta &block_meta);

    KVInstance &kv_instance() const { return kv_instance_; }

    BlockMeta &block_meta() const { return block_meta_; }

    size_t column_idx() const { return column_idx_; }

    Status GetChunkOffset(size_t &chunk_offset);

    Status SetChunkOffset(size_t chunk_offset);

    Status InitSet();

    Status LoadSet();

    Status RestoreSet(const ColumnDef *column_def);

    Status UninitSet(const ColumnDef *column_def, UsageFlag usage_flag);

    Status GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer);

    std::tuple<std::shared_ptr<ColumnDef>, Status> GetColumnDef() const;

    std::tuple<size_t, Status> GetColumnSize(size_t row_cnt) const;

    Status FilePaths(std::vector<std::string> &paths);

    std::tuple<std::shared_ptr<BlockColumnSnapshotInfo>, Status> MapMetaToSnapShotInfo();

    Status RestoreFromSnapshot(ColumnID column_id);

private:
    Status GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer, const ColumnDef *column_def);

    Status LoadChunkOffset();

    Status LoadColumnBuffer(const ColumnDef *col_def);

    std::string GetColumnTag(const std::string &tag) const;

private:
    KVInstance &kv_instance_;
    BlockMeta &block_meta_;
    size_t column_idx_;

    std::optional<size_t> chunk_offset_;

    BufferObj *column_buffer_ = nullptr;
    BufferObj *outline_buffer_ = nullptr;
};

} // namespace infinity
