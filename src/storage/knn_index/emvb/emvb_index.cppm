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

export module emvb_index;
import stl;
import emvb_shared_vec;

namespace infinity {

class EMVBProductQuantizer;

using EMVBQueryResultType = Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>>;

// created with fixed embedding dimension and number of centroids
export class EMVBIndex {
    const u32 start_segment_offset_ = 0;                 // start offset of the index in the segment
    const u32 embedding_dimension_ = 0;                  // dimension of the embeddings
    const u32 n_centroids_ = 0;                          // number of centroids, need to be a multiple of 8
    const u32 residual_pq_subspace_num_ = 0;             // number of subspaces in the residual product quantizer
    const u32 residual_pq_subspace_bits_ = 0;            // number of bits for each centroid representation in the residual product quantizer
    Vector<f32> centroids_data_;                         // centroids data
    Vector<f32> centroid_norms_neg_half_;                // (-0.5 * norm) for each centroid
    atomic_u32 n_docs_ = 0;                              // number of documents in the entire collection
    u32 n_total_embeddings_ = 0;                         // number of embeddings in the entire collection
    EMVBSharedVec<u32> doc_lens_;                        // array of document lengths
    EMVBSharedVec<u32> doc_offsets_;                     // start offsets of each document in all the embeddings
    EMVBSharedVec<u32> centroid_id_assignments_;         // centroid id assignments for each embedding
    UniquePtr<EMVBSharedVec<u32>[]> centroids_to_docid_; // docids belonging to each centroid
    UniquePtr<EMVBProductQuantizer> product_quantizer_;  // product quantizer for residuals of the embeddings
    std::mutex append_mutex_;                            // mutex for append all embeddings for one doc

public:
    EMVBIndex(u32 start_segment_offset, u32 embedding_dimension, u32 n_centroids, u32 residual_pq_subspace_num, u32 residual_pq_subspace_bits);

    [[nodiscard]] u32 ExpectLeastTrainingDataNum() const;

    void Train(const f32 *embedding_data, u32 embedding_num, u32 iter_cnt = 20);

    void AddOneDocEmbeddings(const f32 *embedding_data, u32 embedding_num);

    // the two thresholds are for every (query embedding, candidate embedding) pair
    // candidate embeddings are centroids
    // unqualified pairs will not be scored
    // but if nothing is left, an exhaustive search will be performed
    EMVBQueryResultType GetQueryResult(const f32 *query_ptr,
                                       u32 query_embedding_num,
                                       u32 centroid_nprobe,  // step 1, centroid candidates for every query embedding
                                       f32 threshold_first,  // step 1, threshold for query - centroid score
                                       u32 n_doc_to_score,   // topn by centroids hit count
                                       u32 out_second_stage, // step 2, topn, use nearest centroid score as embedding score
                                       u32 top_k,            // step 3, final topk, refine score by residual pq
                                       f32 threshold_final   // step 3, threshold to reduce maxsim calculation
    ) const;

private:
    template <u32 I, u32... J>
    EMVBQueryResultType query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto... query_args) const;

    template <>
    EMVBQueryResultType query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto... query_args) const;

    template <u32 FIXED_QUERY_TOKEN_NUM>
    EMVBQueryResultType GetQueryResultT(const f32 *query_ptr, u32 query_embedding_num, auto... query_args) const;
};

} // namespace infinity
