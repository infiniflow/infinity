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

export module infinity_core:plaid_index;

import :infinity_type;
import :new_catalog;
import :plaid_global_centroids;

import std;
import column_def;
import internal_types;

namespace infinity {

class LocalFileHandle;
struct BlockIndex;
class PlaidQuantizer;
class SegmentMeta;

using PlaidQueryResultType = std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>;

// PLAID Index for multi-vector (tensor) retrieval
// Follows HNSW/EMVB design: complete index that can be dumped/loaded as a whole chunk
// Supports global centroids for efficient incremental updates and merging
export class PlaidIndex {
public:
    // Constructor for memory-based index
    PlaidIndex(u32 start_segment_offset, u32 embedding_dimension, u32 nbits, u32 n_centroids = 0);

    // Constructor for mmap-based index
    PlaidIndex(u32 start_segment_offset, u32 embedding_dimension, u32 nbits, u32 n_centroids, void *mmap_addr, size_t mmap_size);

    ~PlaidIndex();

    // Disable copy
    PlaidIndex(const PlaidIndex &) = delete;
    PlaidIndex &operator=(const PlaidIndex &) = delete;

    // Enable move
    PlaidIndex(PlaidIndex &&other);
    PlaidIndex &operator=(PlaidIndex &&other);

    // Build index from segment data
    void BuildPlaidIndex(const RowID base_rowid, u32 row_count, SegmentMeta &segment_meta, const std::shared_ptr<ColumnDef> &column_def);

    // Train the index with embedding data
    void Train(u32 centroids_num, const f32 *embedding_data, u64 embedding_num, u32 iter_cnt = 4);

    // Add multiple documents' embeddings in batch (computes centroid assignments internally)
    void AddMultipleDocsEmbeddings(const f32 *embedding_data, const std::vector<u32> &doc_lens);

    // Add multiple documents' embeddings with pre-computed centroid IDs (for incremental updates)
    void AddMultipleDocsEmbeddingsWithCentroids(const f32 *embedding_data,
                                                const std::vector<u32> &doc_lens,
                                                const u32 *centroid_ids,
                                                const u8 *packed_residuals,
                                                u32 packed_dim);

    // Copy centroids from external source (for global centroids support)
    // preserve_ivf_lists = true: for merge
    // preserve_ivf_lists = false: build from scratch
    void CopyCentroidsFrom(const std::vector<f32> &centroids_data,
                           const std::vector<f32> &centroid_norms_neg_half,
                           u32 n_centroids,
                           const PlaidQuantizer *quantizer = nullptr,
                           bool preserve_ivf_lists = false);

    // Share global centroids (avoids copying, preferred for incremental updates)
    void ShareGlobalCentroids(std::shared_ptr<PlaidGlobalCentroids> global_centroids);

    // Search with bitmask filtering
    PlaidQueryResultType SearchWithBitmask(const f32 *query_ptr,
                                           u32 query_embedding_num,
                                           u32 top_n,
                                           Bitmask &bitmask,
                                           const BlockIndex *block_index,
                                           TxnTimeStamp begin_ts,
                                           u32 n_ivf_probe,
                                           f32 centroid_score_threshold,
                                           u32 n_doc_to_score,
                                           u32 n_full_scores) const;

    // Save/Load
    void SaveIndexInner(LocalFileHandle &file_handle) const;
    void ReadIndexInner(LocalFileHandle &file_handle);

    // Mmap support
    void SaveToPtr(void *ptr, size_t &offset) const;
    void LoadFromPtr(void *ptr, size_t mmap_size, size_t file_size);

    // Memory usage
    size_t CalcSize() const;
    size_t MemUsage() const;

    // Accessors
    u32 GetDocNum() const { return n_docs_.load(); }
    u32 GetTotalEmbeddingNum() const { return n_total_embeddings_; }
    u32 GetNCentroids() const { return n_centroids_; }
    u32 GetEmbeddingDimension() const { return embedding_dimension_; }
    bool IsMmap() const { return is_mmap_; }

