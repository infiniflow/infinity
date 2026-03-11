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

module;

#include <algorithm>
#include <random>

export module infinity_core:plaid_global_centroids.impl;

import :plaid_global_centroids;
import :plaid_quantizer;
import :mlas_matrix_multiply;
import :vector_distance;
import :kmeans_partition;
import :logger;
import :infinity_exception;
import :local_file_handle;

import std.compat;

namespace infinity {

PlaidGlobalCentroids::PlaidGlobalCentroids(const u32 embedding_dimension, const u32 nbits)
    : embedding_dimension_(embedding_dimension), nbits_(nbits), quantizer_(std::make_unique<PlaidQuantizer>(nbits, embedding_dimension)) {}

PlaidGlobalCentroids::~PlaidGlobalCentroids() = default;

PlaidGlobalCentroids::PlaidGlobalCentroids(PlaidGlobalCentroids &&other) noexcept
    : embedding_dimension_(other.embedding_dimension_), nbits_(other.nbits_), n_centroids_(other.n_centroids_),
      centroids_data_(std::move(other.centroids_data_)), centroid_norms_neg_half_(std::move(other.centroid_norms_neg_half_)),
      quantizer_(std::move(other.quantizer_)) {
    other.n_centroids_ = 0;
}

PlaidGlobalCentroids &PlaidGlobalCentroids::operator=(PlaidGlobalCentroids &&other) noexcept {
    if (this != &other) {
        std::unique_lock lock(rw_mutex_);
        std::unique_lock other_lock(other.rw_mutex_);

        n_centroids_ = other.n_centroids_;
        centroids_data_ = std::move(other.centroids_data_);
        centroid_norms_neg_half_ = std::move(other.centroid_norms_neg_half_);
        quantizer_ = std::move(other.quantizer_);

        other.n_centroids_ = 0;
    }
    return *this;
}

void PlaidGlobalCentroids::Train(const u32 n_centroids, const f32 *embedding_data, const u64 embedding_num, const u32 iter_cnt) {
    std::unique_lock lock(rw_mutex_);

    if (n_centroids == 0 || (n_centroids % 8) != 0) {
        UnrecoverableError(fmt::format("PlaidGlobalCentroids::Train: n_centroids must be a multiple of 8, got {}", n_centroids));
    }

    // Minimum training data requirement
    const u64 min_data = std::max<u64>(32ul * n_centroids, 256ul);
    if (embedding_num < min_data) {
        LOG_WARN(fmt::format("PlaidGlobalCentroids::Train: Not enough training data. Have {}, need at least {}", embedding_num, min_data));
    }

    n_centroids_ = n_centroids;

    // Use sampling strategy similar to next-plaid: max k * 256 samples
    const u64 max_sample_size = static_cast<u64>(n_centroids) * 256;
    const u64 actual_sample_size = std::min(max_sample_size, embedding_num);

    const f32 *training_data = embedding_data;
    std::unique_ptr<f32[]> sampled_data;
    u64 training_num = embedding_num;

    if (actual_sample_size < embedding_num) {
        LOG_INFO(fmt::format("PlaidGlobalCentroids::Train: Sampling {} from {} embeddings", actual_sample_size, embedding_num));

        // Reservoir sampling
        sampled_data = std::make_unique_for_overwrite<f32[]>(actual_sample_size * embedding_dimension_);
        std::random_device rd;
        std::mt19937 gen(rd());

        // Fill with first k elements
        for (u64 i = 0; i < actual_sample_size; ++i) {
            std::copy_n(embedding_data + i * embedding_dimension_, embedding_dimension_, sampled_data.get() + i * embedding_dimension_);
        }

        // Replace with decreasing probability
        std::uniform_int_distribution<u64> dist(0, std::numeric_limits<u64>::max());
        for (u64 i = actual_sample_size; i < embedding_num; ++i) {
            u64 j = dist(gen) % (i + 1);
            if (j < actual_sample_size) {
                std::copy_n(embedding_data + i * embedding_dimension_, embedding_dimension_, sampled_data.get() + j * embedding_dimension_);
            }
        }

        training_data = sampled_data.get();
        training_num = actual_sample_size;
    }

    // Allocate space for centroids
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);

    // Train K-means
    const auto result_centroid_num =
        GetKMeansCentroids(MetricType::kMetricL2, embedding_dimension_, training_num, training_data, centroids_data_, n_centroids_, iter_cnt);

    if (result_centroid_num != n_centroids_) {
        UnrecoverableError(fmt::format("PlaidGlobalCentroids::Train: KMeans failed to get {} centroids, got {}", n_centroids_, result_centroid_num));
    }

    // Compute centroid norms for fast L2 distance
    ComputeCentroidNorms();

    LOG_INFO(fmt::format("PlaidGlobalCentroids::Train: Trained {} centroids from {} samples", n_centroids_, training_num));

