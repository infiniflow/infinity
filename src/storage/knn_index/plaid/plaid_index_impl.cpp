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

module infinity_core:plaid_index.impl;

import :plaid_index;
import :plaid_quantizer;
import :mlas_matrix_multiply;
import :vector_distance;
import :kmeans_partition;
import :emvb_result_handler;
import :emvb_simd_funcs;
import :index_base;
import :status;
import :logger;
import :infinity_exception;
import :column_vector;
import :roaring_bitmap;

import :default_values;
import :block_index;
import :new_catalog;
import :segment_meta;
import :block_meta;
import :column_meta;
import :kv_store;
import :local_file_handle;
import :virtual_store;

import std.compat;
import third_party;

import column_def;
import internal_types;

namespace infinity {

PlaidIndex::PlaidIndex(const u32 start_segment_offset, const u32 embedding_dimension, const u32 nbits, const u32 n_centroids)
    : start_segment_offset_(start_segment_offset), embedding_dimension_(embedding_dimension), nbits_(nbits), requested_n_centroids_(n_centroids),
      quantizer_(std::make_unique<PlaidQuantizer>(nbits, embedding_dimension)) {}

PlaidIndex::PlaidIndex(const u32 start_segment_offset,
                       const u32 embedding_dimension,
                       const u32 nbits,
                       const u32 n_centroids,
                       void *mmap_addr,
                       const size_t mmap_size)
    : start_segment_offset_(start_segment_offset), embedding_dimension_(embedding_dimension), nbits_(nbits), requested_n_centroids_(n_centroids),
      quantizer_(std::make_unique<PlaidQuantizer>(nbits, embedding_dimension)), mmap_addr_(mmap_addr), mmap_size_(mmap_size), is_mmap_(true),
      owns_data_(false) {}

PlaidIndex::~PlaidIndex() = default;

PlaidIndex::PlaidIndex(PlaidIndex &&other)
    : start_segment_offset_(other.start_segment_offset_), embedding_dimension_(other.embedding_dimension_), nbits_(other.nbits_),
      requested_n_centroids_(other.requested_n_centroids_), n_centroids_(other.n_centroids_), centroids_data_(std::move(other.centroids_data_)),
      centroid_norms_neg_half_(std::move(other.centroid_norms_neg_half_)), n_docs_(other.n_docs_.load()),
      n_total_embeddings_(other.n_total_embeddings_), doc_lens_(std::move(other.doc_lens_)), doc_offsets_(std::move(other.doc_offsets_)),
      centroid_ids_(std::move(other.centroid_ids_)), packed_residuals_(std::move(other.packed_residuals_)),
      packed_residuals_size_(other.packed_residuals_size_), ivf_lists_(std::move(other.ivf_lists_)), quantizer_(std::move(other.quantizer_)),
      mmap_addr_(other.mmap_addr_), mmap_size_(other.mmap_size_), is_mmap_(other.is_mmap_), owns_data_(other.owns_data_) {
    other.mmap_addr_ = nullptr;
    other.mmap_size_ = 0;
    other.n_docs_ = 0;
    other.n_total_embeddings_ = 0;
    other.packed_residuals_size_ = 0;
}

PlaidIndex &PlaidIndex::operator=(PlaidIndex &&other) {
    if (this != &other) {
        std::unique_lock lock(rw_mutex_);
        std::unique_lock other_lock(other.rw_mutex_);

        n_centroids_ = other.n_centroids_;
        centroids_data_ = std::move(other.centroids_data_);
        centroid_norms_neg_half_ = std::move(other.centroid_norms_neg_half_);
        n_docs_ = other.n_docs_.load();
        n_total_embeddings_ = other.n_total_embeddings_;
        doc_lens_ = std::move(other.doc_lens_);
        doc_offsets_ = std::move(other.doc_offsets_);
        centroid_ids_ = std::move(other.centroid_ids_);
        packed_residuals_ = std::move(other.packed_residuals_);
        packed_residuals_size_ = other.packed_residuals_size_;
        ivf_lists_ = std::move(other.ivf_lists_);
        quantizer_ = std::move(other.quantizer_);
        mmap_addr_ = other.mmap_addr_;
        mmap_size_ = other.mmap_size_;
        is_mmap_ = other.is_mmap_;
        owns_data_ = other.owns_data_;

        other.mmap_addr_ = nullptr;
        other.mmap_size_ = 0;
        other.n_docs_ = 0;
        other.n_total_embeddings_ = 0;
        other.packed_residuals_size_ = 0;
    }
    return *this;
}

u32 PlaidIndex::GetDocNum() const {
    std::shared_lock lock(rw_mutex_);
    return n_docs_.load();
}

u32 PlaidIndex::GetTotalEmbeddingNum() const {
    std::shared_lock lock(rw_mutex_);
    return n_total_embeddings_;
}

u64 PlaidIndex::ExpectLeastTrainingDataNum() const { return std::max<u64>(32ul * n_centroids_, 32ul * quantizer_->n_buckets()); }

u64 PlaidIndex::TrainWithSampling(const u32 centroids_num, const f32 *embedding_data, const u64 embedding_num, const u32 iter_cnt) {
    std::unique_lock lock(rw_mutex_);

    n_centroids_ = centroids_num;
    if (n_centroids_ == 0 || (n_centroids_ % 8) != 0) {
        const auto error_msg = fmt::format("PlaidIndex::TrainWithSampling: n_centroids_ must be a multiple of 8, got {} instead.", n_centroids_);
        UnrecoverableError(error_msg);
    }

    if (const auto least_num = ExpectLeastTrainingDataNum(); embedding_num < least_num) {
        const auto error_msg =
            fmt::format("PlaidIndex::TrainWithSampling: embedding_num must be at least {}, got {} instead.", least_num, embedding_num);
        UnrecoverableError(error_msg);
    }

    // Smart sampling similar to next-plaid
    // Sample size formula: min(16 * sqrt(120 * N), N)
    // This balances training quality and speed for large datasets
    const u64 max_sample_size = static_cast<u64>(16.0 * std::sqrt(120.0 * static_cast<f64>(embedding_num)));
    const u64 actual_sample_size = std::min(max_sample_size, embedding_num);

    const f32 *training_data = embedding_data;
    std::unique_ptr<f32[]> sampled_data;
    u64 training_num = embedding_num;

    if (actual_sample_size < embedding_num) {
        LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Using sampling. Total embeddings: {}, Sample size: {} ({}%)",
                             embedding_num,
                             actual_sample_size,
                             static_cast<int>(100.0 * actual_sample_size / embedding_num)));

        // Random sampling
        sampled_data = std::make_unique_for_overwrite<f32[]>(actual_sample_size * embedding_dimension_);
        std::vector<u32> indices(embedding_num);
        std::iota(indices.begin(), indices.end(), 0u);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen);

        for (u64 i = 0; i < actual_sample_size; ++i) {
            std::copy_n(embedding_data + indices[i] * embedding_dimension_, embedding_dimension_, sampled_data.get() + i * embedding_dimension_);
        }

        training_data = sampled_data.get();
        training_num = actual_sample_size;
    } else {
        LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Using all {} embeddings for training", embedding_num));
    }

    // Allocate space for centroids
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);
    ivf_lists_.resize(n_centroids_);

    const auto time_0 = std::chrono::high_resolution_clock::now();

    // Train centroids using K-means on sampled data
    const auto result_centroid_num =
        GetKMeansCentroids(MetricType::kMetricL2, embedding_dimension_, training_num, training_data, centroids_data_, n_centroids_, iter_cnt);

    if (result_centroid_num != n_centroids_) {
        const auto error_msg =
            fmt::format("PlaidIndex::TrainWithSampling: KMeans failed to get {} centroids, got {} instead.", n_centroids_, result_centroid_num);
        UnrecoverableError(error_msg);
    }

    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: KMeans got {} centroids using {} samples.", result_centroid_num, training_num));

    // Compute centroid norms
    const f32 *centroid_data = centroids_data_.data();
    for (u32 i = 0; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, embedding_dimension_);
        centroid_data += embedding_dimension_;
    }

    const auto time_1 = std::chrono::high_resolution_clock::now();
    auto train_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_1 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Finish train centroids, time cost: {} ms", train_ms));

    // Compute residuals on FULL dataset (not just samples) for quantizer training
    const auto residuals = std::make_unique_for_overwrite<f32[]>(embedding_num * embedding_dimension_);
    {
        const auto dist_table = std::make_unique_for_overwrite<f32[]>(embedding_num * n_centroids_);
        matrixA_multiply_transpose_matrixB_output_to_C(embedding_data,
                                                       centroids_data_.data(),
                                                       embedding_num,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());

        for (u64 i = 0; i < embedding_num; ++i) {
            const f32 *embedding_data_ptr = embedding_data + i * embedding_dimension_;
            f32 *output_ptr = residuals.get() + i * embedding_dimension_;
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u64 max_id = 0;
            const f32 *dist_ptr = dist_table.get() + i * n_centroids_;

            for (u32 k = 0; k < n_centroids_; ++k) {
                if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half_[k]; neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    max_id = k;
                }
            }

            const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
            for (u32 j = 0; j < embedding_dimension_; ++j) {
                output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
            }
        }
    }

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto residual_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Finish calculate residuals on {} embeddings, time cost: {} ms", embedding_num, residual_ms));

    // Train residual quantizer
    quantizer_->Train(residuals.get(), embedding_num);

    const auto time_3 = std::chrono::high_resolution_clock::now();
    auto quantizer_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_3 - time_2).count();
    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Finish train residual quantizer, time cost: {} ms", quantizer_ms));

    return training_num;
}

