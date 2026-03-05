// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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
import :buffer_obj;
import :buffer_handle;
import :base_memindex;
import :memindex_tracer;
import :chunk_index_meta;

import column_def;
import internal_types;

namespace infinity {

class IndexBase;
class PlaidIndex;
struct BlockIndex;
struct ColumnVector;
class KVInstance;
class MetaCache;

using PlaidInMemQueryResultType = std::variant<std::pair<u32, u32>, std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>>;

// In-memory PLAID index
// Follows HNSW/EMVB design: holds complete index in memory, dumps as a whole chunk
export class PlaidIndexInMem : public BaseMemIndex {
    const u32 nbits_ = 4;
    const u32 requested_n_centroids_ = 0;
    const u32 embedding_dimension_ = 0;
    const RowID begin_row_id_ = {}; // Original begin row id, never changes
    const std::shared_ptr<ColumnDef> column_def_;

    // row_count_ tracks total rows in current chunk (including before and after BuildIndex)
    u32 row_count_ = 0;
    PlaidIndex *plaid_index_ = nullptr;
    std::atomic_flag is_built_;
    bool own_memory_ = true;
    mutable std::shared_mutex rw_mutex_;
    BufferHandle chunk_handle_; // Like HNSW: holds reference after dump
    u32 build_index_threshold_ = 0;

    // Current begin row id for the next chunk of data
    // Updated after each dump to point to the next new row
    RowID current_begin_row_id_ = {};

public:
    std::string db_name_;
    std::string table_name_;
    std::string index_name_;
    SegmentID segment_id_ = -1;

    static std::shared_ptr<PlaidIndexInMem>
    NewPlaidIndexInMem(const std::shared_ptr<IndexBase> &index_base, const std::shared_ptr<ColumnDef> &column_def, RowID begin_row_id);

    PlaidIndexInMem(u32 nbits, u32 n_centroids, u32 embedding_dimension, RowID begin_row_id, std::shared_ptr<ColumnDef> column_def);

    ~PlaidIndexInMem() override;

    // Returns the current begin row id for the next chunk of data
    // After dump, this is updated to point to the next new row
    RowID GetBeginRowID() const override { return current_begin_row_id_; }
    size_t GetRowCount() const;

    void SetSegmentID(std::string db_id_str, std::string table_id_str, SegmentID segment_id) {
        db_name_ = std::move(db_id_str);
        table_name_ = std::move(table_id_str);
        segment_id_ = segment_id;
    }

    // Insert data into memory index (like HNSW/EMVB)
    void Insert(const ColumnVector &col, u32 row_offset, u32 row_count, KVInstance &kv_instance, TxnTimeStamp begin_ts, MetaCache *meta_cache);

    // Build index from accumulated data (called automatically when threshold reached)
    // Returns true if index was built, false otherwise
    bool BuildIndex();

    // Dump entire index to buffer object (like HNSW/EMVB)
    void Dump(BufferObj *buffer_obj);

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

    // Get pointer to internal index (for disk chunk search)
    PlaidIndex *GetIndex() const { return plaid_index_; }

    // Check if there's buffered data waiting to be built (for incremental dump)
    bool HasBufferedData() const;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

protected:
    MemIndexTracerInfo GetInfo() const override;

private:
    // Buffer for embeddings before index build
    std::vector<std::unique_ptr<f32[]>> buffered_embeddings_;
    std::vector<u32> buffered_doc_lens_;
    u32 buffered_embedding_count_ = 0;
};

} // namespace infinity