    // Sample residuals for quantizer training
    constexpr u64 MAX_SAMPLE_FOR_QUANTIZER = 50000;
    const u64 quantizer_sample_size = std::min(MAX_SAMPLE_FOR_QUANTIZER, embedding_num);

    LOG_INFO(fmt::format("PlaidGlobalCentroids::Train: Sampling {} residuals for quantizer training", quantizer_sample_size));

    // Sample indices for quantizer training
    std::vector<u64> sample_indices;
    sample_indices.reserve(quantizer_sample_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<u64> dist(0, std::numeric_limits<u64>::max());

    for (u64 i = 0; i < quantizer_sample_size; ++i) {
        sample_indices.push_back(i);
    }
    for (u64 i = quantizer_sample_size; i < embedding_num; ++i) {
        u64 j = dist(gen) % (i + 1);
        if (j < quantizer_sample_size) {
            sample_indices[j] = i;
        }
    }

    // Compute residuals for sampled embeddings
    const auto sample_residuals = std::make_unique_for_overwrite<f32[]>(quantizer_sample_size * embedding_dimension_);
    {
        constexpr u32 CHUNK_SIZE = 65536;
        const auto dist_table_chunk = std::make_unique_for_overwrite<f32[]>(CHUNK_SIZE * n_centroids_);

        for (u64 chunk_start = 0; chunk_start < quantizer_sample_size; chunk_start += CHUNK_SIZE) {
            const u32 chunk_end = std::min(static_cast<u32>(chunk_start + CHUNK_SIZE), static_cast<u32>(quantizer_sample_size));
            const u32 chunk_count = chunk_end - static_cast<u32>(chunk_start);

            // Prepare chunk embeddings
            const auto chunk_embeddings = std::make_unique_for_overwrite<f32[]>(chunk_count * embedding_dimension_);
            for (u32 i = 0; i < chunk_count; ++i) {
                const u64 orig_idx = sample_indices[chunk_start + i];
                std::copy_n(embedding_data + orig_idx * embedding_dimension_,
                            embedding_dimension_,
                            chunk_embeddings.get() + i * embedding_dimension_);
            }

            // Compute distances to centroids
            matrixA_multiply_transpose_matrixB_output_to_C(chunk_embeddings.get(),
                                                           centroids_data_.data(),
                                                           chunk_count,
                                                           n_centroids_,
                                                           embedding_dimension_,
                                                           dist_table_chunk.get());

            // Find nearest centroid and compute residual
            for (u32 i = 0; i < chunk_count; ++i) {
                const f32 *emb = chunk_embeddings.get() + i * embedding_dimension_;
                f32 *residual = sample_residuals.get() + (chunk_start + i) * embedding_dimension_;

                f32 max_neg_dist = std::numeric_limits<f32>::lowest();
                u32 nearest_id = 0;

                for (u32 k = 0; k < n_centroids_; ++k) {
                    const f32 neg_dist = dist_table_chunk[i * n_centroids_ + k] + centroid_norms_neg_half_[k];
                    if (neg_dist > max_neg_dist) {
                        max_neg_dist = neg_dist;
                        nearest_id = k;
                    }
                }

                const f32 *centroid = centroids_data_.data() + nearest_id * embedding_dimension_;
                for (u32 j = 0; j < embedding_dimension_; ++j) {
                    residual[j] = emb[j] - centroid[j];
                }
            }
        }
    }

    // Train quantizer on sampled residuals
    quantizer_->Train(sample_residuals.get(), quantizer_sample_size);
    LOG_INFO(fmt::format("PlaidGlobalCentroids::Train: Quantizer trained on {} residuals", quantizer_sample_size));
}

void PlaidGlobalCentroids::ComputeCentroidNorms() {
    const f32 *centroid_data = centroids_data_.data();
    for (u32 i = 0; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, embedding_dimension_);
        centroid_data += embedding_dimension_;
    }
}

u32 PlaidGlobalCentroids::FindNearestCentroid(const f32 *embedding) const {
    std::shared_lock lock(rw_mutex_);

    if (n_centroids_ == 0) {
        UnrecoverableError("PlaidGlobalCentroids::FindNearestCentroid: Centroids not trained");
    }

    // Compute distances to all centroids
    f32 max_neg_distance = std::numeric_limits<f32>::lowest();
    u32 nearest_id = 0;

    for (u32 i = 0; i < n_centroids_; ++i) {
        const f32 *centroid = centroids_data_.data() + i * embedding_dimension_;
        const f32 dot = IPDistance<f32>(embedding, centroid, embedding_dimension_);
        const f32 neg_distance = dot + centroid_norms_neg_half_[i];

        if (neg_distance > max_neg_distance) {
            max_neg_distance = neg_distance;
            nearest_id = i;
        }
    }

    return nearest_id;
}