void PlaidIndex::Train(const u32 centroids_num, const f32 *embedding_data, const u64 embedding_num, const u32 iter_cnt) {
    // Use sampling by default for better performance with large datasets
    TrainWithSampling(centroids_num, embedding_data, embedding_num, iter_cnt);
}

void PlaidIndex::AddOneDocEmbeddings(const f32 *embedding_data, const u32 embedding_num) {
    if (embedding_num == 0) {
        UnrecoverableError("PlaidIndex::AddOneDocEmbeddings: embedding_num must be greater than 0.");
    }

    std::unique_lock lock(rw_mutex_);

    const u32 old_doc_num = n_docs_;
    const u32 old_total_embeddings = n_total_embeddings_;

    doc_lens_.push_back(embedding_num);
    doc_offsets_.push_back(old_total_embeddings);

    // Assign to centroids and compute residuals
    std::vector<u32> centroid_id_assignments(embedding_num);
    const auto residuals = std::make_unique_for_overwrite<f32[]>(embedding_num * embedding_dimension_);

    {
        const auto dist_table = std::make_unique_for_overwrite<f32[]>(embedding_num * n_centroids_);
        matrixA_multiply_transpose_matrixB_output_to_C(embedding_data,
                                                       centroids_data_.data(),
                                                       embedding_num,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());

        for (u32 i = 0; i < embedding_num; ++i) {
            const f32 *embedding_data_ptr = embedding_data + i * embedding_dimension_;
            f32 *output_ptr = residuals.get() + i * embedding_dimension_;
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u32 max_id = 0;
            const f32 *dist_ptr = dist_table.get() + i * n_centroids_;

            for (u32 k = 0; k < n_centroids_; ++k) {
                if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half_[k]; neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    max_id = k;
                }
            }

            centroid_id_assignments[i] = max_id;
            const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
            for (u32 j = 0; j < embedding_dimension_; ++j) {
                output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
            }
        }
    }

    // Store centroid assignments
    centroid_ids_.insert(centroid_ids_.end(), centroid_id_assignments.begin(), centroid_id_assignments.end());

    // Update IVF lists
    for (u32 i = 0; i < embedding_num; ++i) {
        const u32 centroid_id = centroid_id_assignments[i];
        if (ivf_lists_[centroid_id].empty() || ivf_lists_[centroid_id].back() != old_doc_num) {
            ivf_lists_[centroid_id].push_back(old_doc_num);
        }
    }

    // Quantize and store residuals
    u32 packed_dim = 0;
    auto packed_residuals = quantizer_->Quantize(residuals.get(), embedding_num, packed_dim);

    if (!packed_residuals_) {
        packed_residuals_size_ = 0;
    }

    const size_t old_size = packed_residuals_size_;
    packed_residuals_size_ += embedding_num * packed_dim;
    auto new_packed = std::make_unique<u8[]>(packed_residuals_size_);
    if (old_size > 0) {
        std::copy_n(packed_residuals_.get(), old_size, new_packed.get());
    }
    std::copy_n(packed_residuals.get(), embedding_num * packed_dim, new_packed.get() + old_size);
    packed_residuals_ = std::move(new_packed);

    n_total_embeddings_ += embedding_num;
    ++n_docs_;
}

PlaidQueryResultType PlaidIndex::SearchWithBitmask(const f32 *query_ptr,
                                                   const u32 query_embedding_num,
                                                   const u32 top_n,
                                                   Bitmask &bitmask,
                                                   const BlockIndex *block_index,
                                                   const TxnTimeStamp begin_ts,
                                                   const u32 n_ivf_probe,
                                                   const f32 centroid_score_threshold,
                                                   const u32 n_doc_to_score,
                                                   const u32 n_full_scores) const {
    return GetQueryResultWithBitmask(query_ptr,
                                     query_embedding_num,
                                     n_ivf_probe,
                                     centroid_score_threshold,
                                     n_doc_to_score,
                                     n_full_scores,
                                     top_n,
                                     0.0f,
                                     bitmask,
                                     start_segment_offset_,
                                     block_index,
                                     begin_ts);
}

PlaidQueryResultType PlaidIndex::GetQueryResult(const f32 *query_ptr,
                                                const u32 query_embedding_num,
                                                const u32 n_ivf_probe,
                                                const f32 centroid_score_threshold,
                                                const u32 n_doc_to_score,
                                                const u32 n_full_scores,
                                                const u32 top_k,
                                                const f32 threshold_final) const {
    std::shared_lock lock(rw_mutex_);

    // Compute query-centroid scores
    auto query_centroid_scores = ComputeQueryCentroidScores(query_ptr, query_embedding_num);

    // Step 1: IVF Probing - find top centroids per query token
    std::vector<std::vector<u32>> token_top_centroids(query_embedding_num);
    for (u32 i = 0; i < query_embedding_num; ++i) {
        const f32 *scores = query_centroid_scores.get() + i * n_centroids_;

        // Find centroids above threshold
        for (u32 j = 0; j < n_centroids_; ++j) {
            if (scores[j] >= centroid_score_threshold) {
                token_top_centroids[i].push_back(j);
            }
        }

        // If too few, take top n_ivf_probe
        if (token_top_centroids[i].size() < n_ivf_probe) {
            std::vector<std::pair<f32, u32>> centroid_scores;
            for (u32 j = 0; j < n_centroids_; ++j) {
                centroid_scores.emplace_back(scores[j], j);
            }
            std::partial_sort(centroid_scores.begin(),
                              centroid_scores.begin() + std::min(n_ivf_probe, n_centroids_),
                              centroid_scores.end(),
                              std::greater<>());
            token_top_centroids[i].clear();
            for (u32 j = 0; j < n_ivf_probe && j < n_centroids_; ++j) {
                token_top_centroids[i].push_back(centroid_scores[j].second);
            }
        }
    }

    // Step 2: Collect candidate documents
    std::unordered_set<u32> candidate_set;
    for (const auto &centroids : token_top_centroids) {
        for (u32 cid : centroids) {
            for (u32 doc_id : ivf_lists_[cid]) {
                candidate_set.insert(doc_id);
            }
        }
    }

    std::vector<u32> candidates(candidate_set.begin(), candidate_set.end());

    // Step 3: Compute approximate scores
    std::vector<std::pair<f32, u32>> doc_scores;
    for (u32 doc_id : candidates) {
        f32 score = ApproximateScore(&centroid_ids_[doc_offsets_[doc_id]], doc_lens_[doc_id], query_centroid_scores.get(), query_embedding_num);
        doc_scores.emplace_back(score, doc_id);
    }

    // Step 4: Select top n_doc_to_score for reranking
    std::partial_sort(doc_scores.begin(), doc_scores.begin() + std::min(n_doc_to_score, (u32)doc_scores.size()), doc_scores.end(), std::greater<>());

    const u32 n_to_rerank = std::min(n_full_scores, std::min(n_doc_to_score, (u32)doc_scores.size()));

    // Step 5: Exact reranking with residual quantization
    std::vector<std::pair<f32, u32>> final_scores;
    for (u32 i = 0; i < n_to_rerank; ++i) {
        u32 doc_id = doc_scores[i].second;
        f32 score = ExactScore(query_ptr, query_embedding_num, doc_id, nullptr);
        final_scores.emplace_back(score, doc_id);
    }

    std::partial_sort(final_scores.begin(), final_scores.begin() + std::min(top_k, (u32)final_scores.size()), final_scores.end(), std::greater<>());

    // Prepare result
    const u32 result_count = std::min(top_k, (u32)final_scores.size());
    auto scores = std::make_unique<f32[]>(result_count);
    auto ids = std::make_unique<u32[]>(result_count);

    for (u32 i = 0; i < result_count; ++i) {
        scores[i] = final_scores[i].first;
        ids[i] = final_scores[i].second + start_segment_offset_;
    }

    return std::make_tuple(result_count, std::move(scores), std::move(ids));
}

