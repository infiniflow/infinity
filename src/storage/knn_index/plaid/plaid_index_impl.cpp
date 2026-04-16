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

export module infinity_core:plaid_index.impl;

import :plaid_index;
import :plaid_quantizer;
import :plaid_global_centroids;
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
      centroid_norms_neg_half_(std::move(other.centroid_norms_neg_half_)), global_centroids_ref_(std::move(other.global_centroids_ref_)),
      n_docs_(other.n_docs_.load()), n_total_embeddings_(other.n_total_embeddings_), doc_lens_(std::move(other.doc_lens_)),
      doc_offsets_(std::move(other.doc_offsets_)), centroid_ids_(std::move(other.centroid_ids_)),
      packed_residuals_(std::move(other.packed_residuals_)), packed_residuals_size_(other.packed_residuals_size_),
      packed_residuals_capacity_(other.packed_residuals_capacity_), ivf_lists_(std::move(other.ivf_lists_)), quantizer_(std::move(other.quantizer_)),
      mmap_addr_(other.mmap_addr_), mmap_size_(other.mmap_size_), is_mmap_(other.is_mmap_), owns_data_(other.owns_data_) {
    other.mmap_addr_ = nullptr;
    other.mmap_size_ = 0;
    other.n_docs_ = 0;
    other.n_total_embeddings_ = 0;
    other.packed_residuals_size_ = 0;
    other.packed_residuals_capacity_ = 0;
}

PlaidIndex &PlaidIndex::operator=(PlaidIndex &&other) {
    if (this != &other) {
        std::unique_lock lock(rw_mutex_);
        std::unique_lock other_lock(other.rw_mutex_);

        n_centroids_ = other.n_centroids_;
        centroids_data_ = std::move(other.centroids_data_);
        centroid_norms_neg_half_ = std::move(other.centroid_norms_neg_half_);
        global_centroids_ref_ = std::move(other.global_centroids_ref_);
        n_docs_ = other.n_docs_.load();
        n_total_embeddings_ = other.n_total_embeddings_;
        doc_lens_ = std::move(other.doc_lens_);
        doc_offsets_ = std::move(other.doc_offsets_);
        centroid_ids_ = std::move(other.centroid_ids_);
        packed_residuals_ = std::move(other.packed_residuals_);
        packed_residuals_size_ = other.packed_residuals_size_;
        packed_residuals_capacity_ = other.packed_residuals_capacity_;
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
        other.packed_residuals_capacity_ = 0;
    }
    return *this;
}

u64 PlaidIndex::ExpectLeastTrainingDataNum() const { return std::max<u64>(32ul * n_centroids_, 32ul * quantizer_->n_buckets()); }

void PlaidIndex::Train(const u32 centroids_num, const f32 *embedding_data, const u64 embedding_num, const u32 iter_cnt) {
    std::unique_lock lock(rw_mutex_);

    n_centroids_ = centroids_num;
    if (n_centroids_ == 0 || (n_centroids_ % 8) != 0) {
        const auto error_msg = fmt::format("PlaidIndex::TrainWithSampling: n_centroids_ must be a multiple of 8, got {} instead.", n_centroids_);
        UnrecoverableError(error_msg);
    }

    if (const auto least_num = ExpectLeastTrainingDataNum(); embedding_num < least_num) {
        LOG_WARN(fmt::format("PlaidIndex::TrainWithSampling: embedding_num ({}) < minimum ({}). Proceeding with reduced quality.",
                             embedding_num,
                             least_num));
    }

    // Use fastkmeans-rs sampling strategy: max k * 256 samples
    // This is more aggressive than the old formula and ensures fast training
    const u64 max_sample_size = static_cast<u64>(n_centroids_) * 256;
    const u64 actual_sample_size = std::min(max_sample_size, embedding_num);

    const f32 *training_data = embedding_data;
    std::unique_ptr<f32[]> sampled_data;
    u64 training_num = embedding_num;

    if (actual_sample_size < embedding_num) {
        LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Using sampling. Total embeddings: {}, Sample size: {} ({}%)",
                             embedding_num,
                             actual_sample_size,
                             static_cast<int>(100.0 * actual_sample_size / embedding_num)));

        // Random sampling using Reservoir Sampling (memory-efficient)
        // Avoids creating large indices array and full shuffle (saves ~8MB + overhead)
        sampled_data = std::make_unique_for_overwrite<f32[]>(actual_sample_size * embedding_dimension_);

        std::random_device rd;
        std::mt19937 gen(rd());

        // Reservoir sampling: O(n) time, O(k) space where k = actual_sample_size
        // First fill with first k elements
        for (u64 i = 0; i < actual_sample_size; ++i) {
            std::copy_n(embedding_data + i * embedding_dimension_, embedding_dimension_, sampled_data.get() + i * embedding_dimension_);
        }

        // Then replace with decreasing probability
        std::uniform_int_distribution<u64> dist(0, std::numeric_limits<u64>::max());
        for (u64 i = actual_sample_size; i < embedding_num; ++i) {
            u64 j = dist(gen) % (i + 1);
            if (j < actual_sample_size) {
                std::copy_n(embedding_data + i * embedding_dimension_, embedding_dimension_, sampled_data.get() + j * embedding_dimension_);
            }
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

    // IMPORTANT: Free sampled_data immediately after K-means training
    // This releases ~4GB memory before quantizer training
    if (sampled_data) {
        sampled_data.reset();
        LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Freed K-means sampling data, saved ~{} MB",
                             actual_sample_size * embedding_dimension_ * sizeof(f32) / (1024 * 1024)));
    }

    // Sample residuals for quantizer training (following next-plaid's strategy)
    // Use at most 50,000 embeddings to train the quantizer, saving ~16GB memory
    constexpr u64 MAX_SAMPLE_FOR_QUANTIZER = 50000;
    const u64 quantizer_sample_size = std::min(MAX_SAMPLE_FOR_QUANTIZER, embedding_num);

    // Random sampling for quantizer training using Reservoir Sampling (memory-efficient)
    std::vector<u64> sample_indices;
    sample_indices.reserve(quantizer_sample_size);
    for (u64 i = 0; i < quantizer_sample_size; ++i) {
        sample_indices.push_back(i);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<u64> dist(0, std::numeric_limits<u64>::max());
    for (u64 i = quantizer_sample_size; i < embedding_num; ++i) {
        u64 j = dist(gen) % (i + 1);
        if (j < quantizer_sample_size) {
            sample_indices[j] = i;
        }
    }

    // Compute residuals only for sampled embeddings
    const auto sample_residuals = std::make_unique_for_overwrite<f32[]>(quantizer_sample_size * embedding_dimension_);
    {
        // Process in chunks to limit memory usage
        constexpr u32 CHUNK_SIZE = 65536;
        const auto dist_table_chunk = std::make_unique_for_overwrite<f32[]>(CHUNK_SIZE * n_centroids_);

        for (u64 chunk_start = 0; chunk_start < quantizer_sample_size; chunk_start += CHUNK_SIZE) {
            const u32 chunk_end = std::min(static_cast<u32>(chunk_start + CHUNK_SIZE), static_cast<u32>(quantizer_sample_size));
            const u32 chunk_count = chunk_end - chunk_start;

            // Prepare chunk data from sampled indices
            const auto chunk_embeddings = std::make_unique_for_overwrite<f32[]>(chunk_count * embedding_dimension_);
            for (u32 i = 0; i < chunk_count; ++i) {
                const u64 orig_idx = sample_indices[chunk_start + i];
                std::copy_n(embedding_data + orig_idx * embedding_dimension_,
                            embedding_dimension_,
                            chunk_embeddings.get() + i * embedding_dimension_);
            }

            // Compute distances for this chunk
            matrixA_multiply_transpose_matrixB_output_to_C(chunk_embeddings.get(),
                                                           centroids_data_.data(),
                                                           chunk_count,
                                                           n_centroids_,
                                                           embedding_dimension_,
                                                           dist_table_chunk.get());

            // Process chunk to find nearest centroids and compute residuals
            for (u32 i = 0; i < chunk_count; ++i) {
                const f32 *embedding_data_ptr = chunk_embeddings.get() + i * embedding_dimension_;
                f32 *output_ptr = sample_residuals.get() + (chunk_start + i) * embedding_dimension_;
                f32 max_neg_distance = std::numeric_limits<f32>::lowest();
                u32 max_id = 0;
                const f32 *dist_ptr = dist_table_chunk.get() + i * n_centroids_;

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
    }

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto residual_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Sampled {} residuals for quantizer training (from {} total), time cost: {} ms",
                         quantizer_sample_size,
                         embedding_num,
                         residual_ms));

    // Train residual quantizer with sampled residuals (saves ~16GB memory)
    quantizer_->Train(sample_residuals.get(), quantizer_sample_size);

    const auto time_3 = std::chrono::high_resolution_clock::now();
    auto quantizer_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_3 - time_2).count();
    LOG_INFO(fmt::format("PlaidIndex::TrainWithSampling: Finish train residual quantizer, time cost: {} ms", quantizer_ms));

    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_3 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndex::Train: Finish train, total time cost: {} ms", total_ms));
}

