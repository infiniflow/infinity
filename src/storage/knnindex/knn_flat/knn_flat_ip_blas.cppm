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

#define FINTEGER int
extern int sgemm_(const char *transa,
                  const char *transb,
                  FINTEGER *m,
                  FINTEGER *n,
                  FINTEGER *k,
                  const float *alpha,
                  const float *a,
                  FINTEGER *lda,
                  const float *b,
                  FINTEGER *ldb,
                  float *beta,
                  float *c,
                  FINTEGER *ldc);

import stl;
import knn_heap;
import knn_result_handler;
import knn_distance;
import knn_partition;
import faiss;
import parser;
import third_party;

import infinity_exception;
import default_values;

export module knn_flat_ip_blas;

namespace infinity {

export template <typename DistType>
class KnnFlatIPBlas final : public KnnDistance<DistType> {

    using HeapResultHandler = NewHeapResultHandler<FaissCMin<DistType, RowID>>;

public:
    explicit KnnFlatIPBlas(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIpBlas, elem_data_type, query_count, dimension, topk), queries_(queries) {
        id_array_ = MakeUnique<Vector<RowID>>(topk * this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * topk * this->query_count_);

        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, distance_array_.get(), id_array_->data(), topk);
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        const SizeT bs_x = faiss_distance_compute_blas_query_bs;
        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            heap_result_handler_->begin_multiple(i0, i1);
        }

        const SizeT bs_y = faiss_distance_compute_blas_database_bs;
        ip_block_ = MakeUnique<DistType[]>(bs_x * bs_y);

        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatIPBlas isn't begin");
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

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
                    float one = 1, zero = 0;
                    FINTEGER nyi = j1 - j0, nxi = i1 - i0, di = this->dimension_;
                    sgemm_("Transpose",
                           "Not transpose",
                           &nyi,
                           &nxi,
                           &di,
                           &one,
                           base + j0 * this->dimension_,
                           &di,
                           queries_ + i0 * this->dimension_,
                           &di,
                           &zero,
                           ip_block_.get(),
                           &nyi);
                }

                heap_result_handler_->add_results(i0, i1, j0, j1, ip_block_.get(), segment_id, segment_offset_start);
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        const SizeT bs_x = faiss_distance_compute_blas_query_bs;
        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            heap_result_handler_->end_multiple(i0, i1);
        }

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return heap_result_handler_->heap_dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return heap_result_handler_->heap_ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return heap_result_handler_->heap_dis_tab + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return heap_result_handler_->heap_ids_tab + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};
    UniquePtr<DistType[]> ip_block_{};

    UniquePtr<HeapResultHandler> heap_result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

template class KnnFlatIPBlas<f32>;

} // namespace infinity
