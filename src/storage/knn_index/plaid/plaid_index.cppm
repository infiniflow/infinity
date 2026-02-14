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

import std;
import column_def;
import internal_types;

namespace infinity {

class LocalFileHandle;
struct BlockIndex;
class PlaidQuantizer;
class SegmentMeta;

using PlaidQueryResultType = std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>;

// Global IVF shared across all chunks in a segment
// Loaded entirely into memory (~10-100MB)
export class PlaidGlobalIVF {
public:
    PlaidGlobalIVF() = default;

    // Initialize with trained centroids
    void Initialize(u32 n_centroids, u32 embedding_dim, const f32 *centroids_data);

    // Add posting list entries for new documents
    void AddToPostingLists(u32 doc_id_start, const std::vector<u32> &centroid_ids, const std::vector<u32> &doc_lens);

    // Update posting lists during chunk rewrite
    // chunk_id: the chunk being rewritten
    // start_doc_id: starting doc_id of the chunk
    // doc_lens: document lengths in the chunk
    // centroid_ids: centroid assignments for all embeddings in the chunk
    void UpdatePostingListsForChunk(u32 chunk_id, u32 start_doc_id, const std::vector<u32> &doc_lens, const std::vector<u32> &centroid_ids);

    // Compute query-centroid scores
    std::unique_ptr<f32[]> ComputeQueryScores(const f32 *query_ptr, u32 query_len) const;

    // Get candidates for given centroids
    std::vector<u32> GetCandidates(const std::vector<u32> &centroid_ids) const;

    // Save/Load
    void Save(const std::string &file_path) const;
    void Load(const std::string &file_path);

    // Accessors
    u32 GetNCentroids() const { return n_centroids_; }
    const std::vector<f32> &GetCentroids() const { return centroids_data_; }
    const std::vector<std::vector<u32>> &GetIVFLists() const { return ivf_lists_; }

    // Expand centroids (centroid expansion mode)
    u32 ExpandCentroids(const f32 *new_embeddings, u64 new_embedding_count, u32 embedding_dim, u32 expand_iter = 4);

    // Rebuild all centroids from raw embeddings (start-from-scratch mode)
    void RebuildCentroids(const f32 *all_embeddings, u64 total_embeddings, u32 embedding_dim, u32 target_n_centroids);

private:
    u32 n_centroids_ = 0;
    u32 embedding_dimension_ = 0;
    std::vector<f32> centroids_data_;          // [n_centroids, embedding_dim]
    std::vector<f32> centroid_norms_neg_half_; // [n_centroids] for fast L2
    std::vector<std::vector<u32>> ivf_lists_;  // [n_centroids] posting lists
    mutable std::shared_mutex rw_mutex_;
};

// PLAID Index Chunk - stores encoded data for a subset of documents
// Chunks can be rewritten (unlike column store chunks)
export class PlaidIndexChunk {
public:
    PlaidIndexChunk(u32 chunk_id, u32 start_doc_id);

    // Load from file
    void Load(const std::string &chunk_dir);

    // Save to file
    void Save(const std::string &chunk_dir) const;

    // Append data to this chunk (for incremental updates)
    // Returns false if chunk is full
    bool AppendData(const std::vector<u32> &centroid_ids, const std::vector<u8> &packed_residuals, const std::vector<u32> &doc_lens);

    // Rewrite chunk with new data (for centroid expansion)
    void
    Rewrite(const std::vector<u32> &centroid_ids, const std::vector<u8> &packed_residuals, const std::vector<u32> &doc_lens, u32 embedding_count);

    // Get document count
    u32 GetDocCount() const { return doc_lens_.size(); }

    // Get embedding count
    u32 GetEmbeddingCount() const { return centroid_ids_.size(); }

    // Check if chunk can accept more data
    bool CanAppend(u32 additional_docs) const;

    // Accessors
    u32 GetChunkID() const { return chunk_id_; }
    u32 GetStartDocID() const { return start_doc_id_; }
    u32 GetEndDocID() const { return start_doc_id_ + doc_lens_.size(); }

