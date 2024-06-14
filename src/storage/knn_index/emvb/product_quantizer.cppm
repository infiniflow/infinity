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

export module emvb_product_quantization;
import stl;

namespace infinity {

export class EMVBProductQuantizer {
public:
    virtual ~EMVBProductQuantizer() = default;
    virtual void Train(const f32 *embedding_data, u32 embedding_num, u32 iter_cnt) = 0;
    virtual void AddEmbeddings(const f32 *embedding_data, u32 embedding_num) = 0;
    virtual UniquePtr<f32[]> GetIPDistanceTable(const f32 *query_data, u32 query_num) const = 0;
    virtual f32 GetSingleIPDistance(u32 embedding_id, u32 query_id, u32 query_num, const f32 *ip_table) const = 0;
    virtual void
    GetMultipleIPDistance(u32 embedding_offset, u32 embedding_num, u32 query_id, u32 query_num, const f32 *ip_table, f32 *output_ptr) const = 0;
};

template <std::unsigned_integral SUBSPACE_CENTROID_TAG, u32 SUBSPACE_NUM>
class PQ : public EMVBProductQuantizer {
protected:
    static constexpr u32 subspace_centroid_num_ = std::numeric_limits<SUBSPACE_CENTROID_TAG>::max() + 1;
    const u32 subspace_dimension_;                             // dimension of each subspace
    const u32 dimension_ = SUBSPACE_NUM * subspace_dimension_; // dimension of each embedding
    Array<Vector<f32>, SUBSPACE_NUM> subspace_centroids_ = {}; // centroids for each subspace, size: subspace_centroid_num_ * subspace_dimension_
    Array<Array<f32, subspace_centroid_num_>, SUBSPACE_NUM> subspace_centroid_norms_neg_half =
        {}; // (-0.5 * norm) for each centroid, size: subspace_centroid_num_
    Deque<Array<SUBSPACE_CENTROID_TAG, SUBSPACE_NUM>> encoded_embedding_data_;
    u32 next_embedding_id_ = 0;
    std::mutex write_mutex_;

    explicit PQ(u32 subspace_dimension);

    void Train(const f32 *embedding_data, u32 embedding_num, u32 iter_cnt) override;

    f32 GetSingleIPDistance(u32 embedding_id, u32 query_id, u32 query_num, const f32 *ip_table) const override;

    void
    GetMultipleIPDistance(u32 embedding_offset, u32 embedding_num, u32 query_id, u32 query_num, const f32 *ip_table, f32 *output_ptr) const override;

    void EncodeEmbedding(const f32 *embedding_data, u32 embedding_num, auto output_iter) const;

    UniquePtr<f32[]> DecodeEmbedding(auto input_iter_begin, u32 embedding_num) const;
};

// Optimized Product Quantization
// embedding data type: f32
export template <std::unsigned_integral SUBSPACE_CENTROID_TAG, u32 SUBSPACE_NUM>
class OPQ final : public PQ<SUBSPACE_CENTROID_TAG, SUBSPACE_NUM> {
    using PQ_BASE = PQ<SUBSPACE_CENTROID_TAG, SUBSPACE_NUM>;
    UniquePtr<f32[]> matrix_R_; // matrix R for OPQ, size: dimension_ * dimension_

public:
    explicit OPQ(u32 subspace_dimension);

    void Train(const f32 *embedding_data, u32 embedding_num, u32 iter_cnt) override;

    void AddEmbeddings(const f32 *embedding_data, u32 embedding_num) override;

    UniquePtr<f32[]> GetIPDistanceTable(const f32 *query_data, u32 query_num) const override;
};

export UniquePtr<EMVBProductQuantizer> GetEMVBOPQ(u32 pq_subspace_num, u32 pq_subspace_bits, u32 embedding_dimension);

} // namespace infinity