PlaidQueryResultType PlaidIndex::GetQueryResultWithBitmask(const f32 *query_ptr,
                                                           const u32 query_embedding_num,
                                                           const u32 n_ivf_probe,
                                                           const f32 centroid_score_threshold,
                                                           const u32 n_doc_to_score,
                                                           const u32 n_full_scores,
                                                           const u32 top_k,
                                                           const f32 threshold_final,
                                                           Bitmask &bitmask,
                                                           const u32 start_segment_offset,
                                                           const BlockIndex *block_index,
                                                           const TxnTimeStamp begin_ts) const {
    // Apply bitmask filtering to candidates
    std::shared_lock lock(rw_mutex_);

    // Compute query-centroid scores
    auto query_centroid_scores = ComputeQueryCentroidScores(query_ptr, query_embedding_num);

    // Step 1: IVF Probing
    std::vector<std::vector<u32>> token_top_centroids(query_embedding_num);
    for (u32 i = 0; i < query_embedding_num; ++i) {
        const f32 *scores = query_centroid_scores.get() + i * n_centroids_;

        for (u32 j = 0; j < n_centroids_; ++j) {
            if (scores[j] >= centroid_score_threshold) {
                token_top_centroids[i].push_back(j);
            }
        }

        if (token_top_centroids[i].size() < n_ivf_probe) {
            std::vector<std::pair<f32, u32>> centroid_scores;
            for (u32 j = 0; j < n_centroids_; ++j) {
                centroid_scores.emplace_back(scores[j], j);
            }
            std::partial_sort(centroid_scores.begin(),
                              centroid_scores.begin() + std::min(n_ivf_probe, n_centroids_),
                              centroid_scores.end(),
                              std::greater<>());
            token_top_centroids[i].clear();
            for (u32 j = 0; j < n_ivf_probe && j < n_centroids_; ++j) {
                token_top_centroids[i].push_back(centroid_scores[j].second);
            }
        }
    }

    // Step 2: Collect candidates with bitmask filtering
    std::unordered_set<u32> candidate_set;
    for (const auto &centroids : token_top_centroids) {
        for (u32 cid : centroids) {
            for (u32 doc_id : ivf_lists_[cid]) {
                // Apply bitmask filter
                if (!bitmask.IsTrue(doc_id + start_segment_offset))
                    continue;
                candidate_set.insert(doc_id);
            }
        }
    }

    std::vector<u32> candidates(candidate_set.begin(), candidate_set.end());

    if (candidates.empty()) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    // Step 3-5: Scoring and reranking (same as GetQueryResult)
    std::vector<std::pair<f32, u32>> doc_scores;
    for (u32 doc_id : candidates) {
        f32 score = ApproximateScore(&centroid_ids_[doc_offsets_[doc_id]], doc_lens_[doc_id], query_centroid_scores.get(), query_embedding_num);
        doc_scores.emplace_back(score, doc_id);
    }

    std::partial_sort(doc_scores.begin(), doc_scores.begin() + std::min(n_doc_to_score, (u32)doc_scores.size()), doc_scores.end(), std::greater<>());

    const u32 n_to_rerank = std::min(n_full_scores, std::min(n_doc_to_score, (u32)doc_scores.size()));

    std::vector<std::pair<f32, u32>> final_scores;
    for (u32 i = 0; i < n_to_rerank; ++i) {
        u32 doc_id = doc_scores[i].second;
        f32 score = ExactScore(query_ptr, query_embedding_num, doc_id, nullptr);
        final_scores.emplace_back(score, doc_id);
    }

    std::partial_sort(final_scores.begin(), final_scores.begin() + std::min(top_k, (u32)final_scores.size()), final_scores.end(), std::greater<>());

    const u32 result_count = std::min(top_k, (u32)final_scores.size());
    auto scores = std::make_unique<f32[]>(result_count);
    auto ids = std::make_unique<u32[]>(result_count);

    for (u32 i = 0; i < result_count; ++i) {
        scores[i] = final_scores[i].first;
        ids[i] = final_scores[i].second + start_segment_offset_;
    }

    return std::make_tuple(result_count, std::move(scores), std::move(ids));
}

