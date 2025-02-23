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

export module segment_index_meta;

import stl;
import status;

namespace infinity {

class KVInstance;
class TableIndexMeeta;

export class SegmentIndexMeta {
public:
    SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta, KVInstance &kv_instance);

    SegmentID segment_id() const { return segment_id_; }

    TableIndexMeeta &table_index_meta() const { return table_index_meta_; }

    KVInstance &kv_instance() const { return kv_instance_; }

    Status GetChunkIDs(Vector<ChunkID> *&chunk_ids) {
        if (!chunk_ids_) {
            Status status = LoadChunkIDs();
            if (!status.ok()) {
                return status;
            }
        }
        chunk_ids = &chunk_ids_.value();
        return Status::OK();
    }

    Status GetNextChunkID(ChunkID &chunk_id) {
        if (!next_chunk_id_) {
            Status status = LoadChunkIDs();
            if (!status.ok()) {
                return status;
            }
        }
        chunk_id = *next_chunk_id_;
        return Status::OK();
    }

    Status SetChunkIDs(const Vector<ChunkID> &chunk_ids);

    Status AddChunkID(ChunkID chunk_id);

    Status SetNextChunkID(ChunkID chunk_id);

    Status InitSet();

private:
    Status LoadChunkIDs();

    Status LoadNextChunkID();

    String GetSegmentIndexTag(const String &tag);

private:
    KVInstance &kv_instance_;
    TableIndexMeeta &table_index_meta_;
    SegmentID segment_id_;

    Optional<Vector<ChunkID>> chunk_ids_;
    Optional<ChunkID> next_chunk_id_;
};

} // namespace infinity