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

export module infinity_core:plaid_global_centroids;

import :infinity_type;
import :plaid_quantizer;

import std;

namespace infinity {

class LocalFileHandle;

// Global centroids shared across all chunks in a segment
// Key design: centroids are trained once and never changed (unless explicit rebuild)
// This allows:
// 1. Fast incremental updates (no re-training)
// 2. Efficient merge (same centroids = direct data merge)
// 3. Low memory footprint (chunks share centroids)
export class PlaidGlobalCentroids {
public:
    PlaidGlobalCentroids(u32 embedding_dimension, u32 nbits);
    ~PlaidGlobalCentroids();

    // Disable copy
    PlaidGlobalCentroids(const PlaidGlobalCentroids &) = delete;
    PlaidGlobalCentroids &operator=(const PlaidGlobalCentroids &) = delete;

    // Enable move
    PlaidGlobalCentroids(PlaidGlobalCentroids &&other) noexcept;
    PlaidGlobalCentroids &operator=(PlaidGlobalCentroids &&other) noexcept;

    // Train centroids from embedding data
    // Uses K-means with sampling (similar to next-plaid strategy)
    void Train(u32 n_centroids, const f32 *embedding_data, u64 embedding_num, u32 iter_cnt = 4);

    // Check if centroids are trained
    bool IsTrained() const { return n_centroids_ > 0; }

    // Find nearest centroid for a single embedding
    u32 FindNearestCentroid(const f32 *embedding) const;

    // Find nearest centroids for multiple embeddings (batch version)
    void FindNearestCentroids(const f32 *embeddings, u64 n_embeddings, u32 *centroid_ids) const;

    // Compute centroid scores for query tokens
    // Output: [n_query_tokens * n_centroids]
    std::unique_ptr<f32[]> ComputeCentroidScores(const f32 *query_ptr, u32 n_query_tokens) const;

    // Compute residuals for embeddings (embedding - centroid)
    void ComputeResiduals(const f32 *embeddings, u64 n_embeddings, const u32 *centroid_ids, f32 *residuals) const;

    // Get quantizer for residual quantization
    PlaidQuantizer *quantizer() const { return quantizer_.get(); }

    // Accessors
    u32 n_centroids() const { return n_centroids_; }
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 nbits() const { return nbits_; }
    const std::vector<f32> &centroids_data() const { return centroids_data_; }
    const std::vector<f32> &centroid_norms_neg_half() const { return centroid_norms_neg_half_; }

    // Persistence
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);

    // Size calculation
    size_t MemUsage() const;

private:
    // Fixed parameters
    const u32 embedding_dimension_;
    const u32 nbits_;

    // Trained parameters
    u32 n_centroids_ = 0;
    std::vector<f32> centroids_data_;          // [n_centroids * embedding_dimension]
    std::vector<f32> centroid_norms_neg_half_; // [n_centroids] for fast L2 computation

    // Residual quantizer
    std::unique_ptr<PlaidQuantizer> quantizer_;

    // Thread safety
    mutable std::shared_mutex rw_mutex_;

    // Private helper
    void ComputeCentroidNorms();
};

} // namespace infinity