std::unique_ptr<f32[]> PlaidIndex::ComputeQueryCentroidScores(const f32 *query_ptr, u32 n_query_tokens) const {
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

f32 PlaidIndex::ApproximateScore(const u32 *doc_centroid_ids, u32 doc_len, const f32 *query_centroid_scores, u32 n_query_tokens) const {
    f32 total_score = 0.0f;

    for (u32 q = 0; q < n_query_tokens; ++q) {
        f32 max_score = std::numeric_limits<f32>::lowest();
        for (u32 d = 0; d < doc_len; ++d) {
            u32 cid = doc_centroid_ids[d];
            f32 score = query_centroid_scores[q * n_centroids_ + cid];
            max_score = std::max(max_score, score);
        }
        if (max_score > std::numeric_limits<f32>::lowest()) {
            total_score += max_score;
        }
    }

    return total_score;
}

f32 PlaidIndex::ExactScore(const f32 *query_ptr, u32 n_query_tokens, u32 doc_id, const f32 *centroid_distances) const {
    f32 total_score = 0.0f;
    u32 doc_offset = doc_offsets_[doc_id];
    u32 doc_len = doc_lens_[doc_id];

    for (u32 q = 0; q < n_query_tokens; ++q) {
        const f32 *query_vec = query_ptr + q * embedding_dimension_;
        f32 max_score = std::numeric_limits<f32>::lowest();

        // Precompute IP distance table for this query token
        auto ip_table = quantizer_->GetIPDistanceTable(query_vec);

        for (u32 d = 0; d < doc_len; ++d) {
            u32 embedding_idx = doc_offset + d;
            u32 cid = centroid_ids_[embedding_idx];

            // Base centroid score
            f32 centroid_score = 0.0f;
            if (centroid_distances) {
                centroid_score = centroid_distances[d * n_query_tokens + q];
            } else {
                const f32 *centroid_vec = centroids_data_.data() + cid * embedding_dimension_;
                centroid_score = IPDistance<f32>(query_vec, centroid_vec, embedding_dimension_);
            }

            // Refined score with residual
            f32 residual_score = quantizer_->GetSingleIPDistance(embedding_idx,
                                                                 q,
                                                                 n_query_tokens,
                                                                 ip_table.get(),
                                                                 packed_residuals_.get(),
                                                                 centroid_ids_.data(),
                                                                 centroids_data_.data());

            f32 combined_score = centroid_score + residual_score * 0.1f;
            max_score = std::max(max_score, combined_score);
        }

        if (max_score > std::numeric_limits<f32>::lowest()) {
            total_score += max_score;
        }
    }

    return total_score;
}

void PlaidIndex::SaveIndexInner(LocalFileHandle &file_handle) const {
    std::shared_lock lock(rw_mutex_);

    // Header
    file_handle.Append(&start_segment_offset_, sizeof(start_segment_offset_));
    file_handle.Append(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Append(&nbits_, sizeof(nbits_));
    file_handle.Append(&n_centroids_, sizeof(n_centroids_));
    file_handle.Append(&n_docs_, sizeof(u32));
    file_handle.Append(&n_total_embeddings_, sizeof(n_total_embeddings_));

    // Centroids
    file_handle.Append(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    file_handle.Append(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Document info
    u32 doc_lens_size = doc_lens_.size();
    file_handle.Append(&doc_lens_size, sizeof(doc_lens_size));
    file_handle.Append(doc_lens_.data(), doc_lens_size * sizeof(u32));

    u32 doc_offsets_size = doc_offsets_.size();
    file_handle.Append(&doc_offsets_size, sizeof(doc_offsets_size));
    file_handle.Append(doc_offsets_.data(), doc_offsets_size * sizeof(u32));

    // Centroid assignments
    u32 centroid_ids_size = centroid_ids_.size();
    file_handle.Append(&centroid_ids_size, sizeof(centroid_ids_size));
    file_handle.Append(centroid_ids_.data(), centroid_ids_size * sizeof(u32));

    // IVF lists
    for (u32 i = 0; i < n_centroids_; ++i) {
        u32 list_size = ivf_lists_[i].size();
        file_handle.Append(&list_size, sizeof(list_size));
        file_handle.Append(ivf_lists_[i].data(), list_size * sizeof(u32));
    }

    // Residual codes
    u32 packed_residuals_size_u32 = packed_residuals_size_;
    file_handle.Append(&packed_residuals_size_u32, sizeof(packed_residuals_size_u32));
    file_handle.Append(packed_residuals_.get(), packed_residuals_size_);

    // Quantizer
    quantizer_->Save(file_handle);
}

void PlaidIndex::ReadIndexInner(LocalFileHandle &file_handle) {
    std::unique_lock lock(rw_mutex_);

    // Verify header
    u32 stored_start_segment_offset, stored_embedding_dimension, stored_nbits;
    file_handle.Read(&stored_start_segment_offset, sizeof(stored_start_segment_offset));
    file_handle.Read(&stored_embedding_dimension, sizeof(stored_embedding_dimension));
    file_handle.Read(&stored_nbits, sizeof(stored_nbits));

    if (stored_start_segment_offset != start_segment_offset_ || stored_embedding_dimension != embedding_dimension_ || stored_nbits != nbits_) {
        UnrecoverableError("PlaidIndex::ReadIndexInner: header mismatch");
    }

    file_handle.Read(&n_centroids_, sizeof(n_centroids_));
    file_handle.Read(&n_docs_, sizeof(u32));
    file_handle.Read(&n_total_embeddings_, sizeof(n_total_embeddings_));

    // Read centroids
    centroids_data_.resize(n_centroids_ * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);
    file_handle.Read(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    file_handle.Read(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Read document info
    u32 doc_lens_size;
    file_handle.Read(&doc_lens_size, sizeof(doc_lens_size));
    doc_lens_.resize(doc_lens_size);
    file_handle.Read(doc_lens_.data(), doc_lens_size * sizeof(u32));

    u32 doc_offsets_size;
    file_handle.Read(&doc_offsets_size, sizeof(doc_offsets_size));
    doc_offsets_.resize(doc_offsets_size);
    file_handle.Read(doc_offsets_.data(), doc_offsets_size * sizeof(u32));

    // Read centroid assignments
    u32 centroid_ids_size;
    file_handle.Read(&centroid_ids_size, sizeof(centroid_ids_size));
    centroid_ids_.resize(centroid_ids_size);
    file_handle.Read(centroid_ids_.data(), centroid_ids_size * sizeof(u32));

    // Read IVF lists
    ivf_lists_.resize(n_centroids_);
    for (u32 i = 0; i < n_centroids_; ++i) {
        u32 list_size;
        file_handle.Read(&list_size, sizeof(list_size));
        ivf_lists_[i].resize(list_size);
        file_handle.Read(ivf_lists_[i].data(), list_size * sizeof(u32));
    }

    // Read residual codes
    u32 packed_residuals_size_u32;
    file_handle.Read(&packed_residuals_size_u32, sizeof(packed_residuals_size_u32));
    packed_residuals_size_ = packed_residuals_size_u32;
    packed_residuals_ = std::make_unique<u8[]>(packed_residuals_size_);
    file_handle.Read(packed_residuals_.get(), packed_residuals_size_);

    // Read quantizer
    quantizer_->Load(file_handle);
}

size_t PlaidIndex::CalcSize() const {
    std::shared_lock lock(rw_mutex_);
    size_t size = 0;

    size += sizeof(start_segment_offset_);
    size += sizeof(embedding_dimension_);
    size += sizeof(nbits_);
    size += sizeof(n_centroids_);
    size += sizeof(u32); // n_docs_
    size += sizeof(n_total_embeddings_);

    size += centroids_data_.size() * sizeof(f32);
    size += centroid_norms_neg_half_.size() * sizeof(f32);

    size += sizeof(u32) + doc_lens_.size() * sizeof(u32);
    size += sizeof(u32) + doc_offsets_.size() * sizeof(u32);
    size += sizeof(u32) + centroid_ids_.size() * sizeof(u32);

    for (const auto &list : ivf_lists_) {
        size += sizeof(u32) + list.size() * sizeof(u32);
    }

    size += sizeof(u32) + packed_residuals_size_;

    // Quantizer size (nbits_ and embedding_dimension_ are already counted in PlaidIndex header)
    size += sizeof(f32);                                               // avg_residual_
    size += sizeof(u32) + (quantizer_->n_buckets() - 1) * sizeof(f32); // bucket_cutoffs (n_buckets - 1)
    size += sizeof(u32) + quantizer_->n_buckets() * sizeof(f32);       // bucket_weights

    return size;
}

size_t PlaidIndex::MemUsage() const { return CalcSize(); }

void PlaidIndex::SaveToPtr(void *ptr, size_t &offset) const {
    std::shared_lock lock(rw_mutex_);
    char *dst = static_cast<char *>(ptr);

    auto append = [&dst, &offset](const void *src, size_t len) {
        std::memcpy(dst + offset, src, len);
        offset += len;
    };

    append(&start_segment_offset_, sizeof(start_segment_offset_));
    append(&embedding_dimension_, sizeof(embedding_dimension_));
    append(&nbits_, sizeof(nbits_));
    append(&n_centroids_, sizeof(n_centroids_));
    u32 n_docs = n_docs_.load();
    append(&n_docs, sizeof(n_docs));
    append(&n_total_embeddings_, sizeof(n_total_embeddings_));

    append(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    append(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    u32 doc_lens_size = doc_lens_.size();
    append(&doc_lens_size, sizeof(doc_lens_size));
    append(doc_lens_.data(), doc_lens_.size() * sizeof(u32));

    u32 doc_offsets_size = doc_offsets_.size();
    append(&doc_offsets_size, sizeof(doc_offsets_size));
    append(doc_offsets_.data(), doc_offsets_.size() * sizeof(u32));

    u32 centroid_ids_size = centroid_ids_.size();
    append(&centroid_ids_size, sizeof(centroid_ids_size));
    append(centroid_ids_.data(), centroid_ids_.size() * sizeof(u32));

    for (const auto &list : ivf_lists_) {
        u32 list_size = list.size();
        append(&list_size, sizeof(list_size));
        append(list.data(), list.size() * sizeof(u32));
    }

    u32 packed_size = packed_residuals_size_;
    append(&packed_size, sizeof(packed_size));
    append(packed_residuals_.get(), packed_residuals_size_);

    // Save quantizer data
    quantizer_->SaveToPtr(ptr, offset);
}

void PlaidIndex::LoadFromPtr(void *ptr, size_t mmap_size, size_t file_size) {
    std::unique_lock lock(rw_mutex_);
    char *src = static_cast<char *>(ptr);
    size_t offset = 0;

    auto read = [&src, &offset](void *dst, size_t len) {
        std::memcpy(dst, src + offset, len);
        offset += len;
    };

    u32 stored_start_segment_offset, stored_embedding_dimension, stored_nbits;
    read(&stored_start_segment_offset, sizeof(stored_start_segment_offset));
    read(&stored_embedding_dimension, sizeof(stored_embedding_dimension));
    read(&stored_nbits, sizeof(stored_nbits));

    if (stored_start_segment_offset != start_segment_offset_ || stored_embedding_dimension != embedding_dimension_ || stored_nbits != nbits_) {
        UnrecoverableError("PlaidIndex::LoadFromPtr: header mismatch");
    }

    read(&n_centroids_, sizeof(n_centroids_));
    u32 n_docs;
    read(&n_docs, sizeof(n_docs));
    n_docs_ = n_docs;
    read(&n_total_embeddings_, sizeof(n_total_embeddings_));

    centroids_data_.resize(n_centroids_ * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);
    read(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    read(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    u32 doc_lens_size;
    read(&doc_lens_size, sizeof(doc_lens_size));
    doc_lens_.resize(doc_lens_size);
    read(doc_lens_.data(), doc_lens_size * sizeof(u32));

    u32 doc_offsets_size;
    read(&doc_offsets_size, sizeof(doc_offsets_size));
    doc_offsets_.resize(doc_offsets_size);
    read(doc_offsets_.data(), doc_offsets_size * sizeof(u32));

    u32 centroid_ids_size;
    read(&centroid_ids_size, sizeof(centroid_ids_size));
    centroid_ids_.resize(centroid_ids_size);
    read(centroid_ids_.data(), centroid_ids_size * sizeof(u32));

    ivf_lists_.resize(n_centroids_);
    for (u32 i = 0; i < n_centroids_; ++i) {
        u32 list_size;
        read(&list_size, sizeof(list_size));
        ivf_lists_[i].resize(list_size);
        read(ivf_lists_[i].data(), list_size * sizeof(u32));
    }

    u32 packed_size;
    read(&packed_size, sizeof(packed_size));
    packed_residuals_size_ = packed_size;
    packed_residuals_ = std::make_unique<u8[]>(packed_residuals_size_);
    read(packed_residuals_.get(), packed_residuals_size_);

    // Load quantizer data
    quantizer_->LoadFromPtr(ptr, offset);
}

u32 PlaidIndex::GetDocLen(u32 doc_id) const {
    std::shared_lock lock(rw_mutex_);
    if (doc_id >= n_docs_.load()) {
        UnrecoverableError(fmt::format("PlaidIndex::GetDocLen: Invalid doc_id {}, total docs {}", doc_id, n_docs_.load()));
    }
    return doc_lens_[doc_id];
}

std::unique_ptr<f32[]> PlaidIndex::ReconstructDocument(u32 doc_id, u32 &doc_len) const {
    std::shared_lock lock(rw_mutex_);

    if (doc_id >= n_docs_.load()) {
        UnrecoverableError(fmt::format("PlaidIndex::ReconstructDocument: Invalid doc_id {}, total docs {}", doc_id, n_docs_.load()));
    }

    // Get document range
    u32 start = doc_offsets_[doc_id];
    u32 end = (doc_id + 1 < n_docs_.load()) ? doc_offsets_[doc_id + 1] : n_total_embeddings_;
    doc_len = end - start;

    if (doc_len == 0) {
        return nullptr;
    }

    // Allocate output buffer
    auto output = std::make_unique_for_overwrite<f32[]>(doc_len * embedding_dimension_);

    // Calculate packed residual offset and size per embedding
    u32 packed_dim = (embedding_dimension_ * nbits_ + 7) / 8;

    // Dequantize using the quantizer
    quantizer_->Dequantize(&centroid_ids_[start],
                           packed_residuals_.get() + start * packed_dim,
                           doc_len,
                           output.get(),
                           centroids_data_.data(),
                           n_centroids_);

    return output;
}

std::vector<std::unique_ptr<f32[]>> PlaidIndex::ReconstructDocuments(const std::vector<u32> &doc_ids, std::vector<u32> &doc_lens) const {
    std::vector<std::unique_ptr<f32[]>> results;
    doc_lens.resize(doc_ids.size());

    for (size_t i = 0; i < doc_ids.size(); ++i) {
        u32 len = 0;
        results.push_back(ReconstructDocument(doc_ids[i], len));
        doc_lens[i] = len;
    }

    return results;
}

void PlaidIndex::StoreRawEmbeddings(const f32 *embedding_data, const u64 embedding_num) {
    std::unique_lock lock(rw_mutex_);

    // Store raw embeddings for potential rebuild
    const u64 new_size = raw_embeddings_.size() + embedding_num * embedding_dimension_;
    std::vector<f32> new_raw(new_size);

    // Copy existing embeddings
    std::copy(raw_embeddings_.begin(), raw_embeddings_.end(), new_raw.begin());

    // Copy new embeddings
    std::copy_n(embedding_data, embedding_num * embedding_dimension_, new_raw.begin() + raw_embeddings_.size());

    raw_embeddings_ = std::move(new_raw);
    raw_embeddings_count_ += embedding_num;

    LOG_INFO(fmt::format("PlaidIndex::StoreRawEmbeddings: Stored {} raw embeddings, total: {}", embedding_num, raw_embeddings_count_));
}

void PlaidIndex::ClearRawEmbeddings() {
    std::unique_lock lock(rw_mutex_);
    raw_embeddings_.clear();
    raw_embeddings_.shrink_to_fit();
    raw_embeddings_count_ = 0;
    LOG_INFO("PlaidIndex::ClearRawEmbeddings: Cleared all raw embeddings");
}

void PlaidIndex::RebuildFromRawEmbeddings(const u32 new_centroids_num, const u32 iter_cnt) {
    std::unique_lock lock(rw_mutex_);

    if (raw_embeddings_.empty() || raw_embeddings_count_ == 0) {
        LOG_WARN("PlaidIndex::RebuildFromRawEmbeddings: No raw embeddings stored, cannot rebuild");
        return;
    }

    LOG_INFO(fmt::format("PlaidIndex::RebuildFromRawEmbeddings: Rebuilding from {} raw embeddings", raw_embeddings_count_));

    // Save current document metadata for reconstruction
    const auto old_doc_lens = doc_lens_;
    const auto old_doc_offsets = doc_offsets_;
    const u32 old_n_docs = n_docs_.load();

    // Clear current index state (but keep raw embeddings)
    n_centroids_ = 0;
    centroids_data_.clear();
    centroid_norms_neg_half_.clear();
    centroid_ids_.clear();
    packed_residuals_.reset();
    packed_residuals_size_ = 0;
    ivf_lists_.clear();
    n_docs_ = 0;
    n_total_embeddings_ = 0;
    doc_lens_.clear();
    doc_offsets_.clear();
    quantizer_ = std::make_unique<PlaidQuantizer>(nbits_, embedding_dimension_);

    // Release lock during training to avoid blocking
    lock.unlock();

    // Determine number of centroids
    u32 centroids_num = new_centroids_num;
    if (centroids_num == 0) {
        centroids_num = requested_n_centroids_;
    }
    if (centroids_num == 0) {
        // Auto: sqrt(N) rounded to multiple of 8
        centroids_num = static_cast<u32>(std::sqrt(raw_embeddings_count_));
        centroids_num = ((centroids_num + 7) / 8) * 8;
        centroids_num = std::max(8u, centroids_num);
    }

    // Retrain with sampling
    TrainWithSampling(centroids_num, raw_embeddings_.data(), raw_embeddings_count_, iter_cnt);

    // Re-acquire lock
    lock.lock();

    // Re-add all documents
    u64 offset = 0;
    for (u32 i = 0; i < old_n_docs; ++i) {
        u32 doc_len = old_doc_lens[i];
        AddOneDocEmbeddings(raw_embeddings_.data() + offset, doc_len);
        offset += doc_len * embedding_dimension_;
    }

    LOG_INFO(fmt::format("PlaidIndex::RebuildFromRawEmbeddings: Rebuild complete with {} centroids, {} docs", n_centroids_, n_docs_.load()));
}

u32 PlaidIndex::UpdateWithNewEmbeddings(const f32 *embedding_data, const u64 embedding_num, const bool allow_centroid_expansion) {
    std::unique_lock lock(rw_mutex_);

    if (n_centroids_ == 0) {
        LOG_WARN("PlaidIndex::UpdateWithNewEmbeddings: Index not trained yet, cannot update");
        return 0;
    }

    LOG_INFO(
        fmt::format("PlaidIndex::UpdateWithNewEmbeddings: Adding {} new embeddings, allow_expansion={}", embedding_num, allow_centroid_expansion));

    // Store raw embeddings if in start_from_scratch mode
    if (!raw_embeddings_.empty()) {
        // Extend raw embeddings storage
        const u64 old_size = raw_embeddings_.size();
        const u64 new_size = old_size + embedding_num * embedding_dimension_;
        raw_embeddings_.resize(new_size);
        std::copy_n(embedding_data, embedding_num * embedding_dimension_, raw_embeddings_.begin() + old_size);
        raw_embeddings_count_ += embedding_num;
    }

    // Expand centroids if enabled and needed
    u32 new_centroids = 0;
    if (allow_centroid_expansion && embedding_num > 0) {
        // Check if expansion is needed by finding outliers
        const auto dist_table = std::make_unique_for_overwrite<f32[]>(embedding_num * n_centroids_);

        // Release lock during computation to allow concurrent reads
        lock.unlock();

        matrixA_multiply_transpose_matrixB_output_to_C(embedding_data,
                                                       centroids_data_.data(),
                                                       embedding_num,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());

        // Count outliers
        u32 outlier_count = 0;
        const f32 outlier_threshold = -0.1f; // Adjust based on your similarity metric
        for (u64 i = 0; i < embedding_num; ++i) {
            f32 max_sim = std::numeric_limits<f32>::lowest();
            for (u32 j = 0; j < n_centroids_; ++j) {
                f32 sim = dist_table[i * n_centroids_ + j] + centroid_norms_neg_half_[j];
                max_sim = std::max(max_sim, sim);
            }
            if (max_sim < outlier_threshold) {
                outlier_count++;
            }
        }

        // If more than 10% are outliers, trigger expansion
        if (outlier_count > embedding_num / 10) {
            LOG_INFO(
                fmt::format("PlaidIndex::UpdateWithNewEmbeddings: Found {} outliers out of {}, expanding centroids", outlier_count, embedding_num));

            // Lock is already released above, acquire it for expansion
            std::unique_lock relock(rw_mutex_);
            // ExpandCentroids assumes lock is held by caller
            ExpandCentroidsInternal(embedding_data, embedding_num, 4);
            new_centroids = n_centroids_;
        } else {
            // Re-acquire lock
            lock.lock();
        }
    }

    LOG_INFO(fmt::format("PlaidIndex::UpdateWithNewEmbeddings: Added {} new embeddings, {} new centroids", embedding_num, new_centroids));

    return new_centroids;
}

void PlaidIndex::ExpandCentroidsInternal(const f32 *new_embeddings, const u64 new_embedding_count, const u32 expand_iter) {
    // Assume lock is already held by caller
    if (new_embedding_count == 0) {
        return;
    }

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Expanding with {} new embeddings", new_embedding_count));

    // Find outliers: embeddings that are far from existing centroids
    const auto dist_table = std::make_unique_for_overwrite<f32[]>(new_embedding_count * n_centroids_);
    matrixA_multiply_transpose_matrixB_output_to_C(new_embeddings,
                                                   centroids_data_.data(),
                                                   new_embedding_count,
                                                   n_centroids_,
                                                   embedding_dimension_,
                                                   dist_table.get());

    // Find embeddings with max distance below threshold (outliers)
    std::vector<u32> outlier_indices;
    std::vector<f32> outlier_embeddings;
    const f32 outlier_threshold = -0.5f; // Threshold for cosine similarity

    for (u64 i = 0; i < new_embedding_count; ++i) {
        f32 max_sim = std::numeric_limits<f32>::lowest();
        for (u32 j = 0; j < n_centroids_; ++j) {
            f32 sim = dist_table[i * n_centroids_ + j] + centroid_norms_neg_half_[j];
            max_sim = std::max(max_sim, sim);
        }
        if (max_sim < outlier_threshold) {
            // This is an outlier, add to list
            outlier_indices.push_back(static_cast<u32>(i));
            for (u32 d = 0; d < embedding_dimension_; ++d) {
                outlier_embeddings.push_back(new_embeddings[i * embedding_dimension_ + d]);
            }
        }
    }

    if (outlier_indices.size() < 8) {
        LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Too few outliers ({}), skipping expansion", outlier_indices.size()));
        return;
    }

    // Compute new centroids from outliers using K-means
    u32 n_new_centroids = static_cast<u32>(std::sqrt(outlier_indices.size()));
    n_new_centroids = ((n_new_centroids + 7) / 8) * 8;               // Round to multiple of 8
    n_new_centroids = std::max(8u, std::min(n_new_centroids, 128u)); // Clamp between 8 and 128

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Creating {} new centroids from {} outliers", n_new_centroids, outlier_indices.size()));

    std::vector<f32> new_centroids(n_new_centroids * embedding_dimension_);
    const auto result_centroid_num = GetKMeansCentroids(MetricType::kMetricL2,
                                                        embedding_dimension_,
                                                        outlier_embeddings.size() / embedding_dimension_,
                                                        outlier_embeddings.data(),
                                                        new_centroids,
                                                        n_new_centroids,
                                                        expand_iter);

    if (result_centroid_num != n_new_centroids) {
        LOG_WARN(fmt::format("PlaidIndex::ExpandCentroids: KMeans failed to get {} centroids, got {} instead", n_new_centroids, result_centroid_num));
        return;
    }

    // Merge new centroids with existing ones
    u32 old_n_centroids = n_centroids_;
    n_centroids_ += n_new_centroids;

    // Extend centroids data
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dimension_);
    std::copy(new_centroids.begin(), new_centroids.end(), centroids_data_.begin() + old_n_centroids * embedding_dimension_);

    // Extend centroid norms
    centroid_norms_neg_half_.resize(n_centroids_);
    const f32 *centroid_data = centroids_data_.data() + old_n_centroids * embedding_dimension_;
    for (u32 i = old_n_centroids; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, embedding_dimension_);
        centroid_data += embedding_dimension_;
    }

    // Extend IVF lists
    ivf_lists_.resize(n_centroids_);

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Expanded from {} to {} centroids", old_n_centroids, n_centroids_));
}

void PlaidIndex::ExpandCentroids(const f32 *new_embeddings, const u64 new_embedding_count, const u32 expand_iter) {
    std::unique_lock lock(rw_mutex_);
    ExpandCentroidsInternal(new_embeddings, new_embedding_count, expand_iter);
}

// ============================================================================
// PlaidGlobalIVF Implementation
// ============================================================================

void PlaidGlobalIVF::Initialize(const u32 n_centroids, const u32 embedding_dim, const f32 *centroids_data) {
    std::unique_lock lock(rw_mutex_);
    n_centroids_ = n_centroids;
    embedding_dimension_ = embedding_dim;
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dim);
    centroid_norms_neg_half_.resize(n_centroids_);
    ivf_lists_.resize(n_centroids_);

    std::copy_n(centroids_data, static_cast<u64>(n_centroids_) * embedding_dim, centroids_data_.data());

    const f32 *centroid_ptr = centroids_data_.data();
    for (u32 i = 0; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_ptr, embedding_dim);
        centroid_ptr += embedding_dim;
    }
}

void PlaidGlobalIVF::AddToPostingLists(const u32 doc_id_start, const std::vector<u32> &centroid_ids, const std::vector<u32> &doc_lens) {
    std::unique_lock lock(rw_mutex_);

    u32 current_doc_id = doc_id_start;
    u32 offset = 0;

    for (u32 doc_len : doc_lens) {
        for (u32 i = 0; i < doc_len; ++i) {
            u32 centroid_id = centroid_ids[offset + i];
            if (centroid_id < ivf_lists_.size()) {
                // Only add if not already present for this doc
                if (ivf_lists_[centroid_id].empty() || ivf_lists_[centroid_id].back() != current_doc_id) {
                    ivf_lists_[centroid_id].push_back(current_doc_id);
                }
            }
        }
        offset += doc_len;
        ++current_doc_id;
    }
}

void PlaidGlobalIVF::UpdatePostingListsForChunk(const u32 chunk_id, const std::vector<u32> &doc_ids, const std::vector<u32> &centroid_ids) {
    std::unique_lock lock(rw_mutex_);

    // Remove old entries for this chunk
    // TODO: Implement chunk-based doc_id filtering
    (void)chunk_id;

    // Add new entries
    // TODO: Implement proper centroid id extraction and posting list update
    (void)doc_ids;
    (void)centroid_ids;
}

std::unique_ptr<f32[]> PlaidGlobalIVF::ComputeQueryScores(const f32 *query_ptr, const u32 query_len) const {
    std::shared_lock lock(rw_mutex_);

    auto scores = std::make_unique_for_overwrite<f32[]>(query_len * n_centroids_);

    matrixA_multiply_transpose_matrixB_output_to_C(query_ptr, centroids_data_.data(), query_len, n_centroids_, embedding_dimension_, scores.get());

    for (u32 i = 0; i < query_len; ++i) {
        for (u32 j = 0; j < n_centroids_; ++j) {
            scores[i * n_centroids_ + j] += centroid_norms_neg_half_[j];
        }
    }

    return scores;
}

std::vector<u32> PlaidGlobalIVF::GetCandidates(const std::vector<u32> &centroid_ids) const {
    std::shared_lock lock(rw_mutex_);

    std::unordered_set<u32> candidate_set;
    for (u32 cid : centroid_ids) {
        if (cid < ivf_lists_.size()) {
            candidate_set.insert(ivf_lists_[cid].begin(), ivf_lists_[cid].end());
        }
    }

    return std::vector<u32>(candidate_set.begin(), candidate_set.end());
}

void PlaidGlobalIVF::Save(const std::string &file_path) const {
    std::shared_lock lock(rw_mutex_);

    auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("PlaidGlobalIVF::Save: Failed to open file: {}", file_path));
    }

    // Write header
    file_handle->Append(&n_centroids_, sizeof(n_centroids_));
    file_handle->Append(&embedding_dimension_, sizeof(embedding_dimension_));

    // Write centroids
    file_handle->Append(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    file_handle->Append(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Write IVF lists
    for (u32 i = 0; i < n_centroids_; ++i) {
        u32 list_size = ivf_lists_[i].size();
        file_handle->Append(&list_size, sizeof(list_size));
        if (list_size > 0) {
            file_handle->Append(ivf_lists_[i].data(), list_size * sizeof(u32));
        }
    }
}

void PlaidGlobalIVF::Load(const std::string &file_path) {
    std::unique_lock lock(rw_mutex_);

    auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("PlaidGlobalIVF::Load: Failed to open file: {}", file_path));
    }

    // Read header
    std::ignore = file_handle->Read(&n_centroids_, sizeof(n_centroids_));
    std::ignore = file_handle->Read(&embedding_dimension_, sizeof(embedding_dimension_));

    // Read centroids
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);

    std::ignore = file_handle->Read(centroids_data_.data(), centroids_data_.size() * sizeof(f32));
    std::ignore = file_handle->Read(centroid_norms_neg_half_.data(), centroid_norms_neg_half_.size() * sizeof(f32));

    // Read IVF lists
    ivf_lists_.resize(n_centroids_);
    for (u32 i = 0; i < n_centroids_; ++i) {
        u32 list_size;
        std::ignore = file_handle->Read(&list_size, sizeof(list_size));
        ivf_lists_[i].resize(list_size);
        if (list_size > 0) {
            std::ignore = file_handle->Read(ivf_lists_[i].data(), list_size * sizeof(u32));
        }
    }
}

u32 PlaidGlobalIVF::ExpandCentroids(const f32 *new_embeddings, const u64 new_embedding_count, const u32 embedding_dim, const u32 expand_iter) {
    std::unique_lock lock(rw_mutex_);

    if (new_embedding_count == 0) {
        return 0;
    }

    // Find outliers
    const auto dist_table = std::make_unique_for_overwrite<f32[]>(new_embedding_count * n_centroids_);
    matrixA_multiply_transpose_matrixB_output_to_C(new_embeddings,
                                                   centroids_data_.data(),
                                                   new_embedding_count,
                                                   n_centroids_,
                                                   embedding_dim,
                                                   dist_table.get());

    std::vector<u32> outlier_indices;
    std::vector<f32> outlier_embeddings;
    const f32 outlier_threshold = -0.5f;

    for (u64 i = 0; i < new_embedding_count; ++i) {
        f32 max_sim = std::numeric_limits<f32>::lowest();
        for (u32 j = 0; j < n_centroids_; ++j) {
            f32 sim = dist_table[i * n_centroids_ + j] + centroid_norms_neg_half_[j];
            max_sim = std::max(max_sim, sim);
        }
        if (max_sim < outlier_threshold) {
            outlier_indices.push_back(static_cast<u32>(i));
            for (u32 d = 0; d < embedding_dim; ++d) {
                outlier_embeddings.push_back(new_embeddings[i * embedding_dim + d]);
            }
        }
    }

    if (outlier_indices.size() < 8) {
        LOG_INFO(fmt::format("PlaidGlobalIVF::ExpandCentroids: Too few outliers ({}), skipping expansion", outlier_indices.size()));
        return 0;
    }

    // Compute new centroids
    u32 n_new_centroids = static_cast<u32>(std::sqrt(outlier_indices.size()));
    n_new_centroids = ((n_new_centroids + 7) / 8) * 8;
    n_new_centroids = std::max(8u, std::min(n_new_centroids, 128u));

    std::vector<f32> new_centroids(n_new_centroids * embedding_dim);
    const auto result_centroid_num = GetKMeansCentroids(MetricType::kMetricL2,
                                                        embedding_dim,
                                                        outlier_embeddings.size() / embedding_dim,
                                                        outlier_embeddings.data(),
                                                        new_centroids,
                                                        n_new_centroids,
                                                        expand_iter);

    if (result_centroid_num != n_new_centroids) {
        LOG_WARN(fmt::format("PlaidGlobalIVF::ExpandCentroids: KMeans failed, expected {}, got {}", n_new_centroids, result_centroid_num));
        return 0;
    }

    // Merge new centroids
    u32 old_n_centroids = n_centroids_;
    n_centroids_ += n_new_centroids;

    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dim);
    centroid_norms_neg_half_.resize(n_centroids_);
    ivf_lists_.resize(n_centroids_);

    std::copy(new_centroids.begin(), new_centroids.end(), centroids_data_.begin() + old_n_centroids * embedding_dim);

    const f32 *centroid_ptr = centroids_data_.data() + old_n_centroids * embedding_dim;
    for (u32 i = old_n_centroids; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_ptr, embedding_dim);
        centroid_ptr += embedding_dim;
    }

    LOG_INFO(fmt::format("PlaidGlobalIVF::ExpandCentroids: Expanded from {} to {} centroids", old_n_centroids, n_centroids_));
    return n_new_centroids;
}

void PlaidGlobalIVF::RebuildCentroids(const f32 *all_embeddings, const u64 total_embeddings, const u32 embedding_dim, const u32 target_n_centroids) {
    std::unique_lock lock(rw_mutex_);

    n_centroids_ = target_n_centroids;
    embedding_dimension_ = embedding_dim;

    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dim);
    centroid_norms_neg_half_.resize(n_centroids_);
    ivf_lists_.clear();
    ivf_lists_.resize(n_centroids_);

    const auto result_centroid_num =
        GetKMeansCentroids(MetricType::kMetricL2, embedding_dim, total_embeddings, all_embeddings, centroids_data_, n_centroids_, 4);

    if (result_centroid_num != n_centroids_) {
        LOG_WARN(fmt::format("PlaidGlobalIVF::RebuildCentroids: KMeans failed, expected {}, got {}", n_centroids_, result_centroid_num));
        n_centroids_ = result_centroid_num;
        centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dim);
        centroid_norms_neg_half_.resize(n_centroids_);
        ivf_lists_.resize(n_centroids_);
    }

    const f32 *centroid_ptr = centroids_data_.data();
    for (u32 i = 0; i < n_centroids_; ++i) {
        centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_ptr, embedding_dim);
        centroid_ptr += embedding_dim;
    }

    LOG_INFO(fmt::format("PlaidGlobalIVF::RebuildCentroids: Rebuilt with {} centroids", n_centroids_));
}

