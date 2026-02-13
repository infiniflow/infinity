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

export module infinity_core:plaid_index_in_mem;

import :roaring_bitmap;
import :file_worker;
import :index_file_worker;

import column_def;
import internal_types;

namespace infinity {

class IndexBase;
class PlaidIndex;
struct BlockIndex;
struct ColumnVector;
class KVInstance;
struct ChunkIndexMetaInfo;
class MetaCache;
class PlaidIndexFileWorker;

using PlaidInMemQueryResultType = std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>;

// In-memory PLAID index with incremental update support
export class PlaidIndexInMem {
    const u32 nbits_ = 4;
    const u32 requested_n_centroids_ = 0;
    const u32 embedding_dimension_ = 0;
    const RowID begin_row_id_ = {};
    const std::shared_ptr<ColumnDef> column_def_;

    std::string db_id_str_;
    std::string table_id_str_;

    u32 row_count_ = 0;
    u32 embedding_count_ = 0;
    std::unique_ptr<PlaidIndex> plaid_index_;
    std::atomic_flag is_built_;
    mutable std::shared_mutex rw_mutex_;
    u32 build_index_threshold_ = 0;

public:
    std::string db_name_;
    std::string table_name_;
    std::string index_name_;
    SegmentID segment_id_ = -1;

    static std::shared_ptr<PlaidIndexInMem>
    NewPlaidIndexInMem(const std::shared_ptr<IndexBase> &index_base, const std::shared_ptr<ColumnDef> &column_def, RowID begin_row_id);

    PlaidIndexInMem(u32 nbits, u32 n_centroids, u32 embedding_dimension, RowID begin_row_id, std::shared_ptr<ColumnDef> column_def);

    void SetSegmentID(std::string db_id_str, std::string table_id_str, SegmentID segment_id) {
        db_id_str_ = std::move(db_id_str);
        table_id_str_ = std::move(table_id_str);
        segment_id_ = segment_id;
    }

    RowID GetBeginRowID() const { return begin_row_id_; }
    u32 GetRowCount() const;

    // Insert data - supports incremental updates
    void Insert(const ColumnVector &col, u32 row_offset, u32 row_count, KVInstance &kv_instance, TxnTimeStamp begin_ts, MetaCache *meta_cache);

    // Build index from accumulated data
    void BuildIndex(SegmentMeta &segment_meta);

    void Dump(PlaidIndexFileWorker *index_file_worker);

    // Search
    PlaidInMemQueryResultType SearchWithBitmask(const f32 *query_ptr,
                                                u32 query_embedding_num,
                                                u32 top_n,
                                                Bitmask &bitmask,
                                                const BlockIndex *block_index,
                                                TxnTimeStamp begin_ts,
                                                u32 n_ivf_probe,
                                                f32 centroid_score_threshold,
                                                u32 n_doc_to_score,
                                                u32 n_full_scores) const;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const;

    // Batch insert for efficiency
    void InsertBatch(const std::vector<const ColumnVector *> &cols,
                     const std::vector<u32> &row_offsets,
                     u32 total_row_count,
                     KVInstance &kv_instance,
                     TxnTimeStamp begin_ts,
                     MetaCache *meta_cache);

private:
    // Buffer for incremental updates before index build
    std::vector<std::unique_ptr<f32[]>> buffered_embeddings_;
    std::vector<u32> buffered_doc_lens_;
    u32 buffered_embedding_count_ = 0;
};

} // namespace infinity
