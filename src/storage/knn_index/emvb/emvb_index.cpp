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

module;

module emvb_index;
import stl;
import mlas_matrix_multiply;
import vector_distance;
import emvb_product_quantization;
import emvb_search;
import kmeans_partition;
import index_base;
import status;
import logger;
import third_party;
import infinity_exception;

namespace infinity {

extern template class EMVBSharedVec<u32>;
extern template class EMVBSearch<32>;
extern template class EMVBSearch<64>;
extern template class EMVBSearch<96>;
extern template class EMVBSearch<128>;
extern template class EMVBSearch<160>;
extern template class EMVBSearch<192>;
extern template class EMVBSearch<224>;
extern template class EMVBSearch<256>;

EMVBIndex::EMVBIndex(const u32 start_segment_offset,
                     const u32 embedding_dimension,
                     const u32 n_centroids,
                     const u32 residual_pq_subspace_num,
                     const u32 residual_pq_subspace_bits)
    : start_segment_offset_(start_segment_offset), embedding_dimension_(embedding_dimension), n_centroids_(n_centroids),
      residual_pq_subspace_num_(residual_pq_subspace_num), residual_pq_subspace_bits_(residual_pq_subspace_bits) {
    centroids_data_.resize(n_centroids * embedding_dimension);
    centroid_norms_neg_half_.resize(n_centroids);
    centroids_to_docid_ = MakeUnique<EMVBSharedVec<u32>[]>(n_centroids);
    // now always use OPQ
    product_quantizer_ = GetEMVBOPQ(residual_pq_subspace_num, residual_pq_subspace_bits, embedding_dimension);
}

// need embedding num:
// 1. 256 * n_centroids_ for centroids
// 2. 256 * (1 << residual_pq_subspace_bits) for residual product quantizer
u32 EMVBIndex::ExpectLeastTrainingDataNum() const { return std::max<u32>(256 * n_centroids_, 256 * (1 << residual_pq_subspace_bits_)); }

void EMVBIndex::Train(const f32 *embedding_data, const u32 embedding_num, const u32 iter_cnt) {
    // check n_centroids_
    if (((n_centroids_ % 8) != 0) || (n_centroids_ == 0)) {
        const auto error_msg = fmt::format("EMVBIndex::Train: n_centroids_ must be a multiple of 8, got {} instead.", n_centroids_);
        LOG_ERROR(error_msg);
        UnrecoverableError(error_msg);
    }
    // check training data num
    if (const u32 least_num = ExpectLeastTrainingDataNum(); embedding_num < least_num) {
        const auto error_msg = fmt::format("EMVBIndex::Train: embedding_num must be at least {}, got {} instead.", least_num, embedding_num);
        LOG_ERROR(error_msg);
        UnrecoverableError(error_msg);
    }
    // train both centroids and residual product quantizer
    // step 1. train centroids
    {
        const auto result_centroid_num = GetKMeansCentroids<f32>(MetricType::kMetricL2,
                                                                 embedding_dimension_,
                                                                 embedding_num,
                                                                 embedding_data,
                                                                 centroids_data_,
                                                                 n_centroids_,
                                                                 iter_cnt);
        if (result_centroid_num != n_centroids_) {
            const auto error_msg =
                fmt::format("EMVBIndex::Train: KMeans failed to get {} centroids, got {} instead.", n_centroids_, result_centroid_num);
            LOG_ERROR(error_msg);
            UnrecoverableError(error_msg);
        }
        LOG_TRACE(fmt::format("EMVBIndex::Train: KMeans got {} centroids.", result_centroid_num));
    }
    {
        const f32 *centroid_data = centroids_data_.data();
        for (u32 i = 0; i < n_centroids_; ++i) {
            centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, embedding_dimension_);
            centroid_data += embedding_dimension_;
        }
    }
    // step 2. get residuals
    const auto residuals = MakeUniqueForOverwrite<f32[]>(embedding_num * embedding_dimension_);
    {
        // distance: for every embedding, e * c - 0.5 * c^2, find max
        const auto dist_table = MakeUniqueForOverwrite<f32[]>(embedding_num * n_centroids_);
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
            const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
            for (u32 j = 0; j < embedding_dimension_; ++j) {
                output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
            }
        }
    }
    LOG_TRACE("EMVBIndex::Train: Finish calculate residuals.");
    // step 3. train residuals
    product_quantizer_->Train(residuals.get(), embedding_num, iter_cnt);
    LOG_TRACE("EMVBIndex::Train: Finish train pq for residuals.");
}

