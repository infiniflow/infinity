//
// Created by jinhai on 23-9-27.
//

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
import infinity_assert;
import infinity_exception;
import third_party;
import default_values;

export module knn_flat_ip_blas_reservoir;

namespace infinity {

export template <typename DistType>
class KnnFlatIPBlasReservoir final : public KnnDistance<DistType> {

    using ReservoirResultHandler = NewReservoirResultHandler<FaissCMin<float, RowID>>;
    using ReservoirSingleResultHandler = ReservoirResultHandler::ReservoirSingleResultHandler;

public:
    explicit KnnFlatIPBlasReservoir(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIpBlasReservoir, elem_data_type, query_count, dimension, topk), queries_(queries) {
        id_array_ = MakeUnique<Vector<RowID>>(topk * this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * topk * this->query_count_);

        reservoir_result_handler_ = MakeUnique<ReservoirResultHandler>(query_count, distance_array_.get(), id_array_->data(), topk);
        single_reservoir_result_handler_ = MakeUnique<ReservoirSingleResultHandler>(*reservoir_result_handler_);
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

            reservoir_result_handler_->begin_multiple(i0, i1);
        }

        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatIPBlasReservoir isn't begin", __FILE_NAME__, __LINE__);
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        const SizeT bs_x = faiss_distance_compute_blas_query_bs;
        const SizeT bs_y = faiss_distance_compute_blas_database_bs;
        u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
        UniquePtr<float[]> ip_block(new float[bs_x * bs_y]);
        for (SizeT i0 = 0; i0 < this->query_count_; i0 += bs_x) {
            SizeT i1 = i0 + bs_x;
            if (i1 > this->query_count_)
                i1 = this->query_count_;

            for (i16 j0 = 0; j0 < base_count; j0 += bs_y) {
                i16 j1 = j0 + bs_y;
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
                           ip_block.get(),
                           &nyi);
                }

                reservoir_result_handler_->add_results(i0, i1, j0, j1, ip_block.get(), segment_id, segment_offset_start);
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

            reservoir_result_handler_->end_multiple(i0, i1);
        }

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return reservoir_result_handler_->heap_dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return reservoir_result_handler_->heap_ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return reservoir_result_handler_->heap_dis_tab + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return reservoir_result_handler_->heap_ids_tab + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<ReservoirResultHandler> reservoir_result_handler_{};
    UniquePtr<ReservoirSingleResultHandler> single_reservoir_result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

template class KnnFlatIPBlasReservoir<f32>;

} // namespace infinity