// ============================================================================
// PlaidIndexChunk Implementation
// ============================================================================

PlaidIndexChunk::PlaidIndexChunk(const u32 chunk_id, const u32 start_doc_id) : chunk_id_(chunk_id), start_doc_id_(start_doc_id) {}

void PlaidIndexChunk::Load(const std::string &chunk_dir) {
    std::unique_lock lock(rw_mutex_);

    // Load centroid ids (codes.npy style)
    std::string codes_path = chunk_dir + "/codes.bin";
    auto [codes_file, codes_status] = VirtualStore::Open(codes_path, FileAccessMode::kRead);
    if (codes_status.ok()) {
        size_t file_size = codes_file->FileSize();
        centroid_ids_.resize(file_size / sizeof(u32));
        std::ignore = codes_file->Read(centroid_ids_.data(), file_size);
    }

    // Load residuals (residuals.npy style)
    std::string residuals_path = chunk_dir + "/residuals.bin";
    auto [res_file, res_status] = VirtualStore::Open(residuals_path, FileAccessMode::kRead);
    if (res_status.ok()) {
        size_t file_size = res_file->FileSize();
        packed_residuals_.resize(file_size);
        std::ignore = res_file->Read(packed_residuals_.data(), file_size);
    }

    // Load doc lens (doclens.json style - simplified binary format)
    std::string doclens_path = chunk_dir + "/doclens.bin";
    auto [doc_file, doc_status] = VirtualStore::Open(doclens_path, FileAccessMode::kRead);
    if (doc_status.ok()) {
        size_t file_size = doc_file->FileSize();
        doc_lens_.resize(file_size / sizeof(u32));
        std::ignore = doc_file->Read(doc_lens_.data(), file_size);
    }

    // Rebuild doc offsets
    doc_offsets_.resize(doc_lens_.size());
    u32 offset = 0;
    for (size_t i = 0; i < doc_lens_.size(); ++i) {
        doc_offsets_[i] = offset;
        offset += doc_lens_[i];
    }
}

