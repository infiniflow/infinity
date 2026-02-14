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

export module infinity_core:plaid_index;

import :infinity_type;
import :new_catalog;

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
// Based on next-plaid implementation with mmap support
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
    // Uses intelligent sampling when embedding_num is large to improve performance
    // Sample size: min(16 * sqrt(120 * embedding_num), embedding_num)
    void Train(u32 centroids_num, const f32 *embedding_data, u64 embedding_num, u32 iter_cnt = 4);

    // Train with sampling strategy similar to next-plaid
    // Returns actual number of embeddings used for training (after sampling)
    u64 TrainWithSampling(u32 centroids_num, const f32 *embedding_data, u64 embedding_num, u32 iter_cnt = 4);

    // Add one document's embeddings
    void AddOneDocEmbeddings(const f32 *embedding_data, u32 embedding_num);

    // Incrementally update index with new embeddings
    // Supports centroid expansion when outliers are detected
    // Returns number of new centroids added (0 if no expansion)
    u32 UpdateWithNewEmbeddings(const f32 *embedding_data, u64 embedding_num, bool allow_centroid_expansion = true);

    // Rebuild index from stored raw embeddings (start_from_scratch mode)
    // Used when index quality degrades or for recovery
    void RebuildFromRawEmbeddings(u32 new_centroids_num = 0, u32 iter_cnt = 4);

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

    // Batch search for multiple queries
    std::vector<PlaidQueryResultType> SearchBatch(const std::vector<const f32 *> &query_ptrs,
                                                  const std::vector<u32> &query_embedding_nums,
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
    u32 GetDocNum() const;
    u32 GetTotalEmbeddingNum() const;
    u32 GetNCentroids() const { return n_centroids_; }
    u32 GetEmbeddingDimension() const { return embedding_dimension_; }
    bool IsMmap() const { return is_mmap_; }

    // Get document length (number of tokens)
    u32 GetDocLen(u32 doc_id) const;

    // Reconstruct embeddings for a document
    // Returns: [doc_len, embedding_dim] flattened array
    std::unique_ptr<f32[]> ReconstructDocument(u32 doc_id, u32 &doc_len) const;

    // Reconstruct multiple documents
    std::vector<std::unique_ptr<f32[]>> ReconstructDocuments(const std::vector<u32> &doc_ids, std::vector<u32> &doc_lens) const;

    // Centroid expansion for incremental updates
    // Expands centroids with new embeddings using partial K-means
    void ExpandCentroids(const f32 *new_embeddings, u64 new_embedding_count, u32 expand_iter = 4);

    // Store raw embeddings for potential rebuild (start_from_scratch mode)
    void StoreRawEmbeddings(const f32 *embedding_data, u64 embedding_num);
    void ClearRawEmbeddings();
    bool HasRawEmbeddings() const { return !raw_embeddings_.empty(); }
    u64 GetRawEmbeddingCount() const { return raw_embeddings_count_; }

    // Search without bitmask (for testing/simple use cases)
    PlaidQueryResultType GetQueryResult(const f32 *query_ptr,
                                        u32 query_embedding_num,
                                        u32 n_ivf_probe,
                                        f32 centroid_score_threshold,
                                        u32 n_doc_to_score,
                                        u32 n_full_scores,
                                        u32 top_k,
                                        f32 threshold_final = 0.0f) const;

private:
    // Fixed parameters (set at construction)
    const u32 start_segment_offset_ = 0;
    const u32 embedding_dimension_ = 0;
    const u32 nbits_ = 4;
    const u32 requested_n_centroids_ = 0; // 0 = auto

    // Trained parameters
    u32 n_centroids_ = 0;
    std::vector<f32> centroids_data_;          // [n_centroids_, embedding_dimension_]
    std::vector<f32> centroid_norms_neg_half_; // [n_centroids_] for fast L2 computation

    // Document metadata
    std::atomic_uint32_t n_docs_ = 0;
    u32 n_total_embeddings_ = 0;
    std::vector<u32> doc_lens_;    // [n_docs_] length of each document
    std::vector<u32> doc_offsets_; // [n_docs_] offset of each doc in centroid_ids_

    // Embedding data
    std::vector<u32> centroid_ids_;          // [n_total_embeddings_] centroid assignment for each embedding
    std::unique_ptr<u8[]> packed_residuals_; // Quantized residuals
    size_t packed_residuals_size_ = 0;

    // Inverted index: centroid -> doc ids
    std::vector<std::vector<u32>> ivf_lists_; // [n_centroids_] posting lists

    // Quantizer
    std::unique_ptr<PlaidQuantizer> quantizer_;

    // Mmap support
    void *mmap_addr_ = nullptr;
    size_t mmap_size_ = 0;
    bool is_mmap_ = false;
    bool owns_data_ = true;

    // Raw embeddings storage for start_from_scratch rebuild mode
    // Stored as flattened array: [n_embeddings, embedding_dimension]
    std::vector<f32> raw_embeddings_;
    u64 raw_embeddings_count_ = 0;

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

    // Compute approximate score using centroid lookups
    f32 ApproximateScore(const u32 *doc_centroid_ids, u32 doc_len, const f32 *query_centroid_scores, u32 n_query_tokens) const;

    // Compute exact score with dequantization
    f32 ExactScore(const f32 *query_ptr, u32 n_query_tokens, u32 doc_id, const f32 *centroid_distances) const;

    // Helper for batch centroid scoring
    std::unique_ptr<f32[]> ComputeQueryCentroidScores(const f32 *query_ptr, u32 n_query_tokens) const;
};

} // namespace infinity
