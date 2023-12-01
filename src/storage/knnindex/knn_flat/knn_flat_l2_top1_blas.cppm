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
import knn_heap;
import knn_result_handler;
import knn_distance;
import knn_partition;
import faiss;
import parser;

import infinity_exception;
import default_values;
import vector_distance;
import use_mlas;

export module knn_flat_l2_top1_blas;

namespace infinity {

export template <typename DistType>
class KnnFlatL2Top1Blas final : public KnnDistance<DistType> {

    using SingleBestResultHandler = SingleBestResultHandler<CompareMax<DistType, RowID>>;
    using SingleResultHandler = SingleBestResultHandler::SingleResultHandler;

public:
    explicit KnnFlatL2Top1Blas(const DistType *queries, i64 query_count, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Top1Blas, elem_data_type, query_count, dimension, 1), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * this->query_count_);

        single_best_result_handler_ = MakeUnique<SingleBestResultHandler>(query_count, distance_array_.get(), id_array_->data());
        //        single_result_handler_ = MakeUnique<SingleResultHandler>(*single_best_result_handler_);
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        // block sizes
        const SizeT bs_x = faiss_distance_compute_blas_query_bs;
        const SizeT bs_y = faiss_distance_compute_blas_database_bs;
        // const SizeT bs_x = 16, bs_y = 16;

        ip_block_ = MakeUnique<DistType[]>(bs_x * bs_y);
        x_norms_ = MakeUnique<DistType[]>(this->query_count_);

        L2NormsSquares(x_norms_.get(), queries_, this->dimension_, this->query_count_);

        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            single_best_result_handler_->begin_multiple(i0, i1);
        }
        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2Top1Blas isn't begin");
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        y_norms_ = MakeUnique<DistType[]>(base_count);
        L2NormsSquares(y_norms_.get(), base, this->dimension_, base_count);

        // block sizes
        const SizeT bs_x = faiss_distance_compute_blas_query_bs;
        const SizeT bs_y = faiss_distance_compute_blas_database_bs;
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
                    matrixA_multiply_transpose_matrixB_output_to_C
                            (queries_ + i0 * this->dimension_, base + j0 * this->dimension_, nxi, nyi, di,
                             ip_block_.get());
                }
                for (i64 i = i0; i < i1; i++) {
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
                single_best_result_handler_->add_results(i0, i1, j0, j1, ip_block_.get(), segment_id, segment_offset_start);
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        for (u64 i = 0; i < this->query_count_; ++i) {
            single_best_result_handler_->end_multiple();
        }

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return single_best_result_handler_->dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return single_best_result_handler_->ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return single_best_result_handler_->dis_tab + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return single_best_result_handler_->ids_tab + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<SingleBestResultHandler> single_best_result_handler_{};
    //    UniquePtr<SingleResultHandler> single_result_handler_{};
    const DistType *queries_{};
    bool begin_{false};

    UniquePtr<DistType[]> ip_block_{};
    UniquePtr<DistType[]> x_norms_{};
    UniquePtr<DistType[]> y_norms_{};
};

template class KnnFlatL2Top1Blas<f32>;

} // namespace infinity
