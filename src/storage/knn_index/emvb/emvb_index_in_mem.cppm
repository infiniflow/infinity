// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

export module emvb_index_in_mem;
import stl;
import internal_types;
import roaring_bitmap;

namespace infinity {

class BufferManager;
class ColumnDef;
class IndexBase;
class EMVBIndex;
struct BlockIndex;
class ColumnVector;
class BufferObj;
class KVInstance;
struct ChunkIndexMetaInfo;

using EMVBInMemQueryResultType = Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>>;

// 1. when data count is not enough for training centroids, just keep a record of the embeddings count
// 2. when data count is enough, build the emvb index
export class EMVBIndexInMem {
    const u32 residual_pq_subspace_num_ = 0;
    const u32 residual_pq_subspace_bits_ = 0;
    const u32 embedding_dimension_ = 0;
    const RowID begin_row_id_ = {};
    const SharedPtr<ColumnDef> column_def_;

    String db_id_str_;
    String table_id_str_;

    u32 row_count_ = 0;       // row of tensors
    u32 embedding_count_ = 0; // count of total embeddings
    UniquePtr<EMVBIndex> emvb_index_;
    std::atomic_flag is_built_;
    mutable std::shared_mutex rw_mutex_;
    u32 build_index_threshold_ = 0; // bar for building index

public:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    SegmentID segment_id_ = -1;

    static SharedPtr<EMVBIndexInMem>
    NewEMVBIndexInMem(const SharedPtr<IndexBase> &index_base, const SharedPtr<ColumnDef> &column_def, RowID begin_row_id);

    EMVBIndexInMem(u32 residual_pq_subspace_num,
                   u32 residual_pq_subspace_bits,
                   u32 embedding_dimension,
                   RowID begin_row_id,
                   SharedPtr<ColumnDef> column_def);

    void SetSegmentID(String db_id_str, String table_id_str, SegmentID segment_id) {
        db_id_str_ = std::move(db_id_str);
        table_id_str_ = std::move(table_id_str);
        segment_id_ = segment_id;
    }

    RowID GetBeginRowID() const { return begin_row_id_; }

    u32 GetRowCount() const;

    void Insert(const ColumnVector &col, u32 row_offset, u32 row_count, KVInstance &kv_instance, TxnTimeStamp begin_ts);

    void Dump(BufferObj *buffer_obj);

    // return id: offset in the segment
    std::variant<Pair<u32, u32>, EMVBInMemQueryResultType> SearchWithBitmask(const f32 *query_ptr,
                                                                             u32 query_embedding_num,
                                                                             u32 top_n,
                                                                             Bitmask &bitmask,
                                                                             const BlockIndex *block_index,
                                                                             TxnTimeStamp begin_ts,
                                                                             u32 centroid_nprobe,
                                                                             f32 threshold_first,
                                                                             u32 n_doc_to_score,
                                                                             u32 out_second_stage,
                                                                             f32 threshold_final) const;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const;
};

} // namespace infinity