void PlaidIndexChunk::Save(const std::string &chunk_dir) const {
    std::shared_lock lock(rw_mutex_);

    // Create directory if not exists
    std::ignore = VirtualStore::MakeDirectory(chunk_dir);

    // Save centroid ids
    std::string codes_path = chunk_dir + "/codes.bin";
    auto [codes_file, codes_status] = VirtualStore::Open(codes_path, FileAccessMode::kWrite);
    if (codes_status.ok() && !centroid_ids_.empty()) {
        codes_file->Append(centroid_ids_.data(), centroid_ids_.size() * sizeof(u32));
    }

    // Save residuals
    std::string residuals_path = chunk_dir + "/residuals.bin";
    auto [res_file, res_status] = VirtualStore::Open(residuals_path, FileAccessMode::kWrite);
    if (res_status.ok() && !packed_residuals_.empty()) {
        res_file->Append(packed_residuals_.data(), packed_residuals_.size());
    }

    // Save doc lens
    std::string doclens_path = chunk_dir + "/doclens.bin";
    auto [doc_file, doc_status] = VirtualStore::Open(doclens_path, FileAccessMode::kWrite);
    if (doc_status.ok() && !doc_lens_.empty()) {
        doc_file->Append(doc_lens_.data(), doc_lens_.size() * sizeof(u32));
    }
}

