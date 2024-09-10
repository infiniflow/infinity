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

export module emvb_search;
import stl;
import emvb_shared_vec;
import simd_init;
import simd_functions;
import roaring_bitmap;

namespace infinity {

extern template class EMVBSharedVec<u32>;
class EMVBProductQuantizer;
struct BlockIndex;
struct SegmentEntry;

// REQUIREMENTS:
// FIXED_QUERY_TOKEN_NUM % 8 == 0, to simplify alignment
// FIXED_QUERY_TOKEN_NUM % 32 == 0, to simplify avx
// n_centroids % 8 == 0, to simplify alignment

export template <u32 FIXED_QUERY_TOKEN_NUM>
class EMVBSearch {
    static_assert(FIXED_QUERY_TOKEN_NUM % 32 == 0, "FIXED_QUERY_TOKEN_NUM must be a multiple of 32");
    // info of the index
    const u32 embedding_dimension_;                 // dimension of the embeddings
    const u32 n_docs_;                              // number of documents in the entire collection
    const u32 n_centroids_;                         // number of centroids
    const u32 *doc_lens_;                           // array of document lengths
    const u32 *doc_offsets_;                        // start offsets of each document in all the embeddings
    const u32 *centroid_id_assignments_;            // centroid id assignments for each embedding
    const f32 *centroids_data_;                     // centroids data
    const EMVBSharedVec<u32> *centroids_to_docid_;  // docids belonging to each centroid
    const EMVBProductQuantizer *product_quantizer_; // product quantizer for residuals of the embeddings
    FilterScoresOutputIdsFuncType filter_scores_output_ids_func_ptr = GetSIMD_FUNCTIONS().FilterScoresOutputIds_func_ptr_;

public:
    EMVBSearch(u32 embedding_dimension,
               u32 n_docs,
               u32 n_centroids, // need to be a multiple of 8
               const u32 *doc_lens,
               const u32 *doc_offsets,
               const u32 *centroid_id_assignments,
               const f32 *centroids_data,
               const EMVBSharedVec<u32> *centroids_to_docid,
               const EMVBProductQuantizer *product_quantizer);

    // return docid: start from 0
    Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>>
    GetQueryResult(const f32 *query_ptr, u32 nprobe, f32 thresh, u32 n_doc_to_score, u32 out_second_stage, u32 k, f32 thresh_query) const;

    // return docid: start from start_segment_offset
    Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>> GetQueryResult(const f32 *query_ptr,
                                                                  u32 nprobe,
                                                                  f32 thresh,
                                                                  u32 n_doc_to_score,
                                                                  u32 out_second_stage,
                                                                  u32 k,
                                                                  f32 thresh_query,
                                                                  Bitmask &bitmask,
                                                                  u32 start_segment_offset,
                                                                  const SegmentEntry *segment_entry,
                                                                  const BlockIndex *block_index,
                                                                  TxnTimeStamp begin_ts) const;

private:
    auto find_candidate_docs(const f32 *centroids_scores, u32 nprobe, f32 th) const;

    auto compute_hit_frequency(Vector<u32> candidate_documents, u32 n_doc_to_score, auto centroid_q_token_sim) const;

    auto second_stage_filtering(auto selected_cnt_and_docs, u32 out_second_stage, auto query_token_centroids_scores) const;

    auto compute_ip_of_vectors_in_doc_with_centroids(u32 doc_id, const f32 *centroids_scores_transposed) const;

    auto compute_topk_documents_selected(const f32 *query_ptr, auto selected_docs_centroid_scores, u32 k, f32 th) const;
};

} // namespace infinity