    // Get data for search
    const std::vector<u32> &GetCentroidIDs() const { return centroid_ids_; }
    const std::vector<u8> &GetPackedResiduals() const { return packed_residuals_; }
    const std::vector<u32> &GetDocLens() const { return doc_lens_; }
    const std::vector<u32> &GetDocOffsets() const { return doc_offsets_; }

private:
    u32 chunk_id_ = 0;
    u32 start_doc_id_ = 0; // Global doc ID offset
    std::vector<u32> centroid_ids_;
    std::vector<u8> packed_residuals_;
    std::vector<u32> doc_lens_;
    std::vector<u32> doc_offsets_;
    mutable std::shared_mutex rw_mutex_;
};

// Segment-level PLAID index manager
// Manages multiple chunks and shared global IVF
export class PlaidSegmentIndex {
public:
    PlaidSegmentIndex(SegmentID segment_id, const std::string &index_dir);

    // Load existing segment index
    void Load();

    // Save segment index metadata
    void Save() const;

    // Append data to segment (handles chunk management)
    // Returns the chunk ID where data was written
    Status AppendData(const std::vector<u32> &centroid_ids,
                      const std::vector<u8> &packed_residuals,
                      const std::vector<u32> &doc_lens,
                      u32 embedding_count,
                      ChunkID &out_chunk_id);

    // Append to last chunk (buffer mode)
    Status AppendToLastChunk(const std::vector<u32> &centroid_ids,
                             const std::vector<u8> &packed_residuals,
                             const std::vector<u32> &doc_lens,
                             ChunkID &out_chunk_id);

    // Create new chunk with data
    ChunkID CreateNewChunk(const std::vector<u32> &centroid_ids, const std::vector<u8> &packed_residuals, const std::vector<u32> &doc_lens);

    // Rewrite last chunk (for centroid expansion)
    Status RewriteLastChunk(const std::vector<u32> &new_centroid_ids,
                            const std::vector<u8> &new_packed_residuals,
                            const std::vector<u32> &new_doc_lens,
                            u32 new_embedding_count);

    // Get global IVF for centroid management
    PlaidGlobalIVF *GetGlobalIVF() { return &global_ivf_; }
    const PlaidGlobalIVF *GetGlobalIVF() const { return &global_ivf_; }

    // Get total document count
    u32 GetTotalDocCount() const;

    // Get total embedding count
    u32 GetTotalEmbeddingCount() const;

    // Get chunk count
    u32 GetChunkCount() const { return chunks_.size(); }

    // Get chunk info for metadata
    std::vector<std::pair<ChunkID, u32>> GetChunkInfos() const;

    // Access chunks
    const std::vector<std::unique_ptr<PlaidIndexChunk>> &GetChunks() const { return chunks_; }

    // Get chunk directory path
    std::string GetChunkDir(ChunkID chunk_id) const;

    // Search interface
    PlaidQueryResultType Search(const f32 *query_ptr,
                                u32 query_embedding_num,
                                u32 top_n,
                                Bitmask &bitmask,
                                const BlockIndex *block_index,
                                TxnTimeStamp begin_ts,
                                u32 n_ivf_probe,
                                f32 centroid_score_threshold,
                                u32 n_doc_to_score,
                                u32 n_full_scores) const;

private:
    SegmentID segment_id_;
    std::string index_dir_;
    std::string ivf_file_path_;
    std::string meta_file_path_;

    PlaidGlobalIVF global_ivf_;
    std::vector<std::unique_ptr<PlaidIndexChunk>> chunks_;
    u32 next_doc_id_ = 0; // Global document ID counter

    mutable std::shared_mutex rw_mutex_;

    static constexpr u32 DEFAULT_CHUNK_SIZE = 50000; // ~50k docs per chunk
};

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

    // Internal version of ExpandCentroids - assumes caller holds the lock
    void ExpandCentroidsInternal(const f32 *new_embeddings, u64 new_embedding_count, u32 expand_iter);

    // Store raw embeddings for potential rebuild (start_from_scratch mode)
    void StoreRawEmbeddings(const f32 *embedding_data, u64 embedding_num);
    void ClearRawEmbeddings();
    bool HasRawEmbeddings() const { return !raw_embeddings_.empty(); }
    u64 GetRawEmbeddingCount() const { return raw_embeddings_count_; }

    // Get internal data for dump operations (used by PlaidIndexInMem::PrepareDumpData)
    // Returns pointers to internal data - caller must not modify
    const std::vector<u32> &GetCentroidIds() const { return centroid_ids_; }
    const u8 *GetPackedResiduals() const { return packed_residuals_.get(); }
    size_t GetPackedResidualsSize() const { return packed_residuals_size_; }
    const std::vector<u32> &GetDocLens() const { return doc_lens_; }
    const std::vector<u32> &GetDocOffsets() const { return doc_offsets_; }
    const PlaidQuantizer *GetQuantizer() const { return quantizer_.get(); }

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
