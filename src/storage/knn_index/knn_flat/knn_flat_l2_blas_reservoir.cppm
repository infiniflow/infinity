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

export module knn_flat_l2_blas_reservoir;

import stl;
import knn_result_handler;
import knn_distance;
import mlas_matrix_multiply;
import bitmask;

import logger;
import infinity_exception;
import default_values;
import vector_distance;
import knn_expr;
import internal_types;

namespace infinity {

export template <typename DistType>
class KnnFlatL2BlasReservoir final : public KnnDistance<DistType> {

    using ResultHandler = ReservoirResultHandler<CompareMax<DistType, RowID>>;

public:
    explicit KnnFlatL2BlasReservoir(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2BlasReservoir, elem_data_type, query_count, dimension, topk), queries_(queries) {
        id_array_ = MakeUniqueForOverwrite<RowID[]>(topk * query_count);
        distance_array_ = MakeUniqueForOverwrite<DistType[]>(topk * query_count);

        result_handler_ = MakeUnique<ResultHandler>(query_count, topk, distance_array_.get(), id_array_.get());
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        // block sizes
        const SizeT bs_x = DISTANCE_COMPUTE_BLAS_QUERY_BS;
        const SizeT bs_y = DISTANCE_COMPUTE_BLAS_DATABASE_BS;
        // const SizeT bs_x = 16, bs_y = 16;

        ip_block_ = MakeUniqueForOverwrite<DistType[]>(bs_x * bs_y);
        x_norms_ = MakeUniqueForOverwrite<DistType[]>(this->query_count_);

        L2NormsSquares(x_norms_.get(), queries_, this->dimension_, this->query_count_);

        result_handler_->Begin();
        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            String error_message = "KnnFlatL2BlasReservoir isn't begin";
            LOG_ERROR(error_message);
            UnrecoverableError(error_message);
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        y_norms_ = MakeUniqueForOverwrite<DistType[]>(base_count);
        L2NormsSquares(y_norms_.get(), base, this->dimension_, base_count);

        // block sizes
        const SizeT bs_x = DISTANCE_COMPUTE_BLAS_QUERY_BS;
        const SizeT bs_y = DISTANCE_COMPUTE_BLAS_DATABASE_BS;
        u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            for (u16 j0 = 0; j0 < base_count; j0 += bs_y) {
                u16 j1 = j0 + bs_y;
                if (j1 > base_count)
                    j1 = base_count;
                /* compute the actual dot products */
                {
                    int nyi = j1 - j0, nxi = i1 - i0, di = this->dimension_;
                    matrixA_multiply_transpose_matrixB_output_to_C(queries_ + i0 * this->dimension_,
                                                                   base + j0 * this->dimension_,
                                                                   nxi,
                                                                   nyi,
                                                                   di,
                                                                   ip_block_.get());
                }
                for (SizeT i = i0; i < i1; i++) {
                    DistType *ip_line = ip_block_.get() + (i - i0) * (j1 - j0);

                    for (SizeT j = j0; j < j1; j++) {
                        DistType ip = *ip_line;
                        DistType dis = x_norms_[i] + y_norms_[j] - 2 * ip;

                        // negative values can occur for identical vectors
                        // due to roundoff errors
                        if (dis < 0)
                            dis = 0;

                        *ip_line = dis;
                        ip_line++;
                    }
                }
                result_handler_->AddResults(i0, i1, j0, j1, ip_block_.get(), segment_id, segment_offset_start);
            }
        }
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id, Bitmask &bitmask) final {
        if (bitmask.IsAllTrue()) {
            Search(base, base_count, segment_id, block_id);
            return;
        }
        if (!begin_) {
            String error_message = "KnnFlatL2BlasReservoir isn't begin";
            LOG_ERROR(error_message);
            UnrecoverableError(error_message);
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        y_norms_ = MakeUniqueForOverwrite<DistType[]>(base_count);
        L2NormsSquares(y_norms_.get(), base, this->dimension_, base_count);

        // block sizes
        const SizeT bs_x = DISTANCE_COMPUTE_BLAS_QUERY_BS;
        const SizeT bs_y = DISTANCE_COMPUTE_BLAS_DATABASE_BS;
        u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            for (u16 j0 = 0; j0 < base_count; j0 += bs_y) {
                u16 j1 = j0 + bs_y;
                if (j1 > base_count)
                    j1 = base_count;
                /* compute the actual dot products */
                {
                    int nyi = j1 - j0, nxi = i1 - i0, di = this->dimension_;
                    matrixA_multiply_transpose_matrixB_output_to_C(queries_ + i0 * this->dimension_,
                                                                   base + j0 * this->dimension_,
                                                                   nxi,
                                                                   nyi,
                                                                   di,
                                                                   ip_block_.get());
                }
                for (SizeT i = i0; i < i1; i++) {
                    DistType *ip_line = ip_block_.get() + (i - i0) * (j1 - j0);

                    for (SizeT j = j0; j < j1; j++) {
                        if (bitmask.IsTrue(segment_offset_start + j)) {
                            DistType ip = *ip_line;
                            DistType dis = x_norms_[i] + y_norms_[j] - 2 * ip;

                            // negative values can occur for identical vectors
                            // due to roundoff errors
                            if (dis < 0)
                                dis = 0;

                            *ip_line = dis;
                        } else {
                            *ip_line = std::numeric_limits<DistType>::max();
                        }
                        ip_line++;
                    }
                }
                result_handler_->AddResults(i0, i1, j0, j1, ip_block_.get(), segment_id, segment_offset_start);
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        result_handler_->End();

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_.get(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_.get(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            String error_message = "Query index exceeds the limit";
            LOG_ERROR(error_message);
            UnrecoverableError(error_message);
        }
        return distance_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            String error_message = "Query index exceeds the limit";
            LOG_ERROR(error_message);
            UnrecoverableError(error_message);
        }
        return id_array_.get() + idx * this->top_k_;
    }

private:
    UniquePtr<RowID[]> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<ResultHandler> result_handler_{};
    const DistType *queries_{};
    bool begin_{false};

    UniquePtr<DistType[]> ip_block_{};
    UniquePtr<DistType[]> x_norms_{};
    UniquePtr<DistType[]> y_norms_{};
};

template class KnnFlatL2BlasReservoir<f32>;

} // namespace infinity
