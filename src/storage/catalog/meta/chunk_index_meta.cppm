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

export module infinity_core:chunk_index_meta;

import :status;
import :new_catalog;

import third_party;

import internal_types;

namespace infinity {
export struct ChunkIndexSnapshotInfo;
class KVInstance;
class SegmentIndexMeta;
// class BMPIndexFileWorker;
// class EMVBIndexFileWorker;
// class HnswFileWorker;
// class IVFIndexFileWorker;
// class SecondaryIndexFileWorker;
class IndexFileWorker;

export struct ChunkIndexMetaInfo {
    ChunkIndexMetaInfo() = default;
    ChunkIndexMetaInfo(std::string base_name, RowID base_row_id, size_t row_cnt, size_t term_cnt, size_t index_size)
        : base_name_(base_name), base_row_id_(base_row_id), row_cnt_(row_cnt), term_cnt_(term_cnt), index_size_(index_size) {}
    std::string base_name_{};
    RowID base_row_id_{};
    size_t row_cnt_{};
    size_t term_cnt_{}; // used only in fulltext index
    size_t index_size_{};

    void ToJson(nlohmann::json &json) const;

    void FromJson(std::string_view json_str);

    static std::string IndexFileName(ChunkID chunk_id) { return fmt::format("chunk_{}.idx", chunk_id); }
};

export class ChunkIndexMeta {
public:
    ChunkIndexMeta(ChunkID chunk_id, SegmentIndexMeta &segment_index_meta);

    ChunkID chunk_id() const { return chunk_id_; }

    SegmentIndexMeta &segment_index_meta() const { return segment_index_meta_; }

    Status GetChunkInfo(ChunkIndexMetaInfo *&chunk_info);

    Status GetFileWorker(IndexFileWorker *&index_file_worker);

    Status InitSet(const ChunkIndexMetaInfo &chunk_info);

    Status LoadSet();

    Status RestoreSetFromSnapshot(const ChunkIndexMetaInfo &chunk_info);

    Status UninitSet(UsageFlag usage_flag);

    Status SetChunkInfo(const ChunkIndexMetaInfo &chunk_info);

    Status FilePaths(std::vector<std::string> &paths);

    std::tuple<std::shared_ptr<ChunkIndexSnapshotInfo>, Status> MapMetaToSnapShotInfo(ChunkID chunk_id);

private:
    Status LoadChunkInfo();

    Status LoadIndexFileWorker();

    std::string GetChunkIndexTag(const std::string &tag) const;

private:
    mutable std::mutex mtx_;

    KVInstance &kv_instance_;
    SegmentIndexMeta &segment_index_meta_;
    ChunkID chunk_id_;

    std::optional<ChunkIndexMetaInfo> chunk_info_;

    IndexFileWorker *index_file_worker_{};
};

} // namespace infinity