    // Access centroids (for global centroids sharing)
    const std::vector<f32> &centroids_data() const { return global_centroids_ref_ ? global_centroids_ref_->centroids_data() : centroids_data_; }
    const std::vector<f32> &centroid_norms_neg_half() const {
        return global_centroids_ref_ ? global_centroids_ref_->centroid_norms_neg_half() : centroid_norms_neg_half_;
    }

    // Get document length (number of tokens)
    u32 GetDocLen(u32 doc_id) const;

    // Merge multiple chunk indexes directly (without loading raw embeddings)
    // This is the key optimization: merge quantized data (centroid_ids + packed_residuals)
    // instead of reading from column store
    void MergeChunks(const std::vector<std::pair<u32, const PlaidIndex *>> &chunks_with_doc_offsets);

    // Streaming merge: merge one chunk at a time (memory efficient for large indexes)
    // InitializeMerge(n_centroids) -> MergeOneChunk() x N -> FinalizeMerge()
    void InitializeMerge(u32 n_centroids);
    void MergeOneChunk(const PlaidIndex *chunk, u32 doc_offset);
    void FinalizeMerge();

    // Accessors for merge operation (needed for direct data access)
    const std::vector<u32> &centroid_ids() const { return centroid_ids_; }
    const u8 *packed_residuals() const { return packed_residuals_.get(); }
    size_t packed_residuals_size() const { return packed_residuals_size_; }
    const std::vector<u32> &doc_lens() const { return doc_lens_; }
    const std::vector<u32> &doc_offsets() const { return doc_offsets_; }
    const std::vector<std::vector<u32>> &ivf_lists() const { return ivf_lists_; }

    // Accept merged data from disk merger - efficient move semantics
    void AcceptMergedData(std::vector<u32> &&doc_lens,
                          std::vector<u32> &&doc_offsets,
                          std::vector<u32> &&centroid_ids,
                          std::unique_ptr<u8[]> &&packed_residuals,
                          size_t packed_residuals_size,
                          std::vector<std::vector<u32>> &&ivf_lists,
                          u32 total_docs,
                          u64 total_embeddings);

    // Flatten IVF lists from nested vector to contiguous arrays for cache-friendly search
    // Call after all data is added (before dump or after merge)
    void FinalizeIVF();

    // Check if IVF is flattened (for search path selection)
    bool IsIVFFlattened() const { return ivf_flattened_; }

    // Access flattened IVF data for a centroid
    const u32 *GetIVFListData(u32 centroid_id) const;
    u32 GetIVFListLength(u32 centroid_id) const;

    // Outlier-based Centroid Expansion for incremental updates
    // Finds embeddings far from existing centroids, clusters them into new centroids
    // Returns number of new centroids added
    u32 ExpandCentroids(const f32 *new_embeddings, u64 n_new_embeddings, f32 cluster_threshold);

    // Find outlier embeddings (distance > threshold^2 from nearest centroid)
    // Returns indices of outlier embeddings
    std::vector<u64> FindOutliers(const f32 *embeddings, u64 n_embeddings, f32 threshold_sq) const;

public:
    // Fixed parameters (set at construction)
    const u32 start_segment_offset_ = 0;
    const u32 embedding_dimension_ = 0;
    const u32 nbits_ = 4;
    const u32 requested_n_centroids_ = 0; // 0 = auto

    // Batch size for batched IVF probing (0 = auto, based on n_centroids)
    static constexpr u32 DEFAULT_CENTROID_BATCH_SIZE = 4096;
    u32 centroid_batch_size_ = DEFAULT_CENTROID_BATCH_SIZE;

    // Trained parameters
    u32 n_centroids_ = 0;
    std::vector<f32> centroids_data_;          // [n_centroids_, embedding_dimension_]
    std::vector<f32> centroid_norms_neg_half_; // [n_centroids_] for fast L2 computation

    // Optional: shared global centroids reference (avoids copying for incremental updates)
    std::shared_ptr<PlaidGlobalCentroids> global_centroids_ref_;

    // Document metadata
    std::atomic_uint32_t n_docs_ = 0;
    u32 n_total_embeddings_ = 0;
    std::vector<u32> doc_lens_;    // [n_docs_] length of each document
    std::vector<u32> doc_offsets_; // [n_docs_] offset of each doc in centroid_ids_

