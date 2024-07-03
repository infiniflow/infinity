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

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdlib>
#include <map>
#include <memory>
#include <utility>
#include <vector>

module emvb_search;
import stl;
import emvb_result_handler;
import emvb_simd_funcs;
import mlas_matrix_multiply;
import emvb_product_quantization;
import emvb_shared_vec;
import infinity_exception;
import bitmask;
import segment_entry;
import block_index;
import knn_filter;

namespace infinity {

extern template class EMVBSharedVec<u32>;

using UniquePtrF32Aligned = std::unique_ptr<f32[], decltype([](f32 *p) { std::free(p); })>;

// For avx
auto Get256AlignedF32Array(const u32 elem_num) {
    const u32 alloc_bytes = (elem_num * sizeof(f32) + 31u) & (~(31u)); // need to be multiple of 32
    auto ptr = std::aligned_alloc(32, alloc_bytes);
    if (!ptr) {
        UnrecoverableError("Out of memory!");
    }
    UniquePtrF32Aligned result(static_cast<f32 *>(ptr));
    return result;
}

template <u32 FIXED_QUERY_TOKEN_NUM>
auto EMVBSearch<FIXED_QUERY_TOKEN_NUM>::find_candidate_docs(const f32 *centroids_scores, const u32 nprobe, const f32 th) const {
    std::vector<std::bitset<FIXED_QUERY_TOKEN_NUM>> centroid_q_token_sim(n_centroids_);
    // TODO: reuse space?
    auto filtered_centroid_ids = std::make_unique_for_overwrite<u32[]>(n_centroids_ + 8);
    std::vector<u32> closest_centroids_ids;
    closest_centroids_ids.reserve(FIXED_QUERY_TOKEN_NUM * nprobe);
    for (u32 i = 0; i < FIXED_QUERY_TOKEN_NUM; ++i) {
        const auto score_start_ptr = centroids_scores + i * n_centroids_;
        const auto filtered_end = filter_scores_output_ids(filtered_centroid_ids.get(), th, score_start_ptr, n_centroids_);
        const u32 result_id_n = filtered_end - filtered_centroid_ids.get();
        for (u32 j = 0; j < result_id_n; ++j) {
            centroid_q_token_sim[filtered_centroid_ids[j]].set(i);
        }
        if (result_id_n == 0) {
            // find the closest centroid
            const u32 argmax = std::distance(score_start_ptr, std::max_element(score_start_ptr, score_start_ptr + n_centroids_));
            closest_centroids_ids.push_back(argmax);
        } else if (result_id_n <= nprobe) {
            closest_centroids_ids.insert(closest_centroids_ids.end(), filtered_centroid_ids.get(), filtered_end);
        } else {
            // now result_id_n > nprobe
            using ResultHandler = EMVBReservoirResultHandler<f32, u32>;
            ResultHandler result_handler(nprobe);
            for (u32 j = 0; j < result_id_n; ++j) {
                const auto idx = filtered_centroid_ids[j];
                result_handler.Add(score_start_ptr[idx], idx);
            }
            result_handler.EndWithoutSort();
            const u32 result_cnt = result_handler.GetSize();
            assert(result_cnt == nprobe);
            auto idx_result = result_handler.GetIdPtr();
            closest_centroids_ids.insert(closest_centroids_ids.end(), idx_result.get(), idx_result.get() + result_cnt);
        }
    }
    std::unordered_set<u32> remove_dup_cid;
    std::vector<bool> doc_visited(n_docs_);
    std::vector<u32> candidate_docs;
    for (const auto &cid : closest_centroids_ids) {
        if (const auto [_, success] = remove_dup_cid.insert(cid); success) {
            const auto [docid_shared, size] = centroids_to_docid_[cid].GetData();
            const auto docid_ptr = docid_shared.get();
            for (u32 i = 0; i < size; ++i) {
                const auto doc_id = docid_ptr[i];
                if (doc_id >= n_docs_) {
                    // new added doc
                    break;
                }
                if (!doc_visited[doc_id]) {
                    candidate_docs.push_back(doc_id);
                    doc_visited[doc_id] = true;
                }
            }
        }
    }
    return std::make_pair(std::move(candidate_docs), std::move(centroid_q_token_sim));
}

template <u32 FIXED_QUERY_TOKEN_NUM>
auto EMVBSearch<FIXED_QUERY_TOKEN_NUM>::compute_hit_frequency(std::vector<u32> candidate_documents,
                                                              const u32 n_doc_to_score,
                                                              auto centroid_q_token_sim) const {
    static_assert(std::is_same_v<typename decltype(centroid_q_token_sim)::value_type, std::bitset<FIXED_QUERY_TOKEN_NUM>>);
    if (n_doc_to_score >= candidate_documents.size()) [[unlikely]] {
        // too few documents
        std::pair<u32, std::unique_ptr<u32[]>> result(candidate_documents.size(), nullptr);
        result.second = std::make_unique_for_overwrite<u32[]>(candidate_documents.size());
        std::copy(candidate_documents.begin(), candidate_documents.end(), result.second.get());
        return result;
    }
    using ResultHandler = EMVBReservoirResultHandler<u32, u32>;
    ResultHandler result_handler(n_doc_to_score);
    for (const auto doc_id : candidate_documents) {
        const auto doclen = doc_lens_[doc_id];
        const auto doc_offset = doc_offsets_[doc_id];
        std::bitset<FIXED_QUERY_TOKEN_NUM> mask; // query token mask of this doc
        for (u32 i = 0; i < doclen; ++i) {
            const auto cid = centroid_id_assignments_[doc_offset + i];
            mask |= centroid_q_token_sim[cid];
        }
        result_handler.Add(mask.count(), doc_id);
    }
    result_handler.EndWithoutSort();
    assert(result_handler.GetSize() == n_doc_to_score);
    return std::make_pair(result_handler.GetSize(), result_handler.GetIdPtr());
}

template <u32 FIXED_QUERY_TOKEN_NUM>
auto EMVBSearch<FIXED_QUERY_TOKEN_NUM>::compute_ip_of_vectors_in_doc_with_centroids(const u32 doc_id, const f32 *centroids_scores_transposed) const {
    const auto doclen = doc_lens_[doc_id];
    const auto doc_offset = doc_offsets_[doc_id];
    auto centroid_distances = Get256AlignedF32Array(doclen * FIXED_QUERY_TOKEN_NUM);
    auto dst_start = centroid_distances.get();
    for (u32 j = 0; j < doclen; ++j) {
        const auto centroid_id = centroid_id_assignments_[doc_offset + j];
        const auto src_start = centroids_scores_transposed + centroid_id * FIXED_QUERY_TOKEN_NUM;
        std::copy_n(src_start, FIXED_QUERY_TOKEN_NUM, dst_start);
        dst_start += FIXED_QUERY_TOKEN_NUM;
    }
    return centroid_distances;
}

template <u32 FIXED_QUERY_TOKEN_NUM, typename IndexSequence>
struct GetMaxSimSumUp;

template <u32 FIXED_QUERY_TOKEN_NUM, std::size_t... I>
struct GetMaxSimSumUp<FIXED_QUERY_TOKEN_NUM, std::index_sequence<I...>> {
    static f32 Get(const f32 *centroid_distances, const u32 doclen) {
        return (GetMaxSim32Width<FIXED_QUERY_TOKEN_NUM, I * 32>(centroid_distances, doclen) + ...);
    }
};

template <u32 FIXED_QUERY_TOKEN_NUM>
    requires((FIXED_QUERY_TOKEN_NUM % 32 == 0) && (FIXED_QUERY_TOKEN_NUM > 0))
struct GetMaxSimScoreOfDoc {
    static f32 Get(const f32 *centroid_distances, const u32 doclen) {
        return GetMaxSimSumUp<FIXED_QUERY_TOKEN_NUM, std::make_index_sequence<FIXED_QUERY_TOKEN_NUM / 32>>::Get(centroid_distances, doclen);
    }
};

template <u32 FIXED_QUERY_TOKEN_NUM>
auto EMVBSearch<FIXED_QUERY_TOKEN_NUM>::second_stage_filtering(auto selected_cnt_and_docs,
                                                               const u32 out_second_stage,
                                                               auto query_token_centroids_scores) const {
    static_assert(std::is_same_v<decltype(query_token_centroids_scores), UniquePtrF32Aligned>);
    const auto &[selected_cnt, selected_docs] = selected_cnt_and_docs;
    auto centroids_scores_transposed = std::make_unique_for_overwrite<f32[]>(FIXED_QUERY_TOKEN_NUM * n_centroids_);
    TransposeMatrix(query_token_centroids_scores.get(), centroids_scores_transposed.get(), FIXED_QUERY_TOKEN_NUM, n_centroids_);
    // release memory no longer needed
    query_token_centroids_scores.reset();
    using ResultHandler = EMVBReservoirResultHandler<f32, std::pair<u32, UniquePtrF32Aligned>>;
    ResultHandler result_handler(out_second_stage);
    for (u32 i = 0; i < selected_cnt; ++i) {
        const auto doc_id = selected_docs[i];
        auto centroid_scores = compute_ip_of_vectors_in_doc_with_centroids(doc_id, centroids_scores_transposed.get());
        const auto score = GetMaxSimScoreOfDoc<FIXED_QUERY_TOKEN_NUM>::Get(centroid_scores.get(), doc_lens_[doc_id]);
        result_handler.Add(score, std::make_pair(doc_id, std::move(centroid_scores)));
    }
    result_handler.EndWithoutSort();
    return std::make_pair(result_handler.GetSize(), result_handler.GetIdPtr());
}

template <typename T>
struct ReuseableBuffer {
    T *buffer_ = nullptr;
    u32 capacity_ = 0;
    ~ReuseableBuffer() { delete[] buffer_; }
    void Resize(const u32 num) {
        if (num > capacity_) {
            capacity_ = num + (num / 2);
            delete[] buffer_;
            buffer_ = nullptr;
            auto ptr = new T[capacity_];
            if (!ptr) {
                UnrecoverableError("Out of memory!");
            }
            buffer_ = static_cast<T *>(ptr);
        }
    }
    T *GetPtr() const { return buffer_; }
};

template <typename T>
struct ReuseableBuffer256Aligned {
    T *buffer_ = nullptr;
    u32 capacity_ = 0;
    ~ReuseableBuffer256Aligned() { std::free(buffer_); }
    void Resize(const u32 num) {
        if (num > capacity_) {
            std::free(buffer_);
            buffer_ = nullptr;
            capacity_ = (num + (num / 2) + 7u) & (~(7u));                     // for f32 and u32
            const u32 alloc_bytes = (capacity_ * sizeof(T) + 31u) & (~(31u)); // need to be multiple of 32
            auto ptr = std::aligned_alloc(32, alloc_bytes);
            if (!ptr) {
                UnrecoverableError("Out of memory!");
            }
            buffer_ = static_cast<T *>(ptr);
        }
    }
    T *GetPtr() const { return buffer_; }
};

template <u32 FIXED_QUERY_TOKEN_NUM>
auto EMVBSearch<FIXED_QUERY_TOKEN_NUM>::compute_topk_documents_selected(const f32 *query_ptr,
                                                                        auto selected_docs_centroid_scores,
                                                                        const u32 k,
                                                                        const f32 th) const {
    const auto residual_pq_ip_table = product_quantizer_->GetIPDistanceTable(query_ptr, FIXED_QUERY_TOKEN_NUM);
    using ResultHandler = EMVBReservoirResultHandler<f32, u32>;
    ResultHandler result_handler(k);
    auto &[selected_num, docs_centroid_scores_ptr] = selected_docs_centroid_scores;
    ReuseableBuffer<u32> embedding_id_buffer;
    ReuseableBuffer256Aligned<f32> distances_buffer;
    ReuseableBuffer256Aligned<f32> residual_distances_buffer;
    for (u32 selected_i = 0; selected_i < selected_num; ++selected_i) {
        const auto &[doc_id, centroid_distances] = docs_centroid_scores_ptr[selected_i];
        const auto doclen = doc_lens_[doc_id];
        const auto doc_offset = doc_offsets_[doc_id];
        distances_buffer.Resize(doclen);
        const auto distances_ptr = distances_buffer.GetPtr();
        embedding_id_buffer.Resize(doclen + 8);
        const auto embedding_id_ptr = embedding_id_buffer.GetPtr();
        // auto buffer_centroids = embedding_id_ptr
        Array<f32, FIXED_QUERY_TOKEN_NUM> maxs = {};
        for (u32 query_token_id = 0; query_token_id < FIXED_QUERY_TOKEN_NUM; ++query_token_id) {
            // TODO: transpose?
            for (u32 j = 0; j < doclen; ++j) {
                distances_ptr[j] = centroid_distances[j * FIXED_QUERY_TOKEN_NUM + query_token_id];
            }
            const auto embedding_id_ptr_end = filter_scores_output_ids(embedding_id_ptr, th, distances_ptr, doclen);
            if (embedding_id_ptr_end > embedding_id_ptr) [[likely]] {
                auto distances_ptr_end = distances_ptr;
                for (auto copy_embedding_id_ptr = embedding_id_ptr; copy_embedding_id_ptr < embedding_id_ptr_end; ++copy_embedding_id_ptr) {
                    const auto embedding_id = *copy_embedding_id_ptr;
                    *distances_ptr_end = distances_ptr[embedding_id] + product_quantizer_->GetSingleIPDistance(doc_offset + embedding_id,
                                                                                                               query_token_id,
                                                                                                               FIXED_QUERY_TOKEN_NUM,
                                                                                                               residual_pq_ip_table.get());
                    ++distances_ptr_end;
                }
                maxs[query_token_id] = *std::max_element(distances_ptr, distances_ptr_end);
            } else {
                // exhausitive search
                residual_distances_buffer.Resize(doclen);
                const auto residual_distances_ptr = residual_distances_buffer.GetPtr();
                product_quantizer_->GetMultipleIPDistance(doc_offset,
                                                          doclen,
                                                          query_token_id,
                                                          FIXED_QUERY_TOKEN_NUM,
                                                          residual_pq_ip_table.get(),
                                                          residual_distances_ptr);
                f32 max_val = std::numeric_limits<f32>::lowest();
                for (size_t j = 0; j < doclen; j++) {
                    const f32 sum_v = distances_ptr[j] + residual_distances_ptr[j];
                    max_val = std::max(max_val, sum_v);
                }
                maxs[query_token_id] = max_val;
            }
        }
        const f32 score = std::reduce(maxs.begin(), maxs.end());
        result_handler.Add(score, doc_id);
    }
    result_handler.EndSort();
    return std::make_tuple(result_handler.GetSize(), result_handler.GetDistancePtr(), result_handler.GetIdPtr());
}

template <u32 FIXED_QUERY_TOKEN_NUM>
EMVBSearch<FIXED_QUERY_TOKEN_NUM>::EMVBSearch(const u32 embedding_dimension,
                                              const u32 n_docs,
                                              const u32 n_centroids, // need to be a multiple of 8
                                              const u32 *doc_lens,
                                              const u32 *doc_offsets,
                                              const u32 *centroid_id_assignments,
                                              const f32 *centroids_data,
                                              const EMVBSharedVec<u32> *centroids_to_docid,
                                              const EMVBProductQuantizer *product_quantizer)
    : embedding_dimension_(embedding_dimension), n_docs_(n_docs), n_centroids_(n_centroids), doc_lens_(doc_lens), doc_offsets_(doc_offsets),
      centroid_id_assignments_(centroid_id_assignments), centroids_data_(centroids_data), centroids_to_docid_(centroids_to_docid),
      product_quantizer_(product_quantizer) {}

// return docid: start from 0
template <u32 FIXED_QUERY_TOKEN_NUM>
std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>> EMVBSearch<FIXED_QUERY_TOKEN_NUM>::GetQueryResult(const f32 *query_ptr,
                                                                                                                  const u32 nprobe,
                                                                                                                  const f32 thresh,
                                                                                                                  const u32 n_doc_to_score,
                                                                                                                  const u32 out_second_stage,
                                                                                                                  const u32 k,
                                                                                                                  const f32 thresh_query) const {
    assert(n_centroids_ % 8 == 0);
    auto query_token_centroids_scores = Get256AlignedF32Array(FIXED_QUERY_TOKEN_NUM * n_centroids_);
    matrixA_multiply_transpose_matrixB_output_to_C(query_ptr,
                                                   centroids_data_,
                                                   FIXED_QUERY_TOKEN_NUM,
                                                   n_centroids_,
                                                   embedding_dimension_,
                                                   query_token_centroids_scores.get());
    const u32 real_nprobe = std::min(n_centroids_, nprobe);
    assert(real_nprobe > 0);
    auto [candidate_docs, centroid_q_token_sim] = find_candidate_docs(query_token_centroids_scores.get(), real_nprobe, thresh);
    auto selected_cnt_and_docs = compute_hit_frequency(std::move(candidate_docs), n_doc_to_score, std::move(centroid_q_token_sim));
    auto selected_docs_centroid_scores =
        second_stage_filtering(std::move(selected_cnt_and_docs), out_second_stage, std::move(query_token_centroids_scores));
    auto query_res = compute_topk_documents_selected(query_ptr, std::move(selected_docs_centroid_scores), k, thresh_query);
    static_assert(std::is_same_v<decltype(query_res), std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>>);
    return query_res;
}

// return docid: start from start_segment_offset
template <u32 FIXED_QUERY_TOKEN_NUM>
Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>> EMVBSearch<FIXED_QUERY_TOKEN_NUM>::GetQueryResult(const f32 *query_ptr,
                                                                                                 const u32 nprobe,
                                                                                                 const f32 thresh,
                                                                                                 const u32 n_doc_to_score,
                                                                                                 const u32 out_second_stage,
                                                                                                 const u32 k,
                                                                                                 const f32 thresh_query,
                                                                                                 Bitmask &bitmask,
                                                                                                 const u32 start_segment_offset,
                                                                                                 const SegmentEntry *segment_entry,
                                                                                                 const BlockIndex *block_index,
                                                                                                 const TxnTimeStamp begin_ts) const {
    assert(n_centroids_ % 8 == 0);
    auto query_token_centroids_scores = Get256AlignedF32Array(FIXED_QUERY_TOKEN_NUM * n_centroids_);
    matrixA_multiply_transpose_matrixB_output_to_C(query_ptr,
                                                   centroids_data_,
                                                   FIXED_QUERY_TOKEN_NUM,
                                                   n_centroids_,
                                                   embedding_dimension_,
                                                   query_token_centroids_scores.get());
    const u32 real_nprobe = std::min(n_centroids_, nprobe);
    assert(real_nprobe > 0);
    auto [candidate_docs, centroid_q_token_sim] = find_candidate_docs(query_token_centroids_scores.get(), real_nprobe, thresh);
    std::vector<u32> candidate_docs_filtered;
    auto filter_doc = [&candidate_docs_filtered, &candidate_docs, start_segment_offset](auto &&filter) {
        candidate_docs_filtered.reserve(candidate_docs.size());
        for (const auto doc_id : candidate_docs) {
            if (filter(doc_id + start_segment_offset)) {
                candidate_docs_filtered.push_back(doc_id);
            }
        }
    };
    if (const bool use_bitmask = !bitmask.IsAllTrue(); use_bitmask) {
        if (segment_entry->CheckAnyDelete(begin_ts)) {
            DeleteWithBitmaskFilter filter(bitmask, segment_entry, begin_ts);
            filter_doc(filter);
        } else {
            BitmaskFilter<SegmentOffset> filter(bitmask);
            filter_doc(filter);
        }
    } else {
        if (segment_entry->CheckAnyDelete(begin_ts)) {
            const auto segment_id = segment_entry->segment_id();
            const SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
            DeleteFilter filter(segment_entry, begin_ts, max_segment_offset);
            filter_doc(filter);
        } else {
            // no delete
            candidate_docs_filtered = std::move(candidate_docs);
        }
    }
    auto selected_cnt_and_docs = compute_hit_frequency(std::move(candidate_docs_filtered), n_doc_to_score, std::move(centroid_q_token_sim));
    auto selected_docs_centroid_scores =
        second_stage_filtering(std::move(selected_cnt_and_docs), out_second_stage, std::move(query_token_centroids_scores));
    auto query_res = compute_topk_documents_selected(query_ptr, std::move(selected_docs_centroid_scores), k, thresh_query);
    static_assert(std::is_same_v<decltype(query_res), std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>>);
    // consider start_segment_offset
    auto &[doc_num, scores, doc_ids] = query_res;
    for (u32 i = 0; i < doc_num; ++i) {
        doc_ids[i] += start_segment_offset;
    }
    return query_res;
}

template class EMVBSearch<32>;
template class EMVBSearch<64>;
template class EMVBSearch<96>;
template class EMVBSearch<128>;
template class EMVBSearch<160>;
template class EMVBSearch<192>;
template class EMVBSearch<224>;
template class EMVBSearch<256>;

} // namespace infinity
