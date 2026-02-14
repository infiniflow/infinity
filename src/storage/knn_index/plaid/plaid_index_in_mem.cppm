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

// Forward declarations
class PlaidIndexFileWorker;
class PlaidSegmentIndex;

// In-memory PLAID index with incremental update support
// Follows next-plaid's three-mode update strategy:
// 1. Start-from-scratch: < 999 docs, store raw embeddings and rebuild on each update
// 2. Buffer mode: < 100 new embeddings, append without centroid expansion
// 3. Centroid expansion: >= 100 new embeddings, expand centroids and rebuild
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
    
    // next-plaid update mode thresholds
    static constexpr u32 BUFFER_THRESHOLD = 100;           // Buffer mode threshold
    static constexpr u32 APPEND_THRESHOLD = 2000;          // Chunk append threshold
    static constexpr u32 START_FROM_SCRATCH_THRESHOLD = 999; // Full rebuild threshold

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
    // Returns true if index was built during this insert
    bool Insert(const ColumnVector &col, u32 row_offset, u32 row_count, KVInstance &kv_instance, TxnTimeStamp begin_ts, MetaCache *meta_cache);

    // Build index from accumulated data
    // Returns true if index was built, false if not enough data
    bool BuildIndex();

    // Rebuild index from stored raw embeddings (start_from_scratch mode)
    // Use when index quality has degraded
    bool RebuildIndex();

    // Update index with new embeddings after initial build
    // Supports centroid expansion for outlier handling
    // Returns true if update was successful
    bool UpdateIndex(const f32 *embedding_data, u32 embedding_num, bool allow_expansion = true);

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
    
    // ===== Incremental Update Interface (next-plaid style) =====
    
    // Check if we should use start-from-scratch mode (< 999 docs)
    bool ShouldUseStartFromScratch() const;
    
    // Check if we should use buffer mode (< 100 new embeddings)
    bool ShouldUseBufferMode(u32 new_embeddings) const;
    
    // Get current update mode based on existing and new data
    // Returns: 0=start-from-scratch, 1=buffer-mode, 2=centroid-expansion
    int GetUpdateMode(u32 new_doc_count, u32 new_embedding_count) const;
    
    // Dump with incremental update support
    // Supports three modes: start-from-scratch, buffer-mode, centroid-expansion
    Status DumpIncremental(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id);
    
    // Memory usage for triggering dump
    size_t MemUsage() const;
    
    // Get buffered document count
    u32 GetBufferedDocCount() const;
    
    // Get buffered embedding count  
    u32 GetBufferedEmbeddingCount() const;

private:
    // Buffer for incremental updates before index build
    std::vector<std::unique_ptr<f32[]>> buffered_embeddings_;
    std::vector<u32> buffered_doc_lens_;
    u32 buffered_embedding_count_ = 0;

    // For incremental updates after index is built (buffer mode)
    std::vector<u32> buffer_centroid_ids_;
    std::vector<u8> buffer_packed_residuals_;
    std::vector<u32> buffer_doc_lens_;
    u32 buffer_embedding_count_ = 0;
    
    // Raw embeddings storage for start-from-scratch mode
    std::vector<f32> raw_embeddings_storage_;
    std::vector<u32> raw_doc_lens_;
    
    // Incremental capacity tracking
    u32 incremental_embedding_capacity_ = 0;

    // Incrementally update existing index with new data
    void IncrementalUpdate(const f32 *embedding_data, u64 embedding_num);

    // Collect all embeddings (buffered + incremental) into contiguous array
    std::unique_ptr<f32[]> CollectAllEmbeddings(u64 &total_count) const;
    
    // Calculate optimal centroid count (next-plaid formula: 2^floor(log2(16*sqrt(N))))
    static u32 CalcOptimalNCentroids(u32 n_embeddings);
    
    // Internal dump methods for different modes
    Status DumpStartFromScratch(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id);
    Status DumpBufferMode(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id);
    Status DumpCentroidExpansion(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id);
    
    // Prepare data from buffers for dump
    void PrepareDumpData(std::vector<u32> &out_centroid_ids,
                         std::vector<u8> &out_packed_residuals,
                         std::vector<u32> &out_doc_lens,
                         u32 &out_embedding_count);
};

} // namespace infinity
