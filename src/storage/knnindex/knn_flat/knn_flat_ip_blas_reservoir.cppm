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

import stl;
import knn_result_handler;
import knn_distance;
import mlas_matrix_multiply;
import bitmask;
import parser;

import infinity_exception;
import default_values;

export module knn_flat_ip_blas_reservoir;

namespace infinity {

export template <typename DistType>
class KnnFlatIPBlasReservoir final : public KnnDistance<DistType> {

    using ResultHandler = ReservoirResultHandler<CompareMin<DistType, RowID>>;

public:
    explicit KnnFlatIPBlasReservoir(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIpBlasReservoir, elem_data_type, query_count, dimension, topk), queries_(queries) {
        id_array_ = MakeUniqueForOverwrite<RowID[]>(topk * query_count);
        distance_array_ = MakeUniqueForOverwrite<DistType[]>(topk * query_count);
        result_handler_ = MakeUnique<ResultHandler>(query_count, topk, distance_array_.get(), id_array_.get());
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        const SizeT bs_x = DISTANCE_COMPUTE_BLAS_QUERY_BS;
        const SizeT bs_y = DISTANCE_COMPUTE_BLAS_DATABASE_BS;
        ip_block_ = MakeUniqueForOverwrite<DistType[]>(bs_x * bs_y);

        result_handler_->Begin();
        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatIPBlasReservoir isn't begin");
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

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
            Error<ExecutorException>("KnnFlatIPBlasReservoir isn't begin");
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

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

                result_handler_->AddResults(i0, i1, j0, j1, ip_block_.get(), segment_id, segment_offset_start, bitmask);
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
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return distance_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return id_array_.get() + idx * this->top_k_;
    }

private:
    UniquePtr<RowID[]> id_array_{};
    UniquePtr<DistType[]> distance_array_{};
    UniquePtr<DistType[]> ip_block_{};

    UniquePtr<ResultHandler> result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

template class KnnFlatIPBlasReservoir<f32>;

} // namespace infinity
