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

export module infinity_core:column_meta;

import :stl;
import :status;
import :new_catalog;
import column_def;
import :base_meta;
import :snapshot_info;

namespace infinity {

class BlockMeta;
class KVInstance;
class BufferObj;
// class ColumnDef;

export class ColumnMeta : public BaseMeta {
public:
    ColumnMeta(SizeT column_idx, BlockMeta &block_meta);

    BlockMeta &block_meta() const { return block_meta_; }

    SizeT column_idx() const { return column_idx_; }

    Status InitSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status LoadSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status RestoreSet(KVInstance *kv_instance, const ColumnDef *column_def);

    Status UninitSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, const ColumnDef *column_def, UsageFlag usage_flag);

    Status
    GetColumnBuffer(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, BufferObj *&column_buffer, BufferObj *&outline_buffer);

    Tuple<SharedPtr<ColumnDef>, Status> GetColumnDef(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) const;

    Tuple<SizeT, Status> GetColumnSize(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, SizeT row_cnt) const;

    Status FilePaths(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Vector<String> &paths);

    Tuple<SharedPtr<BlockColumnSnapshotInfo>, Status> MapMetaToSnapShotInfo(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status RestoreFromSnapshot(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, ColumnID column_id);

private:
    Status GetColumnBuffer(KVInstance *kv_instance,
                           TxnTimeStamp begin_ts,
                           TxnTimeStamp commit_ts,
                           BufferObj *&column_buffer,
                           BufferObj *&outline_buffer,
                           const ColumnDef *column_def);

    Status LoadChunkOffset(KVInstance *kv_instance);

    Status LoadColumnBuffer(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, const ColumnDef *col_def);

    String GetColumnTag(const String &tag) const;

private:
    BlockMeta &block_meta_;
    SizeT column_idx_;

    Optional<SizeT> chunk_offset_;

    BufferObj *column_buffer_ = nullptr;
    BufferObj *outline_buffer_ = nullptr;
};

} // namespace infinity