void PlaidIndex::AddMultipleDocsEmbeddings(const f32 *embedding_data, const std::vector<u32> &doc_lens) {
    if (doc_lens.empty())
        return;

    std::unique_lock lock(rw_mutex_);

    const u32 old_doc_num = n_docs_;
    const u32 old_total_embeddings = n_total_embeddings_;

    // Calculate total embeddings
    u64 total_embedding_num = 0;
    for (u32 len : doc_lens)
        total_embedding_num += len;

    // Assign to centroids and quantize residuals using STREAMING processing
    // Following next-plaid's strategy: compute residuals -> quantize immediately -> release memory
    std::vector<u32> centroid_id_assignments(total_embedding_num);

    // Get packed dimension from quantizer (constant for all chunks)
    // Use ceiling division to match PlaidQuantizer::packed_dim() computation
    const u32 packed_dim = (embedding_dimension_ * nbits_ + 7) / 8;
    const size_t new_data_size = total_embedding_num * packed_dim;
    const size_t old_packed_size = packed_residuals_size_;

    // IMPORTANT: First release old packed_residuals_ to temporary, then allocate new memory
    // This minimizes peak memory: at any point we only have old OR new, not both
    auto old_packed = std::move(packed_residuals_);
    packed_residuals_size_ = 0; // Reset size since we're moving ownership

    // Now allocate new buffer (old memory is in 'old_packed', will be freed after copy)
    auto new_packed = std::make_unique<u8[]>(old_packed_size + new_data_size);

    // Copy old data first (if any), then immediately release old memory
    if (old_packed_size > 0 && old_packed) {
        std::copy_n(old_packed.get(), old_packed_size, new_packed.get());
        old_packed.reset(); // IMMEDIATELY release old memory
    }

    // Process in chunks and write new data directly to the buffer
    {
        // Process in chunks to limit memory usage
        // Target: keep distance table under ~256MB (64M floats)
        // For n_centroids=4096, chunk_size = 64M / 4096 = 16384
        constexpr u32 CHUNK_SIZE = 16384;
        const auto dist_table_chunk = std::make_unique_for_overwrite<f32[]>(CHUNK_SIZE * n_centroids_);

        for (u64 chunk_start = 0; chunk_start < total_embedding_num; chunk_start += CHUNK_SIZE) {
            const u32 chunk_end = std::min(static_cast<u32>(chunk_start + CHUNK_SIZE), static_cast<u32>(total_embedding_num));
            const u32 chunk_count = chunk_end - chunk_start;

            // Compute distances for this chunk
            matrixA_multiply_transpose_matrixB_output_to_C(embedding_data + chunk_start * embedding_dimension_,
                                                           centroids_data_.data(),
                                                           chunk_count,
                                                           n_centroids_,
                                                           embedding_dimension_,
                                                           dist_table_chunk.get());

            // Compute residuals for this chunk (STREAMING: only allocate chunk-sized residuals)
            const auto chunk_residuals = std::make_unique_for_overwrite<f32[]>(chunk_count * embedding_dimension_);

            for (u32 i = 0; i < chunk_count; ++i) {
                const f32 *embedding_data_ptr = embedding_data + (chunk_start + i) * embedding_dimension_;
                f32 *output_ptr = chunk_residuals.get() + i * embedding_dimension_;
                f32 max_neg_distance = std::numeric_limits<f32>::lowest();
                u32 max_id = 0;
                const f32 *dist_ptr = dist_table_chunk.get() + i * n_centroids_;

                for (u32 k = 0; k < n_centroids_; ++k) {
                    if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half_[k]; neg_distance > max_neg_distance) {
                        max_neg_distance = neg_distance;
                        max_id = k;
                    }
                }

                centroid_id_assignments[chunk_start + i] = max_id;
                const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
                for (u32 j = 0; j < embedding_dimension_; ++j) {
                    output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
                }
            }

            // Quantize this chunk immediately (STREAMING: quantize then release residuals memory)
            u32 chunk_packed_dim = 0;
            auto chunk_packed = quantizer_->Quantize(chunk_residuals.get(), chunk_count, chunk_packed_dim);

            // Write new data after the old data
            std::copy_n(chunk_packed.get(), chunk_count * chunk_packed_dim, new_packed.get() + old_packed_size + chunk_start * packed_dim);
        }
    }

    // Update size and move new data to storage
    packed_residuals_size_ = old_packed_size + new_data_size;
    packed_residuals_capacity_ = packed_residuals_size_;
    packed_residuals_ = std::move(new_packed);

    // Store centroid assignments
    // Reserve space first to avoid multiple reallocations
    const size_t old_centroid_ids_size = centroid_ids_.size();
    centroid_ids_.reserve(old_centroid_ids_size + centroid_id_assignments.size());
    centroid_ids_.insert(centroid_ids_.end(), centroid_id_assignments.begin(), centroid_id_assignments.end());

    // Update IVF lists
    u64 offset = 0;
    for (u32 doc_idx = 0; doc_idx < doc_lens.size(); ++doc_idx) {
        u32 doc_id = old_doc_num + doc_idx;
        u32 len = doc_lens[doc_idx];
        for (u32 i = 0; i < len; ++i) {
            const u32 centroid_id = centroid_id_assignments[offset + i];
            if (ivf_lists_[centroid_id].empty() || ivf_lists_[centroid_id].back() != doc_id) {
                ivf_lists_[centroid_id].push_back(doc_id);
            }
        }
        offset += len;
    }

    // Update metadata
    u32 current_offset = old_total_embeddings;
    for (u32 len : doc_lens) {
        doc_lens_.push_back(len);
        doc_offsets_.push_back(current_offset);
        current_offset += len;
    }
    n_total_embeddings_ += total_embedding_num;
    n_docs_ += doc_lens.size();

    // IMPORTANT: Release centroid_id_assignments memory immediately
    // This vector can be large (4MB per 1M embeddings) and is no longer needed
    centroid_id_assignments.clear();
    centroid_id_assignments.shrink_to_fit();
}

void PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids(const f32 *embedding_data,
                                                        const std::vector<u32> &doc_lens,
                                                        const u32 *centroid_ids,
                                                        const u8 *packed_residuals,
                                                        const u32 packed_dim) {
    if (doc_lens.empty())
        return;

    std::unique_lock lock(rw_mutex_);

    const u32 old_doc_num = n_docs_;
    const u32 old_total_embeddings = n_total_embeddings_;

    // Calculate total embeddings
    u64 total_embedding_num = 0;
    for (u32 len : doc_lens)
        total_embedding_num += len;

    // Get expected packed dimension from quantizer
    // Use ceiling division to match PlaidQuantizer::packed_dim() computation
    const u32 expected_packed_dim = (embedding_dimension_ * nbits_ + 7) / 8;
    if (packed_dim != expected_packed_dim) {
        UnrecoverableError(fmt::format("PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: packed_dim mismatch. Expected {}, got {}",
                                       expected_packed_dim,
                                       packed_dim));
    }

    // Copy centroid IDs
    const size_t old_centroid_ids_size = centroid_ids_.size();
    centroid_ids_.reserve(old_centroid_ids_size + total_embedding_num);
    for (u64 i = 0; i < total_embedding_num; ++i) {
        centroid_ids_.push_back(centroid_ids[i]);
    }

    // Copy packed residuals
    const size_t new_data_size = total_embedding_num * packed_dim;
    const size_t old_packed_size = packed_residuals_size_;

    auto old_packed = std::move(packed_residuals_);
    packed_residuals_size_ = 0;

    auto new_packed = std::make_unique<u8[]>(old_packed_size + new_data_size);

    if (old_packed_size > 0 && old_packed) {
        std::copy_n(old_packed.get(), old_packed_size, new_packed.get());
        old_packed.reset();
    }

    std::copy_n(packed_residuals, new_data_size, new_packed.get() + old_packed_size);

    packed_residuals_size_ = old_packed_size + new_data_size;
    packed_residuals_capacity_ = packed_residuals_size_;
    packed_residuals_ = std::move(new_packed);

    // Update IVF lists
    u64 offset = 0;
    for (u32 doc_idx = 0; doc_idx < doc_lens.size(); ++doc_idx) {
        u32 doc_id = old_doc_num + doc_idx;
        u32 len = doc_lens[doc_idx];
        for (u32 i = 0; i < len; ++i) {
            const u32 centroid_id = centroid_ids[offset + i];
            if (ivf_lists_.empty() || centroid_id >= ivf_lists_.size()) {
                UnrecoverableError(fmt::format("PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: Invalid centroid_id {} (n_centroids={})",
                                               centroid_id,
                                               ivf_lists_.size()));
            }
            if (ivf_lists_[centroid_id].empty() || ivf_lists_[centroid_id].back() != doc_id) {
                ivf_lists_[centroid_id].push_back(doc_id);
            }
        }
        offset += len;
    }

    // Update metadata
    u32 current_offset = old_total_embeddings;
    for (u32 len : doc_lens) {
        doc_lens_.push_back(len);
        doc_offsets_.push_back(current_offset);
        current_offset += len;
    }
    n_total_embeddings_ += total_embedding_num;
    n_docs_ += doc_lens.size();

    LOG_TRACE(fmt::format("PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: Added {} docs ({} embeddings). Total docs: {}",
                          doc_lens.size(),
                          total_embedding_num,
                          n_docs_.load()));
}

void PlaidIndex::CopyCentroidsFrom(const std::vector<f32> &centroids_data,
                                   const std::vector<f32> &centroid_norms_neg_half,
                                   const u32 n_centroids,
                                   const PlaidQuantizer *quantizer,
                                   bool preserve_ivf_lists) {
    std::unique_lock lock(rw_mutex_);

    n_centroids_ = n_centroids;

    centroids_data_ = centroids_data;
    centroid_norms_neg_half_ = centroid_norms_neg_half;

    // Copy quantizer if provided
    if (quantizer != nullptr) {
        quantizer_->CopyFrom(*quantizer);
    }

    // Handle IVF lists based on preserve_ivf_lists flag
    if (preserve_ivf_lists) {
        // Preserve existing IVF lists (used after MergeOneChunk/FinalizeMerge)
        LOG_INFO(fmt::format("PlaidIndex::CopyCentroidsFrom: Copied {} centroids, preserved {} IVF lists", n_centroids, ivf_lists_.size()));
    } else {
        // Initialize/resize IVF lists (used when creating new index)
        ivf_lists_.clear();
        ivf_lists_.resize(n_centroids_);
        LOG_INFO(fmt::format("PlaidIndex::CopyCentroidsFrom: Copied {} centroids, initialized {} IVF lists", n_centroids, n_centroids_));
    }
}