void PlaidGlobalCentroids::FindNearestCentroids(const f32 *embeddings, const u64 n_embeddings, u32 *centroid_ids) const {
    std::shared_lock lock(rw_mutex_);

    if (n_centroids_ == 0) {
        UnrecoverableError("PlaidGlobalCentroids::FindNearestCentroids: Centroids not trained");
    }

    // Process in chunks to limit memory usage
    constexpr u32 CHUNK_SIZE = 16384;
    const auto dist_table = std::make_unique_for_overwrite<f32[]>(CHUNK_SIZE * n_centroids_);

    for (u64 chunk_start = 0; chunk_start < n_embeddings; chunk_start += CHUNK_SIZE) {
        const u32 chunk_end = std::min(static_cast<u32>(chunk_start + CHUNK_SIZE), static_cast<u32>(n_embeddings));
        const u32 chunk_count = chunk_end - static_cast<u32>(chunk_start);

        // Compute distances for this chunk
        matrixA_multiply_transpose_matrixB_output_to_C(embeddings + chunk_start * embedding_dimension_,
                                                       centroids_data_.data(),
                                                       chunk_count,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());

        // Find nearest centroid for each embedding
        for (u32 i = 0; i < chunk_count; ++i) {
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u32 nearest_id = 0;

            for (u32 k = 0; k < n_centroids_; ++k) {
                const f32 neg_distance = dist_table[i * n_centroids_ + k] + centroid_norms_neg_half_[k];
                if (neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    nearest_id = k;
                }
            }

            centroid_ids[chunk_start + i] = nearest_id;
        }
    }
}

std::unique_ptr<f32[]> PlaidGlobalCentroids::ComputeCentroidScores(const f32 *query_ptr, const u32 n_query_tokens) const {
    std::shared_lock lock(rw_mutex_);

    auto scores = std::make_unique<f32[]>(n_query_tokens * n_centroids_);

    matrixA_multiply_transpose_matrixB_output_to_C(query_ptr,
                                                   centroids_data_.data(),
                                                   n_query_tokens,
                                                   n_centroids_,
                                                   embedding_dimension_,
                                                   scores.get());

    // Add centroid_norms_neg_half for L2 distance computation
    for (u32 i = 0; i < n_query_tokens; ++i) {
        for (u32 j = 0; j < n_centroids_; ++j) {
            scores[i * n_centroids_ + j] += centroid_norms_neg_half_[j];
        }
    }

    return scores;
}

void PlaidGlobalCentroids::ComputeResiduals(const f32 *embeddings, const u64 n_embeddings, const u32 *centroid_ids, f32 *residuals) const {
    std::shared_lock lock(rw_mutex_);

    for (u64 i = 0; i < n_embeddings; ++i) {
        const f32 *emb = embeddings + i * embedding_dimension_;
        const f32 *centroid = centroids_data_.data() + centroid_ids[i] * embedding_dimension_;
        f32 *residual = residuals + i * embedding_dimension_;

        for (u32 j = 0; j < embedding_dimension_; ++j) {
            residual[j] = emb[j] - centroid[j];
        }
    }
}

void PlaidGlobalCentroids::Save(LocalFileHandle &file_handle) const {
    std::shared_lock lock(rw_mutex_);

    // Header
    file_handle.Append(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Append(&nbits_, sizeof(nbits_));
    file_handle.Append(&n_centroids_, sizeof(n_centroids_));

    // Centroids data
    file_handle.Append(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    file_handle.Append(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Quantizer
    quantizer_->Save(file_handle);
}

void PlaidGlobalCentroids::Load(LocalFileHandle &file_handle) {
    std::unique_lock lock(rw_mutex_);

    // Verify header
    u32 stored_embedding_dimension, stored_nbits;
    file_handle.Read(&stored_embedding_dimension, sizeof(stored_embedding_dimension));
    file_handle.Read(&stored_nbits, sizeof(stored_nbits));

    if (stored_embedding_dimension != embedding_dimension_ || stored_nbits != nbits_) {
        UnrecoverableError("PlaidGlobalCentroids::Load: header mismatch");
    }

    file_handle.Read(&n_centroids_, sizeof(n_centroids_));

    // Read centroids data
    centroids_data_.resize(n_centroids_ * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);
    file_handle.Read(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    file_handle.Read(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Read quantizer
    quantizer_->Load(file_handle);
}

size_t PlaidGlobalCentroids::MemUsage() const {
    std::shared_lock lock(rw_mutex_);

    size_t size = sizeof(PlaidGlobalCentroids);
    size += centroids_data_.size() * sizeof(f32);
    size += centroid_norms_neg_half_.size() * sizeof(f32);
    size += quantizer_->MemUsage();

    return size;
}

} // namespace infinity
