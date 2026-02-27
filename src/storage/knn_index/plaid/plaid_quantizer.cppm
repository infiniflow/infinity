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

export module infinity_core:plaid_quantizer;

import :infinity_type;

import std;

namespace infinity {

class LocalFileHandle;

// Residual Quantizer for PLAID (based on next-plaid's implementation)
// Uses nbits quantization (2 or 4 bits) for residuals after centroid assignment
export class PlaidQuantizer {
public:
    PlaidQuantizer(u32 nbits, u32 embedding_dim);
    ~PlaidQuantizer() = default;

    // Train the quantizer: compute bucket cutoffs and weights
    void Train(const f32 *residuals, u64 n_embeddings);

    // Quantize residuals into packed bytes
    // Returns: packed data of shape [n_embeddings, packed_dim] where packed_dim = embedding_dim * nbits / 8
    std::unique_ptr<u8[]> Quantize(const f32 *residuals, u64 n_embeddings, u32 &packed_dim) const;

    // Dequantize packed data back to approximate residuals
    // centroid_ids: [n_embeddings] - which centroid each embedding belongs to
    // packed_residuals: [n_embeddings, packed_dim] - quantized residuals
    // output: [n_embeddings, embedding_dim] - reconstructed embeddings (centroid + dequantized residual)
    void
    Dequantize(const u32 *centroid_ids, const u8 *packed_residuals, u64 n_embeddings, f32 *output, const f32 *centroids_data, u32 n_centroids) const;

    // Get inner product distance table for a query
    // query: [embedding_dim]
    // Returns: ip_table [embedding_dim, n_buckets] for fast lookup
    std::unique_ptr<f32[]> GetIPDistanceTable(const f32 *query) const;

    // Get single inner product distance using precomputed table
    // embedding_id: index of the embedding
    // query_id: index of the query (for multi-query)
    // query_num: total number of queries
    // ip_table: precomputed distance table
    // packed_residuals: quantized residual data
    f32 GetSingleIPDistance(u32 embedding_id,
                            u32 query_id,
                            u32 query_num,
                            const f32 *ip_table,
                            const u8 *packed_residuals,
                            const u32 *centroid_ids,
                            const f32 *centroids_data) const;

    // Get multiple inner product distances
    void GetMultipleIPDistance(u32 embedding_offset,
                               u32 n_embeddings,
                               u32 query_id,
                               u32 query_num,
                               const f32 *ip_table,
                               const u8 *packed_residuals,
                               const u32 *centroid_ids,
                               const f32 *centroids_data,
                               f32 *output) const;

    // Save/Load
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);

    // Mmap support
    void SaveToPtr(void *ptr, size_t &offset) const;
    void LoadFromPtr(void *ptr, size_t &offset);

    // Accessors
    u32 nbits() const { return nbits_; }
    u32 embedding_dim() const { return embedding_dim_; }
    u32 n_buckets() const { return 1u << nbits_; }
    u32 packed_dim() const { return (embedding_dim_ * nbits_ + 7) / 8; }

    const f32 *bucket_weights() const { return bucket_weights_.get(); }
    const f32 *bucket_cutoffs() const { return bucket_cutoffs_.get(); }

private:
    const u32 nbits_;
    const u32 embedding_dim_;
    const u32 n_buckets_;
    const u32 packed_dim_;

    // Quantization parameters
    std::unique_ptr<f32[]> bucket_cutoffs_; // [n_buckets - 1] boundaries
    std::unique_ptr<f32[]> bucket_weights_; // [n_buckets] reconstruction values
    f32 avg_residual_ = 0.0f;               // Average residual (for centering)

    // Lookup tables for fast decompression
    std::unique_ptr<u8[]> byte_reversed_bits_map_;       // [256] bit reversal map
    std::unique_ptr<u8[]> bucket_weight_indices_lookup_; // [256, keys_per_byte] lookup table

    mutable std::shared_mutex rw_mutex_;

    // Helper functions
    u8 ReverseBits(u8 byte) const;
    void BuildLookupTables();
};

} // namespace infinity