    // Embedding data
    std::vector<u32> centroid_ids_;          // [n_total_embeddings_] centroid assignment for each embedding
    std::unique_ptr<u8[]> packed_residuals_; // Quantized residuals
    size_t packed_residuals_size_ = 0;
    size_t packed_residuals_capacity_ = 0; // Tracked capacity for amortized growth in MergeOneChunk

    // Inverted index: centroid -> doc ids
    // Legacy: nested vector (used during build/incremental update)
    std::vector<std::vector<u32>> ivf_lists_; // [n_centroids_] posting lists
    // Flattened: contiguous storage (used after FinalizeIVF() for cache-friendly search)
    std::vector<u32> ivf_data_;    // contiguous posting list entries
    std::vector<u32> ivf_offsets_; // [n_centroids_] start offset of each list in ivf_data_
    std::vector<u32> ivf_lengths_; // [n_centroids_] length of each list
    bool ivf_flattened_ = false;   // true when using flattened IVF

    // Quantizer
    std::unique_ptr<PlaidQuantizer> quantizer_;

    // Mmap support
    void *mmap_addr_ = nullptr;
    size_t mmap_size_ = 0;
    bool is_mmap_ = false;
    bool owns_data_ = true;

    // Thread safety
    mutable std::shared_mutex rw_mutex_;

    // Private methods
    u64 ExpectLeastTrainingDataNum() const;

    PlaidQueryResultType GetQueryResultWithBitmask(const f32 *query_ptr,
                                                   u32 query_embedding_num,
                                                   u32 n_ivf_probe,
                                                   f32 centroid_score_threshold,
                                                   u32 n_doc_to_score,
                                                   u32 n_full_scores,
                                                   u32 top_k,
                                                   f32 threshold_final,
                                                   Bitmask &bitmask,
                                                   u32 start_segment_offset,
                                                   const BlockIndex *block_index,
                                                   TxnTimeStamp begin_ts) const;

    // Batched search path: memory-efficient for large centroid counts
    PlaidQueryResultType GetQueryResultBatched(const f32 *query_ptr,
                                               u32 query_embedding_num,
                                               u32 n_ivf_probe,
                                               f32 centroid_score_threshold,
                                               u32 n_doc_to_score,
                                               u32 n_full_scores,
                                               u32 top_k,
                                               Bitmask &bitmask,
                                               u32 start_segment_offset) const;

    // Compute approximate score using centroid lookups
    f32 ApproximateScore(const u32 *doc_centroid_ids, u32 doc_len, const f32 *query_centroid_scores, u32 n_query_tokens) const;

    // Compute exact score with dequantization
    f32 ExactScore(const f32 *query_ptr, u32 n_query_tokens, u32 doc_id, const f32 *centroid_distances) const;

    // Helper for batch centroid scoring
    std::unique_ptr<f32[]> ComputeQueryCentroidScores(const f32 *query_ptr, u32 n_query_tokens) const;

    // Batched IVF Probing: processes centroids in chunks to bound memory usage
    // Returns: set of centroid IDs to probe (union of top-k across all query tokens)
    // Each query token maintains its own top-k heap; final result is the union
    void BatchedIVFProbe(const f32 *query_ptr,
                         u32 n_query_tokens,
                         u32 n_ivf_probe,
                         f32 centroid_score_threshold,
                         u32 centroid_batch_size,
                         std::vector<u32> &probed_centroids,
                         std::unique_ptr<f32[]> &sparse_centroid_scores,
                         u32 &n_sparse_centroids) const;

    // Compute approximate score using sparse centroid score lookup (for batched path)
    f32 ApproximateScoreSparse(const u32 *doc_centroid_ids,
                               u32 doc_len,
                               const f32 *sparse_scores,
                               const u32 *sparse_centroid_id_map,
                               u32 n_sparse_centroids,
                               u32 n_query_tokens) const;

    // Static helper: compute auto n_centroids using next-plaid formula
    static u32 ComputeAutoNCentroids(u64 embedding_count);
};

} // namespace infinity