void EMVBIndex::AddOneDocEmbeddings(const f32 *embedding_data, const u32 embedding_num) {
    std::lock_guard lock(append_mutex_);
    // only one thread can add doc to the index at the same time
    // step 1. doc - embedding info
    const u32 old_doc_num = n_docs_;
    const u32 old_total_embeddings = n_total_embeddings_;
    doc_lens_.PushBack(embedding_num);
    doc_offsets_.PushBack(old_total_embeddings);
    // step 2. assign to centroids
    const auto centroid_id_assignments = MakeUniqueForOverwrite<u32[]>(embedding_num);
    const auto residuals = MakeUniqueForOverwrite<f32[]>(embedding_num * embedding_dimension_);
    {
        // distance: for every embedding, e * c - 0.5 * c^2, find max
        const auto dist_table = MakeUniqueForOverwrite<f32[]>(embedding_num * n_centroids_);
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
    centroid_id_assignments_.PushBack(centroid_id_assignments.get(), centroid_id_assignments.get() + embedding_num);
    for (u32 i = 0; i < embedding_num; ++i) {
        const u32 centroid_id = centroid_id_assignments[i];
        centroids_to_docid_[centroid_id].PushBackIfDifferentFromLast(old_doc_num);
    }
    // step 3. add residuals to product quantizer
    product_quantizer_->AddEmbeddings(residuals.get(), embedding_num);
    // finally, update count
    n_total_embeddings_ += embedding_num;
    ++n_docs_;
}

// the two thresholds are for every (query embedding, candidate embedding) pair
// candidate embeddings are centroids
// unqualified pairs will not be scored
// but if nothing is left, an exhaustive search will be performed

constexpr u32 current_max_query_token_num = 256;

EMVBQueryResultType EMVBIndex::GetQueryResult(const f32 *query_ptr,
                                              const u32 query_embedding_num,
                                              const u32 centroid_nprobe,  // step 1, centroid candidates for every query embedding
                                              const f32 threshold_first,  // step 1, threshold for query - centroid score
                                              const u32 n_doc_to_score,   // topn by centroids hit count
                                              const u32 out_second_stage, // step 2, topn, use nearest centroid score as embedding score
                                              const u32 top_k,            // step 3, final topk, refine score by residual pq
                                              const f32 threshold_final   // step 3, threshold to reduce maxsim calculation
) const {
    // template argument should be in ascending order
    // keep consistent with emvb_search.cpp
    return query_token_num_helper<32, 64, 96, 128, 160, 192, 224, 256>(query_ptr,
                                                                       query_embedding_num,
                                                                       centroid_nprobe,
                                                                       threshold_first,
                                                                       n_doc_to_score,
                                                                       out_second_stage,
                                                                       top_k,
                                                                       threshold_final);
}

template <u32 I, u32... J>
EMVBQueryResultType EMVBIndex::query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto... query_args) const {
    if (query_embedding_num <= I) {
        return GetQueryResultT<I>(query_ptr, query_embedding_num, query_args...);
    }
    return query_token_num_helper<J...>(query_ptr, query_embedding_num, query_args...);
}

template <>
EMVBQueryResultType EMVBIndex::query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto... query_args) const {
    auto error_msg = fmt::format("EMVBIndex::GetQueryResult: query_embedding_num max value: {}, got {} instead.",
                                 current_max_query_token_num,
                                 query_embedding_num);
    error_msg += fmt::format(" Embeddings after {} will not be used for search.", current_max_query_token_num);
    error_msg += " Please Add instantiation of EMVBSearch with a bigger FIXED_QUERY_TOKEN_NUM value.";
    LOG_ERROR(error_msg);
    return GetQueryResultT<current_max_query_token_num>(query_ptr, query_embedding_num, query_args...);
}

template <u32 FIXED_QUERY_TOKEN_NUM>
EMVBQueryResultType EMVBIndex::GetQueryResultT(const f32 *query_ptr, const u32 query_embedding_num, auto... query_args) const {
    UniquePtr<f32[]> extended_query_ptr;
    const f32 *query_ptr_to_use = query_ptr;
    // extend query to FIXED_QUERY_TOKEN_NUM
    if (query_embedding_num < FIXED_QUERY_TOKEN_NUM) {
        extended_query_ptr = MakeUniqueForOverwrite<f32[]>(FIXED_QUERY_TOKEN_NUM * embedding_dimension_);
        std::copy_n(query_ptr, query_embedding_num * embedding_dimension_, extended_query_ptr.get());
        std::fill_n(extended_query_ptr.get() + query_embedding_num * embedding_dimension_,
                    (FIXED_QUERY_TOKEN_NUM - query_embedding_num) * embedding_dimension_,
                    0.0f);
        query_ptr_to_use = extended_query_ptr.get();
    }
    // access snapshot of index data
    const u32 n_docs = n_docs_.load(std::memory_order_acquire);
    const auto doc_lens_snapshot = doc_lens_.GetData();
    const auto doc_offsets_snapshot = doc_offsets_.GetData();
    const auto centroid_id_assignments_snapshot = centroid_id_assignments_.GetData();
    // execute search
    EMVBSearch<FIXED_QUERY_TOKEN_NUM> search_helper(embedding_dimension_,
                                                    n_docs,
                                                    n_centroids_,
                                                    doc_lens_snapshot.first.get(),
                                                    doc_offsets_snapshot.first.get(),
                                                    centroid_id_assignments_snapshot.first.get(),
                                                    centroids_data_.data(),
                                                    centroids_to_docid_.get(),
                                                    product_quantizer_.get());
    return search_helper.GetQueryResult(query_ptr_to_use, query_args...);
}

} // namespace infinity
