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

export module chunk_index_meta;

import stl;
import status;
import internal_types;
import third_party;
import new_catalog;

namespace infinity {

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

    static String IndexFileName(ChunkID chunk_id) { return fmt::format("chunk_{}.idx", chunk_id); }
};

export class ChunkIndexMeta {
public:
    ChunkIndexMeta(ChunkID chunk_id, SegmentIndexMeta &segment_index_meta);

    ChunkID chunk_id() const { return chunk_id_; }

    SegmentIndexMeta &segment_index_meta() const { return segment_index_meta_; }

    Status GetChunkInfo(ChunkIndexMetaInfo *&chunk_info) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!chunk_info_) {
            Status status = LoadChunkInfo();
            if (!status.ok()) {
                return status;
            }
        }
        chunk_info = &chunk_info_.value();
        return Status::OK();
    }

    Status GetIndexBuffer(BufferObj *&index_buffer);

    Status InitSet(const ChunkIndexMetaInfo &chunk_info);

    Status LoadSet();

    Status RestoreSet();

    Status UninitSet(UsageFlag usage_flag);

    Status SetChunkInfo(const ChunkIndexMetaInfo &chunk_info);

    Status SetChunkInfoNoPutKV(const ChunkIndexMetaInfo &chunk_info);

    Status FilePaths(Vector<String> &paths);

private:
    Status LoadChunkInfo();

    Status LoadIndexBuffer();

    String GetChunkIndexTag(const String &tag) const;

private:
    mutable std::mutex mtx_;

    KVInstance &kv_instance_;
    SegmentIndexMeta &segment_index_meta_;
    ChunkID chunk_id_;

    Optional<ChunkIndexMetaInfo> chunk_info_;

    BufferObj *index_buffer_ = nullptr;
};

} // namespace infinity