bool PlaidIndexChunk::AppendData(const std::vector<u32> &centroid_ids, const std::vector<u8> &packed_residuals, const std::vector<u32> &doc_lens) {
    std::unique_lock lock(rw_mutex_);

    // Check if we can append (chunk size limit)
    if (doc_lens_.size() + doc_lens.size() > DEFAULT_CHUNK_SIZE) {
        return false;
    }

    // Append centroid ids
    centroid_ids_.insert(centroid_ids_.end(), centroid_ids.begin(), centroid_ids.end());

    // Append residuals
    packed_residuals_.insert(packed_residuals_.end(), packed_residuals.begin(), packed_residuals.end());

    // Append doc lens and update offsets
    u32 offset = centroid_ids_.size();
    for (u32 doc_len : doc_lens) {
        doc_lens_.push_back(doc_len);
        doc_offsets_.push_back(offset);
        offset += doc_len;
    }

    return true;
}

void PlaidIndexChunk::Rewrite(const std::vector<u32> &centroid_ids,
                              const std::vector<u8> &packed_residuals,
                              const std::vector<u32> &doc_lens,
                              const u32 embedding_count) {
    std::unique_lock lock(rw_mutex_);

    centroid_ids_ = centroid_ids;
    packed_residuals_ = packed_residuals;
    doc_lens_ = doc_lens;

    // Rebuild offsets
    doc_offsets_.resize(doc_lens_.size());
    u32 offset = 0;
    for (size_t i = 0; i < doc_lens_.size(); ++i) {
        doc_offsets_[i] = offset;
        offset += doc_lens_[i];
    }
}

bool PlaidIndexChunk::CanAppend(const u32 additional_docs) const {
    std::shared_lock lock(rw_mutex_);
    return doc_lens_.size() + additional_docs <= DEFAULT_CHUNK_SIZE;
}

// ============================================================================
// PlaidSegmentIndex Implementation
// ============================================================================

PlaidSegmentIndex::PlaidSegmentIndex(const SegmentID segment_id, const std::string &index_dir) : segment_id_(segment_id), index_dir_(index_dir) {
    ivf_file_path_ = index_dir + "/ivf.bin";
    meta_file_path_ = index_dir + "/meta.bin";
}

void PlaidSegmentIndex::Load() {
    std::unique_lock lock(rw_mutex_);

    // Load IVF
    if (VirtualStore::Exists(ivf_file_path_)) {
        global_ivf_.Load(ivf_file_path_);
    }

    // Load metadata and chunk list
    if (VirtualStore::Exists(meta_file_path_)) {
        auto [meta_file, meta_status] = VirtualStore::Open(meta_file_path_, FileAccessMode::kRead);
        if (meta_status.ok()) {
            u32 chunk_count;
            std::ignore = meta_file->Read(&chunk_count, sizeof(chunk_count));
            std::ignore = meta_file->Read(&next_doc_id_, sizeof(next_doc_id_));

            chunks_.clear();
            for (u32 i = 0; i < chunk_count; ++i) {
                u32 chunk_id;
                u32 start_doc_id;
                std::ignore = meta_file->Read(&chunk_id, sizeof(chunk_id));
                std::ignore = meta_file->Read(&start_doc_id, sizeof(start_doc_id));

                auto chunk = std::make_unique<PlaidIndexChunk>(chunk_id, start_doc_id);
                chunk->Load(GetChunkDir(chunk_id));
                chunks_.push_back(std::move(chunk));
            }
        }
    }
}