void PlaidIndex::ShareGlobalCentroids(std::shared_ptr<PlaidGlobalCentroids> global_centroids) {
    std::unique_lock lock(rw_mutex_);

    if (!global_centroids || !global_centroids->IsTrained()) {
        UnrecoverableError("PlaidIndex::ShareGlobalCentroids: Global centroids not trained");
    }

    global_centroids_ref_ = std::move(global_centroids);
    n_centroids_ = global_centroids_ref_->n_centroids();

    // Copy quantizer from global centroids (still needed for dequantization)
    quantizer_->CopyFrom(*global_centroids_ref_->quantizer());

    // Resize IVF lists
    ivf_lists_.clear();
    ivf_lists_.resize(n_centroids_);

    // Clear local copies to save memory
    centroids_data_.clear();
    centroids_data_.shrink_to_fit();
    centroid_norms_neg_half_.clear();
    centroid_norms_neg_half_.shrink_to_fit();

    LOG_INFO(fmt::format("PlaidIndex::ShareGlobalCentroids: Shared {} centroids (no copy)", n_centroids_));
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

    // Decide whether to use batched mode for memory efficiency
    // Batched path: avoids allocating [n_query_tokens * n_centroids] score matrix
    const bool use_batched = n_centroids_ > centroid_batch_size_;

    if (use_batched) {
        return GetQueryResultBatched(query_ptr,
                                     query_embedding_num,
                                     n_ivf_probe,
                                     centroid_score_threshold,
                                     n_doc_to_score,
                                     n_full_scores,
                                     top_k,
                                     bitmask,
                                     start_segment_offset);
    }

    // === Standard path: compute full query-centroid scores upfront ===
    auto query_centroid_scores = ComputeQueryCentroidScores(query_ptr, query_embedding_num);

    // Step 1: IVF Probing - top-k per token, then apply threshold
    std::vector<std::vector<u32>> token_top_centroids(query_embedding_num);
    for (u32 i = 0; i < query_embedding_num; ++i) {
        const f32 *scores = query_centroid_scores.get() + i * n_centroids_;

        // Use partial_sort to find top n_ivf_probe centroids
        std::vector<std::pair<f32, u32>> centroid_scores;
        centroid_scores.reserve(n_centroids_);
        for (u32 j = 0; j < n_centroids_; ++j) {
            centroid_scores.emplace_back(scores[j], j);
        }
        const u32 n_select = std::min(n_ivf_probe, n_centroids_);
        std::partial_sort(centroid_scores.begin(), centroid_scores.begin() + n_select, centroid_scores.end(), std::greater<>());
        for (u32 j = 0; j < n_select; ++j) {
            token_top_centroids[i].push_back(centroid_scores[j].second);
        }
    }

    // Union top centroids across all query tokens, then apply threshold
    std::vector<bool> selected(n_centroids_, false);
    std::vector<u32> probed_centroids;
    for (const auto &centroids : token_top_centroids) {
        for (u32 cid : centroids) {
            if (!selected[cid]) {
                selected[cid] = true;
                probed_centroids.push_back(cid);
            }
        }
    }

    // Apply centroid score threshold: filter centroids where max score < threshold
    if (centroid_score_threshold > 0.0f) {
        probed_centroids.erase(std::remove_if(probed_centroids.begin(),
                                              probed_centroids.end(),
                                              [&](u32 cid) {
                                                  f32 max_score = std::numeric_limits<f32>::lowest();
                                                  for (u32 q = 0; q < query_embedding_num; ++q) {
                                                      max_score = std::max(max_score, query_centroid_scores[q * n_centroids_ + cid]);
                                                  }
                                                  return max_score < centroid_score_threshold;
                                              }),
                               probed_centroids.end());
    }

    // Step 2: Collect candidates with bitmask filtering
    const u32 n_docs = n_docs_.load();
    std::vector<bool> candidate_marker(n_docs, false);
    u32 candidate_count = 0;

    for (u32 cid : probed_centroids) {
        const u32 list_len = GetIVFListLength(cid);
        const u32 *list_data = GetIVFListData(cid);
        for (u32 i = 0; i < list_len; ++i) {
            u32 doc_id = list_data[i];
            if (!bitmask.IsTrue(doc_id))
                continue;
            if (!candidate_marker[doc_id]) {
                candidate_marker[doc_id] = true;
                ++candidate_count;
            }
        }
    }

    if (candidate_count == 0) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    // Collect marked doc_ids into a dense vector
    std::vector<u32> candidates;
    candidates.reserve(candidate_count);
    for (u32 doc_id = 0; doc_id < n_docs; ++doc_id) {
        if (candidate_marker[doc_id]) {
            candidates.push_back(doc_id);
        }
    }

    if (candidates.empty()) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    // Step 3-5: Scoring and reranking
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

PlaidQueryResultType PlaidIndex::GetQueryResultBatched(const f32 *query_ptr,
                                                       const u32 query_embedding_num,
                                                       const u32 n_ivf_probe,
                                                       const f32 centroid_score_threshold,
                                                       const u32 n_doc_to_score,
                                                       const u32 n_full_scores,
                                                       const u32 top_k,
                                                       Bitmask &bitmask,
                                                       const u32 start_segment_offset) const {
    // Batched IVF Probing: memory-efficient path for large n_centroids
    std::vector<u32> probed_centroids;
    std::unique_ptr<f32[]> sparse_centroid_scores;
    u32 n_sparse_centroids = 0;

    BatchedIVFProbe(query_ptr,
                    query_embedding_num,
                    n_ivf_probe,
                    centroid_score_threshold,
                    centroid_batch_size_,
                    probed_centroids,
                    sparse_centroid_scores,
                    n_sparse_centroids);

    if (probed_centroids.empty()) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    // Build centroid_id -> sparse_index mapping for fast lookup
    std::vector<u32> sparse_centroid_id_map(n_centroids_, std::numeric_limits<u32>::max());
    for (u32 i = 0; i < n_sparse_centroids; ++i) {
        sparse_centroid_id_map[probed_centroids[i]] = i;
    }

    // Collect candidates with bitmask filtering
    const u32 n_docs = n_docs_.load();
    std::vector<bool> candidate_marker(n_docs, false);
    u32 candidate_count = 0;

    for (u32 cid : probed_centroids) {
        const u32 list_len = GetIVFListLength(cid);
        const u32 *list_data = GetIVFListData(cid);
        for (u32 i = 0; i < list_len; ++i) {
            u32 doc_id = list_data[i];
            if (!bitmask.IsTrue(doc_id))
                continue;
            if (!candidate_marker[doc_id]) {
                candidate_marker[doc_id] = true;
                ++candidate_count;
            }
        }
    }

    if (candidate_count == 0) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    std::vector<u32> candidates;
    candidates.reserve(candidate_count);
    for (u32 doc_id = 0; doc_id < n_docs; ++doc_id) {
        if (candidate_marker[doc_id]) {
            candidates.push_back(doc_id);
        }
    }

    if (candidates.empty()) {
        return std::make_tuple(0, nullptr, nullptr);
    }

    // Approximate scoring using sparse centroid scores
    std::vector<std::pair<f32, u32>> doc_scores;
    doc_scores.reserve(candidates.size());
    for (u32 doc_id : candidates) {
        f32 score = ApproximateScoreSparse(&centroid_ids_[doc_offsets_[doc_id]],
                                           doc_lens_[doc_id],
                                           sparse_centroid_scores.get(),
                                           sparse_centroid_id_map.data(),
                                           n_sparse_centroids,
                                           query_embedding_num);
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
    // Use accessor to get centroids data (handles shared global centroids)
    const auto &centroids = centroids_data();
    const auto &norms = centroid_norms_neg_half();
    matrixA_multiply_transpose_matrixB_output_to_C(query_ptr, centroids.data(), n_query_tokens, n_centroids_, embedding_dimension_, scores.get());

    // Add centroid_norms_neg_half for L2 distance computation
    for (u32 i = 0; i < n_query_tokens; ++i) {
        for (u32 j = 0; j < n_centroids_; ++j) {
            scores[i * n_centroids_ + j] += norms[j];
        }
    }

    return scores;
}

u32 PlaidIndex::ComputeAutoNCentroids(const u64 embedding_count) {
    // Auto: k = 2^floor(log2(16 * sqrt(N)))
    // Matches next-plaid/fast-plaid heuristic for finer IVF partitioning
    if (embedding_count < 256) {
        return 8;
    }
    double sqrt_n = std::sqrt(static_cast<double>(embedding_count));
    double log2_val = std::log2(16.0 * sqrt_n);
    u32 n_centroids;
    if (log2_val < 3.0) {
        n_centroids = 8;
    } else {
        n_centroids = 1u << static_cast<u32>(std::floor(log2_val));
    }
    // Cap at total embedding count
    n_centroids = std::min(n_centroids, static_cast<u32>(embedding_count));
    // Downgrade if not enough data for K-means (need at least 32 * n_centroids)
    while (n_centroids > 8 && embedding_count < 32u * n_centroids) {
        n_centroids >>= 1;
    }
    // Ensure n_centroids is a multiple of 8 for SIMD alignment
    n_centroids = ((n_centroids + 7) / 8) * 8;
    n_centroids = std::max(8u, n_centroids);
    return n_centroids;
}

void PlaidIndex::BatchedIVFProbe(const f32 *query_ptr,
                                 const u32 n_query_tokens,
                                 const u32 n_ivf_probe,
                                 const f32 centroid_score_threshold,
                                 const u32 centroid_batch_size,
                                 std::vector<u32> &probed_centroids,
                                 std::unique_ptr<f32[]> &sparse_centroid_scores,
                                 u32 &n_sparse_centroids) const {
    // Process centroids in batches to bound memory usage
    // Each batch computes [n_query_tokens, batch_size] score matrix
    // Per-token top-k heaps are maintained across batches
    const auto &centroids = centroids_data();
    const auto &norms = centroid_norms_neg_half();
    const u32 batch_size = (centroid_batch_size > 0) ? centroid_batch_size : DEFAULT_CENTROID_BATCH_SIZE;

    // Per-token top-k min-heaps: store (score, centroid_id)
    // Using vectors of pairs sorted as min-heaps (smallest at front)
    std::vector<std::vector<std::pair<f32, u32>>> token_heaps(n_query_tokens);
    for (auto &heap : token_heaps) {
        heap.reserve(n_ivf_probe + 1);
    }

    // Track max score per centroid across all query tokens (for threshold filtering)
    std::vector<f32> centroid_max_score(n_centroids_, std::numeric_limits<f32>::lowest());

    // Allocate batch score buffer
    auto batch_scores = std::make_unique<f32[]>(n_query_tokens * batch_size);

    for (u32 batch_start = 0; batch_start < n_centroids_; batch_start += batch_size) {
        const u32 batch_end = std::min(batch_start + batch_size, n_centroids_);
        const u32 current_batch_size = batch_end - batch_start;

        // Compute scores for this batch: [n_query_tokens, current_batch_size]
        const f32 *batch_centroids = centroids.data() + batch_start * embedding_dimension_;
        matrixA_multiply_transpose_matrixB_output_to_C(query_ptr,
                                                       batch_centroids,
                                                       n_query_tokens,
                                                       current_batch_size,
                                                       embedding_dimension_,
                                                       batch_scores.get());

        // Add centroid_norms_neg_half for L2 distance computation
        for (u32 i = 0; i < n_query_tokens; ++i) {
            for (u32 j = 0; j < current_batch_size; ++j) {
                batch_scores[i * current_batch_size + j] += norms[batch_start + j];
            }
        }

        // Update per-token top-k heaps
        for (u32 q = 0; q < n_query_tokens; ++q) {
            auto &heap = token_heaps[q];
            for (u32 j = 0; j < current_batch_size; ++j) {
                const f32 score = batch_scores[q * current_batch_size + j];
                const u32 global_cid = batch_start + j;

                // Track max score for this centroid
                centroid_max_score[global_cid] = std::max(centroid_max_score[global_cid], score);

                if (heap.size() < n_ivf_probe) {
                    // Heap not full yet, just push
                    heap.emplace_back(score, global_cid);
                    std::push_heap(heap.begin(), heap.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
                } else if (score > heap.front().first) {
                    // Score is better than worst in heap, replace
                    std::pop_heap(heap.begin(), heap.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
                    heap.back() = {score, global_cid};
                    std::push_heap(heap.begin(), heap.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
                }
            }
        }
    }

    // Collect union of top centroids across all query tokens
    std::vector<bool> selected(n_centroids_, false);
    probed_centroids.clear();
    for (const auto &heap : token_heaps) {
        for (const auto &[score, cid] : heap) {
            if (!selected[cid]) {
                selected[cid] = true;
                probed_centroids.push_back(cid);
            }
        }
    }

    // Apply centroid score threshold: filter out centroids where max score < threshold
    if (centroid_score_threshold > 0.0f) {
        probed_centroids.erase(std::remove_if(probed_centroids.begin(),
                                              probed_centroids.end(),
                                              [&](u32 cid) { return centroid_max_score[cid] < centroid_score_threshold; }),
                               probed_centroids.end());
    }

    // Build sparse centroid scores: only for probed centroids
    // Layout: [n_sparse_centroids, n_query_tokens]
    // Also build a mapping from centroid_id -> sparse_index
    n_sparse_centroids = probed_centroids.size();
    if (n_sparse_centroids == 0) {
        sparse_centroid_scores.reset();
        return;
    }

    sparse_centroid_scores = std::make_unique<f32[]>(n_sparse_centroids * n_query_tokens);

    // Compute sparse scores by re-computing only for probed centroids
    // This is more memory-efficient than storing the full [n_query_tokens, n_centroids] matrix
    // Process in small batches for cache efficiency
    constexpr u32 SPARSE_BATCH = 256;
    auto sparse_batch_scores = std::make_unique<f32[]>(n_query_tokens * SPARSE_BATCH);

    for (u32 batch_start = 0; batch_start < n_sparse_centroids; batch_start += SPARSE_BATCH) {
        const u32 batch_end = std::min(batch_start + SPARSE_BATCH, n_sparse_centroids);
        const u32 current_batch = batch_end - batch_start;

        // Gather centroid vectors for this batch
        auto batch_centroid_data = std::make_unique<f32[]>(current_batch * embedding_dimension_);
        for (u32 i = 0; i < current_batch; ++i) {
            const u32 cid = probed_centroids[batch_start + i];
            std::copy_n(centroids.data() + cid * embedding_dimension_, embedding_dimension_, batch_centroid_data.get() + i * embedding_dimension_);
        }

        // Compute [n_query_tokens, current_batch]
        matrixA_multiply_transpose_matrixB_output_to_C(query_ptr,
                                                       batch_centroid_data.get(),
                                                       n_query_tokens,
                                                       current_batch,
                                                       embedding_dimension_,
                                                       sparse_batch_scores.get());

        // Add norms and store
        for (u32 q = 0; q < n_query_tokens; ++q) {
            for (u32 i = 0; i < current_batch; ++i) {
                const u32 cid = probed_centroids[batch_start + i];
                sparse_centroid_scores[(batch_start + i) * n_query_tokens + q] = sparse_batch_scores[q * current_batch + i] + norms[cid];
            }
        }
    }
}

f32 PlaidIndex::ApproximateScoreSparse(const u32 *doc_centroid_ids,
                                       const u32 doc_len,
                                       const f32 *sparse_scores,
                                       const u32 *sparse_centroid_id_map,
                                       const u32 n_sparse_centroids,
                                       const u32 n_query_tokens) const {
    // sparse_scores layout: [n_sparse_centroids, n_query_tokens]
    // sparse_centroid_id_map maps centroid_id -> sparse index (or UINT32_MAX if not in sparse set)
    // Track max score per query token
    auto max_scores = std::make_unique<f32[]>(n_query_tokens);
    std::fill_n(max_scores.get(), n_query_tokens, std::numeric_limits<f32>::lowest());

    for (u32 d = 0; d < doc_len; ++d) {
        const u32 cid = doc_centroid_ids[d];
        if (cid >= n_centroids_) {
            continue;
        }
        const u32 sparse_idx = sparse_centroid_id_map[cid];
        if (sparse_idx == std::numeric_limits<u32>::max() || sparse_idx >= n_sparse_centroids) {
            continue;
        }
        const f32 *score_row = sparse_scores + sparse_idx * n_query_tokens;
        for (u32 q = 0; q < n_query_tokens; ++q) {
            max_scores[q] = std::max(max_scores[q], score_row[q]);
        }
    }

    // Sum max scores across query tokens
    f32 total_score = 0.0f;
    for (u32 q = 0; q < n_query_tokens; ++q) {
        if (max_scores[q] > std::numeric_limits<f32>::lowest()) {
            total_score += max_scores[q];
        }
    }
    return total_score;
}

f32 PlaidIndex::ApproximateScore(const u32 *doc_centroid_ids, u32 doc_len, const f32 *query_centroid_scores, u32 n_query_tokens) const {
    // Track max score per query token, then sum at the end.
    // Changed from original outer-loop-over-queries to outer-loop-over-doc-tokens:
    // For each doc token's centroid, update all query max-scores at once.
    // This improves cache locality: for a given centroid cid, all query scores
    // are at contiguous stride n_centroids_ in query_centroid_scores.

    auto max_scores = std::make_unique<f32[]>(n_query_tokens);
    std::fill_n(max_scores.get(), n_query_tokens, std::numeric_limits<f32>::lowest());

    // For each doc token, look up all query scores for its centroid in one pass
    // query_centroid_scores layout: [n_query_tokens * n_centroids]
    // For centroid cid, scores for all queries are at: cid, n_centroids+cid, 2*n_centroids+cid, ...
    for (u32 d = 0; d < doc_len; ++d) {
        const u32 cid = doc_centroid_ids[d];
        const f32 *score_ptr = query_centroid_scores + cid;
        for (u32 q = 0; q < n_query_tokens; ++q) {
            max_scores[q] = std::max(max_scores[q], *score_ptr);
            score_ptr += n_centroids_;
        }
    }

    // Sum max scores across query tokens
    f32 total_score = 0.0f;
    for (u32 q = 0; q < n_query_tokens; ++q) {
        if (max_scores[q] > std::numeric_limits<f32>::lowest()) {
            total_score += max_scores[q];
        }
    }

    return total_score;
}

f32 PlaidIndex::ExactScore(const f32 *query_ptr, u32 n_query_tokens, u32 doc_id, const f32 *centroid_distances) const {
    f32 total_score = 0.0f;
    u32 doc_offset = doc_offsets_[doc_id];
    u32 doc_len = doc_lens_[doc_id];

    // Use accessor to get centroids data (handles shared global centroids)
    const auto &centroids = centroids_data();

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
                const f32 *centroid_vec = centroids.data() + cid * embedding_dimension_;
                centroid_score = IPDistance<f32>(query_vec, centroid_vec, embedding_dimension_);
            }

            // Refined score with residual
            f32 residual_score = quantizer_->GetSingleIPDistance(embedding_idx,
                                                                 q,
                                                                 n_query_tokens,
                                                                 ip_table.get(),
                                                                 packed_residuals_.get(),
                                                                 centroid_ids_.data(),
                                                                 centroids.data());

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

    // Centroids - use accessor to get centroids data (handles shared global centroids)
    const auto &centroids = centroids_data();
    const auto &norms = centroid_norms_neg_half();
    file_handle.Append(centroids.data(), centroids.size() * sizeof(f32));
    file_handle.Append(norms.data(), norms.size() * sizeof(f32));

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

    // IVF lists - support both nested and flattened formats
    if (ivf_flattened_) {
        for (u32 i = 0; i < n_centroids_; ++i) {
            u32 list_size = ivf_lengths_[i];
            file_handle.Append(&list_size, sizeof(list_size));
            if (list_size > 0) {
                file_handle.Append(ivf_data_.data() + ivf_offsets_[i], list_size * sizeof(u32));
            }
        }
    } else {
        for (u32 i = 0; i < n_centroids_; ++i) {
            u32 list_size = ivf_lists_[i].size();
            file_handle.Append(&list_size, sizeof(list_size));
            file_handle.Append(ivf_lists_[i].data(), list_size * sizeof(u32));
        }
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
    packed_residuals_capacity_ = packed_residuals_size_;
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

    // Use accessors to get centroids data (handles shared global centroids)
    size += centroids_data().size() * sizeof(f32);
    size += centroid_norms_neg_half().size() * sizeof(f32);

    size += sizeof(u32) + doc_lens_.size() * sizeof(u32);
    size += sizeof(u32) + doc_offsets_.size() * sizeof(u32);
    size += sizeof(u32) + centroid_ids_.size() * sizeof(u32);

    if (ivf_flattened_) {
        for (u32 i = 0; i < n_centroids_; ++i) {
            size += sizeof(u32) + ivf_lengths_[i] * sizeof(u32);
        }
    } else {
        for (const auto &list : ivf_lists_) {
            size += sizeof(u32) + list.size() * sizeof(u32);
        }
    }

    size += sizeof(u32) + packed_residuals_size_;

    // Quantizer size (now includes nbits_ and embedding_dim_ to match SaveToPtr format)
    size += sizeof(u32);                                               // nbits_
    size += sizeof(u32);                                               // embedding_dim_
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

    // Use accessors to get centroids data (handles shared global centroids)
    const auto &centroids = centroids_data();
    const auto &centroid_norms = centroid_norms_neg_half();
    append(centroids.data(), centroids.size() * sizeof(f32));
    append(centroid_norms.data(), centroid_norms.size() * sizeof(f32));

    u32 doc_lens_size = doc_lens_.size();
    append(&doc_lens_size, sizeof(doc_lens_size));
    append(doc_lens_.data(), doc_lens_.size() * sizeof(u32));

    u32 doc_offsets_size = doc_offsets_.size();
    append(&doc_offsets_size, sizeof(doc_offsets_size));
    append(doc_offsets_.data(), doc_offsets_.size() * sizeof(u32));

    u32 centroid_ids_size = centroid_ids_.size();
    append(&centroid_ids_size, sizeof(centroid_ids_size));
    append(centroid_ids_.data(), centroid_ids_.size() * sizeof(u32));

    if (ivf_flattened_) {
        for (u32 i = 0; i < n_centroids_; ++i) {
            u32 list_size = ivf_lengths_[i];
            append(&list_size, sizeof(list_size));
            if (list_size > 0) {
                append(ivf_data_.data() + ivf_offsets_[i], list_size * sizeof(u32));
            }
        }
    } else {
        for (const auto &list : ivf_lists_) {
            u32 list_size = list.size();
            append(&list_size, sizeof(list_size));
            append(list.data(), list.size() * sizeof(u32));
        }
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
    packed_residuals_capacity_ = packed_size;
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

void PlaidIndex::MergeChunks(const std::vector<std::pair<u32, const PlaidIndex *>> &chunks_with_doc_offsets) {
    std::unique_lock lock(rw_mutex_);

    if (chunks_with_doc_offsets.empty()) {
        return;
    }

    const u32 packed_dim = (embedding_dimension_ * nbits_ + 7) / 8;

    // Step 1: Calculate total docs and embeddings
    u32 total_docs = 0;
    u64 total_embeddings = 0;
    for (const auto &[doc_offset, chunk] : chunks_with_doc_offsets) {
        total_docs += chunk->GetDocNum();
        total_embeddings += chunk->GetTotalEmbeddingNum();
    }

    LOG_INFO(fmt::format("PlaidIndex::MergeChunks: Merging {} chunks, {} docs, {} embeddings",
                         chunks_with_doc_offsets.size(),
                         total_docs,
                         total_embeddings));

    // Step 2: Allocate merged data structures
    std::vector<u32> merged_doc_lens;
    std::vector<u32> merged_doc_offsets;
    std::vector<u32> merged_centroid_ids;
    auto merged_packed = std::make_unique<u8[]>(total_embeddings * packed_dim);
    size_t merged_packed_offset = 0;

    merged_doc_lens.reserve(total_docs);
    merged_doc_offsets.reserve(total_docs);
    merged_centroid_ids.reserve(total_embeddings);

    // Step 3: Merge data from each chunk
    u64 current_embedding_offset = 0;

    for (const auto &[doc_offset, chunk] : chunks_with_doc_offsets) {
        const u32 chunk_doc_num = chunk->GetDocNum();
        const u64 chunk_embedding_num = chunk->GetTotalEmbeddingNum();

        // Merge doc_lens and doc_offsets (adjust offsets)
        const auto &chunk_doc_lens = chunk->doc_lens();
        const auto &chunk_doc_offsets = chunk->doc_offsets();

        for (u32 i = 0; i < chunk_doc_num; ++i) {
            merged_doc_lens.push_back(chunk_doc_lens[i]);
            merged_doc_offsets.push_back(current_embedding_offset + chunk_doc_offsets[i]);
        }

        // Merge centroid_ids
        const auto &chunk_centroid_ids = chunk->centroid_ids();
        merged_centroid_ids.insert(merged_centroid_ids.end(), chunk_centroid_ids.begin(), chunk_centroid_ids.end());

        // Merge packed_residuals
        const u8 *chunk_residuals = chunk->packed_residuals();
        size_t chunk_residuals_size = chunk->packed_residuals_size();
        if (chunk_residuals && chunk_residuals_size > 0) {
            std::copy_n(chunk_residuals, chunk_residuals_size, merged_packed.get() + merged_packed_offset);
            merged_packed_offset += chunk_residuals_size;
        }

        current_embedding_offset += chunk_embedding_num;
    }

    // Step 4: Rebuild IVF lists (this is necessary after merging)
    std::vector<std::vector<u32>> merged_ivf_lists(n_centroids_);
    for (u32 doc_id = 0; doc_id < total_docs; ++doc_id) {
        u32 doc_offset = merged_doc_offsets[doc_id];
        u32 doc_len = merged_doc_lens[doc_id];
        for (u32 i = 0; i < doc_len; ++i) {
            u32 centroid_id = merged_centroid_ids[doc_offset + i];
            if (merged_ivf_lists[centroid_id].empty() || merged_ivf_lists[centroid_id].back() != doc_id) {
                merged_ivf_lists[centroid_id].push_back(doc_id);
            }
        }
    }

    // Step 5: Update this index with merged data
    n_docs_ = total_docs;
    n_total_embeddings_ = total_embeddings;
    doc_lens_ = std::move(merged_doc_lens);
    doc_offsets_ = std::move(merged_doc_offsets);
    centroid_ids_ = std::move(merged_centroid_ids);
    packed_residuals_ = std::move(merged_packed);
    packed_residuals_size_ = total_embeddings * packed_dim;
    packed_residuals_capacity_ = packed_residuals_size_;
    ivf_lists_ = std::move(merged_ivf_lists);

    LOG_INFO(fmt::format("PlaidIndex::MergeChunks: Merged successfully. Total docs: {}, embeddings: {}, IVF entries: {}",
                         n_docs_.load(),
                         n_total_embeddings_,
                         std::accumulate(ivf_lists_.begin(), ivf_lists_.end(), 0u, [](u32 sum, const auto &list) { return sum + list.size(); })));
}

// Streaming merge support - allows merging one chunk at a time to control memory
// This is crucial for large indexes that exceed available RAM

void PlaidIndex::InitializeMerge(u32 n_centroids) {
    std::unique_lock lock(rw_mutex_);

    doc_lens_.clear();
    doc_offsets_.clear();
    centroid_ids_.clear();
    packed_residuals_.reset();
    packed_residuals_size_ = 0;
    packed_residuals_capacity_ = 0;

    n_centroids_ = n_centroids;
    ivf_lists_.clear();
    ivf_lists_.resize(n_centroids_);

    n_docs_ = 0;
    n_total_embeddings_ = 0;
}

void PlaidIndex::MergeOneChunk(const PlaidIndex *chunk, u32 doc_offset) {
    std::unique_lock lock(rw_mutex_);

    if (!chunk) {
        return;
    }

    const u32 chunk_doc_num = chunk->GetDocNum();
    const u64 chunk_embedding_num = chunk->GetTotalEmbeddingNum();

    // Get current state
    u32 current_docs = n_docs_.load();
    u64 current_embeddings = n_total_embeddings_;

    // Extend doc_lens_ and doc_offsets_
    const auto &chunk_doc_lens = chunk->doc_lens();
    const auto &chunk_doc_offsets = chunk->doc_offsets();

    for (u32 i = 0; i < chunk_doc_num; ++i) {
        doc_lens_.push_back(chunk_doc_lens[i]);
        doc_offsets_.push_back(current_embeddings + chunk_doc_offsets[i]);
    }

    // Extend centroid_ids_
    const auto &chunk_centroid_ids = chunk->centroid_ids();
    centroid_ids_.insert(centroid_ids_.end(), chunk_centroid_ids.begin(), chunk_centroid_ids.end());

    // Extend packed_residuals_ with amortized O(1) reallocation (2x growth strategy)
    const u8 *chunk_residuals = chunk->packed_residuals();
    size_t chunk_residuals_size = chunk->packed_residuals_size();
    if (chunk_residuals && chunk_residuals_size > 0) {
        const size_t new_size = packed_residuals_size_ + chunk_residuals_size;

        if (new_size <= packed_residuals_capacity_) {
            // Existing buffer has enough capacity — just append in place
            std::copy_n(chunk_residuals, chunk_residuals_size, packed_residuals_.get() + packed_residuals_size_);
        } else if (packed_residuals_ && packed_residuals_size_ > 0) {
            // Need to grow: allocate max(new_size, capacity * 2) to amortize reallocation
            const size_t alloc_size = std::max(new_size, packed_residuals_capacity_ * 2);
            auto new_packed = std::make_unique<u8[]>(alloc_size);
            std::copy_n(packed_residuals_.get(), packed_residuals_size_, new_packed.get());
            std::copy_n(chunk_residuals, chunk_residuals_size, new_packed.get() + packed_residuals_size_);
            packed_residuals_ = std::move(new_packed);
            packed_residuals_capacity_ = alloc_size;
        } else {
            // First allocation
            packed_residuals_ = std::make_unique<u8[]>(new_size);
            std::copy_n(chunk_residuals, chunk_residuals_size, packed_residuals_.get());
            packed_residuals_capacity_ = new_size;
        }
        packed_residuals_size_ = new_size;
    }

    // Update IVF lists for this chunk
    // doc_offset is the starting doc_id in the merged index for this chunk
    u32 max_doc_id_added = 0;
    for (u32 i = 0; i < chunk_doc_num; ++i) {
        u32 global_doc_id = doc_offset + i; // Use doc_offset to calculate correct segment-relative doc_id
        max_doc_id_added = std::max(max_doc_id_added, global_doc_id);
        u32 doc_len = chunk_doc_lens[i];
        u32 local_doc_offset = chunk_doc_offsets[i];
        for (u32 j = 0; j < doc_len; ++j) {
            u32 centroid_id = chunk_centroid_ids[local_doc_offset + j];
            // Safety check: ensure centroid_id is within bounds
            if (centroid_id >= n_centroids_) {
                LOG_ERROR(fmt::format("PlaidIndex::MergeOneChunk: centroid_id {} out of bounds (n_centroids_={})", centroid_id, n_centroids_));
                continue;
            }
            if (ivf_lists_[centroid_id].empty() || ivf_lists_[centroid_id].back() != global_doc_id) {
                ivf_lists_[centroid_id].push_back(global_doc_id);
            }
        }
    }

    n_docs_ = current_docs + chunk_doc_num;
    n_total_embeddings_ = current_embeddings + chunk_embedding_num;
}

void PlaidIndex::FinalizeMerge() {
    // No-op: counters are already updated in MergeOneChunk
}

void PlaidIndex::FinalizeIVF() {
    std::unique_lock lock(rw_mutex_);

    if (ivf_flattened_) {
        return; // Already flattened
    }

    if (ivf_lists_.empty()) {
        return; // Nothing to flatten
    }

    // Calculate total entries
    u64 total_entries = 0;
    for (const auto &list : ivf_lists_) {
        total_entries += list.size();
    }

    // Allocate flattened storage
    ivf_data_.resize(total_entries);
    ivf_offsets_.resize(n_centroids_);
    ivf_lengths_.resize(n_centroids_);

    // Copy data
    u64 offset = 0;
    for (u32 i = 0; i < n_centroids_; ++i) {
        ivf_offsets_[i] = static_cast<u32>(offset);
        ivf_lengths_[i] = static_cast<u32>(ivf_lists_[i].size());
        if (!ivf_lists_[i].empty()) {
            std::copy_n(ivf_lists_[i].data(), ivf_lengths_[i], ivf_data_.data() + offset);
        }
        offset += ivf_lengths_[i];
    }

    // Release nested vectors
    ivf_lists_.clear();
    ivf_lists_.shrink_to_fit();

    ivf_flattened_ = true;

    LOG_INFO(fmt::format("PlaidIndex::FinalizeIVF: Flattened {} IVF lists into {} entries", n_centroids_, total_entries));
}

const u32 *PlaidIndex::GetIVFListData(const u32 centroid_id) const {
    if (ivf_flattened_) {
        if (centroid_id >= n_centroids_ || ivf_lengths_[centroid_id] == 0) {
            return nullptr;
        }
        return ivf_data_.data() + ivf_offsets_[centroid_id];
    }
    // Legacy path
    if (centroid_id >= ivf_lists_.size() || ivf_lists_[centroid_id].empty()) {
        return nullptr;
    }
    return ivf_lists_[centroid_id].data();
}

u32 PlaidIndex::GetIVFListLength(const u32 centroid_id) const {
    if (ivf_flattened_) {
        if (centroid_id >= n_centroids_) {
            return 0;
        }
        return ivf_lengths_[centroid_id];
    }
    // Legacy path
    if (centroid_id >= ivf_lists_.size()) {
        return 0;
    }
    return static_cast<u32>(ivf_lists_[centroid_id].size());
}

std::vector<u64> PlaidIndex::FindOutliers(const f32 *embeddings, const u64 n_embeddings, const f32 threshold_sq) const {
    // Find embeddings whose minimum L2² distance to any centroid exceeds threshold_sq
    // Uses tiled blocked processing to avoid O(n*k) memory allocation
    const auto &centroids = centroids_data();
    const auto &norms = centroid_norms_neg_half();

    std::vector<u64> outlier_indices;

    // Process in chunks to limit memory usage
    constexpr u32 CHUNK_SIZE = 16384;
    auto dist_table_chunk = std::make_unique<f32[]>(CHUNK_SIZE * n_centroids_);

    for (u64 chunk_start = 0; chunk_start < n_embeddings; chunk_start += CHUNK_SIZE) {
        const u32 chunk_end = std::min(static_cast<u64>(chunk_start + CHUNK_SIZE), n_embeddings);
        const u32 chunk_count = chunk_end - chunk_start;

        // Compute distances for this chunk
        matrixA_multiply_transpose_matrixB_output_to_C(embeddings + chunk_start * embedding_dimension_,
                                                       centroids.data(),
                                                       chunk_count,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table_chunk.get());

        for (u32 i = 0; i < chunk_count; ++i) {
            const f32 *dist_ptr = dist_table_chunk.get() + i * n_centroids_;

            // Find minimum L2² distance to any centroid
            // neg_distance = (query · centroid) - 0.5 * ||centroid||²
            // L2² = ||query||² - 2 * neg_distance
            // We only need to find max neg_distance (min L2²)
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            for (u32 k = 0; k < n_centroids_; ++k) {
                const f32 neg_distance = dist_ptr[k] + norms[k];
                max_neg_distance = std::max(max_neg_distance, neg_distance);
            }

            // Compute min L2²: ||embedding||² - 2 * max_neg_distance
            // We can avoid computing ||embedding||² by using a relative threshold:
            // Since neg_distance = (q·c) - 0.5*||c||² and L2² = ||q-c||² = ||q||² - 2*(q·c) + ||c||²
            // = ||q||² - 2*max_neg_distance - ||c||² + 2*0.5*||c||² ... actually:
            // neg_distance = q·c - 0.5*||c||², so -2*neg_distance = -2*q·c + ||c||²
            // L2² = ||q||² - 2*q·c + ||c||² = ||q||² - 2*neg_distance - ||c||²
            // Hmm, this is getting complex. Let's compute ||embedding||² and L2² directly.
            const f32 *emb = embeddings + (chunk_start + i) * embedding_dimension_;
            f32 emb_norm_sq = 0.0f;
            for (u32 d = 0; d < embedding_dimension_; ++d) {
                emb_norm_sq += emb[d] * emb[d];
            }
            f32 min_l2_sq = emb_norm_sq - 2.0f * max_neg_distance;

            if (min_l2_sq > threshold_sq) {
                outlier_indices.push_back(chunk_start + i);
            }
        }
    }

    return outlier_indices;
}

u32 PlaidIndex::ExpandCentroids(const f32 *new_embeddings, const u64 n_new_embeddings, const f32 cluster_threshold) {
    std::unique_lock lock(rw_mutex_);

    if (n_new_embeddings == 0 || n_centroids_ == 0) {
        return 0;
    }

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Checking {} new embeddings for outliers (threshold={})", n_new_embeddings, cluster_threshold));

    // Step 1: Find outlier embeddings
    const f32 threshold_sq = cluster_threshold * cluster_threshold;
    auto outlier_indices = FindOutliers(new_embeddings, n_new_embeddings, threshold_sq);

    const u64 num_outliers = outlier_indices.size();
    if (num_outliers == 0) {
        LOG_INFO("PlaidIndex::ExpandCentroids: No outliers found, no expansion needed");
        return 0;
    }

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Found {} outlier embeddings", num_outliers));

    // Step 2: Compute number of new centroids
    // k_update = max(1, ceil(num_outliers / max_points_per_centroid)) * 4
    // Following next-plaid's formula
    constexpr u32 MAX_POINTS_PER_CENTROID = 256;
    u32 target_k = std::max(1u, static_cast<u32>(std::ceil(static_cast<double>(num_outliers) / MAX_POINTS_PER_CENTROID))) * 4;
    u32 k_update = std::min(target_k, static_cast<u32>(num_outliers));
    // Round up to multiple of 8
    k_update = ((k_update + 7) / 8) * 8;
    k_update = std::max(8u, k_update);

    if (k_update > num_outliers) {
        k_update = ((static_cast<u32>(num_outliers) / 8) * 8);
        if (k_update == 0) {
            k_update = 8;
        }
    }

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Will add {} new centroids from {} outliers", k_update, num_outliers));

    // Step 3: Extract outlier embeddings
    auto outlier_data = std::make_unique<f32[]>(num_outliers * embedding_dimension_);
    for (u64 i = 0; i < num_outliers; ++i) {
        std::copy_n(new_embeddings + outlier_indices[i] * embedding_dimension_, embedding_dimension_, outlier_data.get() + i * embedding_dimension_);
    }

    // Step 4: Cluster outliers to get new centroids
    std::vector<f32> new_centroids_data(k_update * embedding_dimension_);
    const auto result_k = GetKMeansCentroids(MetricType::kMetricL2,
                                             embedding_dimension_,
                                             num_outliers,
                                             outlier_data.get(),
                                             new_centroids_data,
                                             k_update,
                                             4); // 4 iterations

    if (result_k != k_update) {
        LOG_WARN(fmt::format("PlaidIndex::ExpandCentroids: KMeans returned {} centroids, requested {}", result_k, k_update));
        if (result_k == 0) {
            return 0;
        }
        // Adjust to actual result
        k_update = result_k;
        // Round to multiple of 8
        k_update = ((k_update + 7) / 8) * 8;
        if (k_update == 0) {
            return 0;
        }
    }

    // Step 5: Append new centroids
    // If using shared global centroids, we need to copy them locally first
    if (global_centroids_ref_) {
        centroids_data_ = global_centroids_ref_->centroids_data();
        centroid_norms_neg_half_ = global_centroids_ref_->centroid_norms_neg_half();
        quantizer_->CopyFrom(*global_centroids_ref_->quantizer());
        global_centroids_ref_.reset();
    }

    const u32 old_n_centroids = n_centroids_;
    const u32 new_n_centroids = old_n_centroids + k_update;

    // Append centroid data
    centroids_data_.resize(new_n_centroids * embedding_dimension_);
    std::copy_n(new_centroids_data.data(), k_update * embedding_dimension_, centroids_data_.data() + old_n_centroids * embedding_dimension_);

    // Compute norms for new centroids and append
    centroid_norms_neg_half_.resize(new_n_centroids);
    for (u32 i = 0; i < k_update; ++i) {
        const f32 *centroid_vec = centroids_data_.data() + (old_n_centroids + i) * embedding_dimension_;
        centroid_norms_neg_half_[old_n_centroids + i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_vec, embedding_dimension_);
    }

    // Append empty IVF lists for new centroids
    if (ivf_flattened_) {
        // Need to un-flatten to add new centroids
        // Reconstruct ivf_lists_ from flattened data
        ivf_lists_.resize(old_n_centroids);
        for (u32 i = 0; i < old_n_centroids; ++i) {
            const u32 len = ivf_lengths_[i];
            ivf_lists_[i].resize(len);
            if (len > 0) {
                std::copy_n(ivf_data_.data() + ivf_offsets_[i], len, ivf_lists_[i].data());
            }
        }
        ivf_data_.clear();
        ivf_offsets_.clear();
        ivf_lengths_.clear();
        ivf_flattened_ = false;
    }

    ivf_lists_.resize(new_n_centroids); // New lists are empty

    n_centroids_ = new_n_centroids;

    LOG_INFO(fmt::format("PlaidIndex::ExpandCentroids: Expanded from {} to {} centroids", old_n_centroids, new_n_centroids));

    return k_update;
}

void PlaidIndex::AcceptMergedData(std::vector<u32> &&doc_lens,
                                  std::vector<u32> &&doc_offsets,
                                  std::vector<u32> &&centroid_ids,
                                  std::unique_ptr<u8[]> &&packed_residuals,
                                  size_t packed_residuals_size,
                                  std::vector<std::vector<u32>> &&ivf_lists,
                                  u32 total_docs,
                                  u64 total_embeddings) {
    std::unique_lock lock(rw_mutex_);

    LOG_INFO(fmt::format("PlaidIndex::AcceptMergedData: Starting. n_centroids={}, global_centroids_ref={}",
                         n_centroids_,
                         global_centroids_ref_ ? "set" : "null"));

    // Validate IVF lists size matches n_centroids
    if (ivf_lists.size() != n_centroids_) {
        UnrecoverableError(
            fmt::format("PlaidIndex::AcceptMergedData: IVF lists size {} doesn't match n_centroids {}", ivf_lists.size(), n_centroids_));
    }

    // Validate doc_lens size matches total_docs
    if (doc_lens.size() != total_docs) {
        UnrecoverableError(fmt::format("PlaidIndex::AcceptMergedData: doc_lens size {} doesn't match total_docs {}", doc_lens.size(), total_docs));
    }

    // Validate doc_offsets size matches total_docs
    if (doc_offsets.size() != total_docs) {
        UnrecoverableError(
            fmt::format("PlaidIndex::AcceptMergedData: doc_offsets size {} doesn't match total_docs {}", doc_offsets.size(), total_docs));
    }

    // Validate centroid_ids size matches total_embeddings
    if (centroid_ids.size() != total_embeddings) {
        UnrecoverableError(fmt::format("PlaidIndex::AcceptMergedData: centroid_ids size {} doesn't match total_embeddings {}",
                                       centroid_ids.size(),
                                       total_embeddings));
    }

    // Validate packed_residuals_size matches expected size
    size_t expected_packed_size = total_embeddings * ((embedding_dimension_ * nbits_ + 7) / 8);
    if (packed_residuals_size != expected_packed_size) {
        UnrecoverableError(fmt::format("PlaidIndex::AcceptMergedData: packed_residuals_size {} doesn't match expected {}",
                                       packed_residuals_size,
                                       expected_packed_size));
    }

    // Move all data efficiently
    doc_lens_ = std::move(doc_lens);
    doc_offsets_ = std::move(doc_offsets);
    centroid_ids_ = std::move(centroid_ids);
    packed_residuals_ = std::move(packed_residuals);
    packed_residuals_size_ = packed_residuals_size;
    packed_residuals_capacity_ = packed_residuals_size_;
    ivf_lists_ = std::move(ivf_lists);
    n_docs_ = total_docs;
    n_total_embeddings_ = total_embeddings;

    // CRITICAL: Copy centroids and quantizer from global centroids to make index self-contained
    // This is necessary for mmap-based indexes that don't have access to global centroids
    if (global_centroids_ref_) {
        LOG_INFO("PlaidIndex::AcceptMergedData: Copying centroids and quantizer from global centroids");

        // Copy centroids data
        const auto &global_centroids_data = global_centroids_ref_->centroids_data();
        const auto &global_norms = global_centroids_ref_->centroid_norms_neg_half();
        centroids_data_ = global_centroids_data;
        centroid_norms_neg_half_ = global_norms;

        // Copy quantizer
        quantizer_->CopyFrom(*global_centroids_ref_->quantizer());

        // Clear global centroids reference since we now have local copies
        // This makes the index self-contained and safe for mmap
        global_centroids_ref_.reset();

        LOG_INFO("PlaidIndex::AcceptMergedData: Copied centroids and quantizer, index is now self-contained");
    }

    LOG_INFO(fmt::format("PlaidIndex::AcceptMergedData: Completed. Total docs: {}, embeddings: {}, IVF entries: {}, quantizer n_buckets: {}",
                         n_docs_.load(),
                         n_total_embeddings_,
                         ivf_flattened_
                             ? ivf_data_.size()
                             : std::accumulate(ivf_lists_.begin(), ivf_lists_.end(), 0u, [](u32 sum, const auto &list) { return sum + list.size(); }),
                         quantizer_->n_buckets()));

    // Auto-flatten IVF after merge for cache-friendly search
    if (!ivf_flattened_ && n_centroids_ > DEFAULT_CENTROID_BATCH_SIZE) {
        FinalizeIVF();
    }
}

} // namespace infinity
