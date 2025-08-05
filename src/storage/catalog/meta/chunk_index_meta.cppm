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

export module infinity_core:chunk_index_meta;

import :stl;
import :status;
import internal_types;
import :third_party;
import :new_catalog;
import :base_meta;

namespace infinity {
export struct ChunkIndexSnapshotInfo;
class KVInstance;
class SegmentIndexMeta;
class BufferObj;

export struct ChunkIndexMetaInfo {
    ChunkIndexMetaInfo() = default;
    ChunkIndexMetaInfo(String base_name, RowID base_row_id, SizeT row_cnt, SizeT index_size)
        : base_name_(base_name), base_row_id_(base_row_id), row_cnt_(row_cnt), index_size_(index_size) {}
    String base_name_{};
    RowID base_row_id_{};
    SizeT row_cnt_{};
    SizeT index_size_{};

    void ToJson(nlohmann::json &json) const;

    void FromJson(std::string_view json_str);

    nlohmann::json Serialize();

    static SharedPtr<ChunkIndexMetaInfo> Deserialize(const nlohmann::json &chunk_index_json);

    static String IndexFileName(ChunkID chunk_id) { return fmt::format("chunk_{}.idx", chunk_id); }
};

export class ChunkIndexMeta : public BaseMeta {
public:
    ChunkIndexMeta(ChunkID chunk_id, SegmentIndexMeta &segment_index_meta);

    ChunkID chunk_id() const { return chunk_id_; }

    SegmentIndexMeta &segment_index_meta() const { return segment_index_meta_; }

    Status GetChunkInfo(KVInstance *kv_instance, ChunkIndexMetaInfo *&chunk_info);

    Status GetIndexBuffer(KVInstance *kv_instance, TxnTimeStamp begin_ts, BufferObj *&index_buffer);

    Status InitSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, const ChunkIndexMetaInfo &chunk_info);

    Status LoadSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status RestoreSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    Status UninitSet(KVInstance *kv_instance, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    Status RestoreSetFromSnapshot(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, const ChunkIndexMetaInfo &chunk_info);

    Status SetChunkInfo(KVInstance *kv_instance, const ChunkIndexMetaInfo &chunk_info);

    Status SetChunkInfoNoPutKV(const ChunkIndexMetaInfo &chunk_info);

    Status FilePaths(KVInstance *kv_instance, TxnTimeStamp begin_ts, Vector<String> &paths);

    Tuple<SharedPtr<ChunkIndexSnapshotInfo>, Status> MapMetaToSnapShotInfo(KVInstance* kv_instance, ChunkID chunk_id);

private:
    Status LoadChunkInfo(KVInstance *kv_instance);

    Status LoadIndexBuffer(KVInstance *kv_instance, TxnTimeStamp begin_ts);

    String GetChunkIndexTag(const String &tag) const;

private:
    mutable std::mutex mtx_;

    SegmentIndexMeta &segment_index_meta_;
    ChunkID chunk_id_;

    Optional<ChunkIndexMetaInfo> chunk_info_;

    BufferObj *index_buffer_ = nullptr;
};

} // namespace infinity