void PlaidSegmentIndex::Save() const {
    std::shared_lock lock(rw_mutex_);

    // Save IVF
    global_ivf_.Save(ivf_file_path_);

    // Save metadata
    std::ignore = VirtualStore::MakeDirectory(index_dir_);
    auto [meta_file, meta_status] = VirtualStore::Open(meta_file_path_, FileAccessMode::kWrite);
    if (meta_status.ok()) {
        u32 chunk_count = chunks_.size();
        meta_file->Append(&chunk_count, sizeof(chunk_count));
        meta_file->Append(&next_doc_id_, sizeof(next_doc_id_));

        for (const auto &chunk : chunks_) {
            u32 chunk_id = chunk->GetChunkID();
            u32 start_doc_id = chunk->GetStartDocID();
            meta_file->Append(&chunk_id, sizeof(chunk_id));
            meta_file->Append(&start_doc_id, sizeof(start_doc_id));
        }
    }

    // Save each chunk
    for (const auto &chunk : chunks_) {
        chunk->Save(GetChunkDir(chunk->GetChunkID()));
    }
}

Status PlaidSegmentIndex::AppendData(const std::vector<u32> &centroid_ids,
                                     const std::vector<u8> &packed_residuals,
                                     const std::vector<u32> &doc_lens,
                                     const u32 embedding_count,
                                     ChunkID &out_chunk_id) {
    std::unique_lock lock(rw_mutex_);

    // Try to append to last chunk
    if (!chunks_.empty() && chunks_.back()->CanAppend(doc_lens.size())) {
        auto &last_chunk = chunks_.back();
        u32 start_doc_id = last_chunk->GetEndDocID();

        if (last_chunk->AppendData(centroid_ids, packed_residuals, doc_lens)) {
            out_chunk_id = last_chunk->GetChunkID();

            // Update IVF
            global_ivf_.AddToPostingLists(start_doc_id, centroid_ids, doc_lens);

            return Status::OK();
        }
    }

    // Create new chunk
    out_chunk_id = CreateNewChunk(centroid_ids, packed_residuals, doc_lens);
    return Status::OK();
}

Status PlaidSegmentIndex::AppendToLastChunk(const std::vector<u32> &centroid_ids,
                                            const std::vector<u8> &packed_residuals,
                                            const std::vector<u32> &doc_lens,
                                            ChunkID &out_chunk_id) {
    std::unique_lock lock(rw_mutex_);

    if (chunks_.empty()) {
        out_chunk_id = CreateNewChunk(centroid_ids, packed_residuals, doc_lens);
        return Status::OK();
    }

    auto &last_chunk = chunks_.back();
    u32 start_doc_id = last_chunk->GetEndDocID();

    if (!last_chunk->AppendData(centroid_ids, packed_residuals, doc_lens)) {
        out_chunk_id = CreateNewChunk(centroid_ids, packed_residuals, doc_lens);
    } else {
        out_chunk_id = last_chunk->GetChunkID();
        global_ivf_.AddToPostingLists(start_doc_id, centroid_ids, doc_lens);
    }

    return Status::OK();
}

ChunkID
PlaidSegmentIndex::CreateNewChunk(const std::vector<u32> &centroid_ids, const std::vector<u8> &packed_residuals, const std::vector<u32> &doc_lens) {
    ChunkID chunk_id = chunks_.size();
    u32 start_doc_id = next_doc_id_;

    auto chunk = std::make_unique<PlaidIndexChunk>(chunk_id, start_doc_id);
    chunk->AppendData(centroid_ids, packed_residuals, doc_lens);

    chunks_.push_back(std::move(chunk));
    next_doc_id_ += doc_lens.size();

    // Update IVF
    global_ivf_.AddToPostingLists(start_doc_id, centroid_ids, doc_lens);

    return chunk_id;
}

Status PlaidSegmentIndex::RewriteLastChunk(const std::vector<u32> &new_centroid_ids,
                                           const std::vector<u8> &new_packed_residuals,
                                           const std::vector<u32> &new_doc_lens,
                                           const u32 new_embedding_count) {
    std::unique_lock lock(rw_mutex_);

    if (chunks_.empty()) {
        return Status::UnexpectedError("No chunks to rewrite");
    }

    auto &last_chunk = chunks_.back();
    last_chunk->Rewrite(new_centroid_ids, new_packed_residuals, new_doc_lens, new_embedding_count);

    // Note: IVF update should be handled separately with full reindexing

    return Status::OK();
}

u32 PlaidSegmentIndex::GetTotalDocCount() const {
    std::shared_lock lock(rw_mutex_);
    u32 total = 0;
    for (const auto &chunk : chunks_) {
        total += chunk->GetDocCount();
    }
    return total;
}

u32 PlaidSegmentIndex::GetTotalEmbeddingCount() const {
    std::shared_lock lock(rw_mutex_);
    u32 total = 0;
    for (const auto &chunk : chunks_) {
        total += chunk->GetEmbeddingCount();
    }
    return total;
}

std::vector<std::pair<ChunkID, u32>> PlaidSegmentIndex::GetChunkInfos() const {
    std::shared_lock lock(rw_mutex_);
    std::vector<std::pair<ChunkID, u32>> infos;
    for (const auto &chunk : chunks_) {
        infos.emplace_back(chunk->GetChunkID(), chunk->GetDocCount());
    }
    return infos;
}

std::string PlaidSegmentIndex::GetChunkDir(const ChunkID chunk_id) const { return index_dir_ + "/chunk_" + std::to_string(chunk_id); }

PlaidQueryResultType PlaidSegmentIndex::Search(const f32 *query_ptr,
                                               const u32 query_embedding_num,
                                               const u32 top_n,
                                               Bitmask &bitmask,
                                               const BlockIndex *block_index,
                                               const TxnTimeStamp begin_ts,
                                               const u32 n_ivf_probe,
                                               const f32 centroid_score_threshold,
                                               const u32 n_doc_to_score,
                                               const u32 n_full_scores) const {
    std::shared_lock lock(rw_mutex_);

    // Compute query-centroid scores using global IVF
    auto query_centroid_scores = global_ivf_.ComputeQueryScores(query_ptr, query_embedding_num);

    // Find top centroids per query token
    const u32 n_centroids = global_ivf_.GetNCentroids();
    std::vector<std::vector<u32>> token_top_centroids(query_embedding_num);

    for (u32 i = 0; i < query_embedding_num; ++i) {
        const f32 *scores = query_centroid_scores.get() + i * n_centroids;

        // Collect centroids above threshold
        for (u32 j = 0; j < n_centroids; ++j) {
            if (scores[j] >= centroid_score_threshold) {
                token_top_centroids[i].push_back(j);
            }
        }

        // If too few, take top n_ivf_probe
        if (token_top_centroids[i].size() < n_ivf_probe) {
            std::vector<std::pair<f32, u32>> centroid_scores;
            for (u32 j = 0; j < n_centroids; ++j) {
                centroid_scores.emplace_back(scores[j], j);
            }
            std::partial_sort(centroid_scores.begin(),
                              centroid_scores.begin() + std::min(n_ivf_probe, n_centroids),
                              centroid_scores.end(),
                              std::greater<>());
            token_top_centroids[i].clear();
            for (u32 j = 0; j < n_ivf_probe && j < n_centroids; ++j) {
                token_top_centroids[i].push_back(centroid_scores[j].second);
            }
        }
    }

    // Get global candidates from IVF
    std::vector<u32> all_centroids;
    for (const auto &centroids : token_top_centroids) {
        all_centroids.insert(all_centroids.end(), centroids.begin(), centroids.end());
    }
    auto global_candidates = global_ivf_.GetCandidates(all_centroids);

    // Score candidates (simplified - real implementation needs proper scoring)
    std::vector<std::pair<f32, u32>> doc_scores;
    for (u32 global_doc_id : global_candidates) {
        // Apply bitmask filter
        if (!bitmask.IsTrue(global_doc_id)) {
            continue;
        }

        // Find which chunk this doc belongs to
        for (const auto &chunk : chunks_) {
            if (global_doc_id >= chunk->GetStartDocID() && global_doc_id < chunk->GetEndDocID()) {
                // Compute approximate score (simplified)
                f32 score = 0.0f;
                doc_scores.emplace_back(score, global_doc_id);
                break;
            }
        }
    }

    // Sort and return top results
    std::partial_sort(doc_scores.begin(), doc_scores.begin() + std::min(top_n, (u32)doc_scores.size()), doc_scores.end(), std::greater<>());

    const u32 result_count = std::min(top_n, (u32)doc_scores.size());
    auto scores = std::make_unique_for_overwrite<f32[]>(result_count);
    auto ids = std::make_unique_for_overwrite<u32[]>(result_count);

    for (u32 i = 0; i < result_count; ++i) {
        scores[i] = doc_scores[i].first;
        ids[i] = doc_scores[i].second;
    }

    return std::make_tuple(result_count, std::move(scores), std::move(ids));
}

} // namespace infinity
