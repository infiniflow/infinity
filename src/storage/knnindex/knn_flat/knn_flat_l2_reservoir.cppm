//
// Created by jinhai on 23-9-28.
//

module;

import stl;
import knn_heap;
import knn_result_handler;
import knn_distance;
import distance;
import knn_partition;
import faiss;
import parser;
import infinity_assert;
import infinity_exception;
import third_party;

export module knn_flat_l2_reservoir;

namespace infinity {

export template <typename DistType>
class KnnFlatL2Reservoir final : public KnnDistance<DistType> {

    using ReservoirResultHandler = NewReservoirResultHandler<FaissCMax<float, RowID>>;
    using ReservoirSingleResultHandler = ReservoirResultHandler::ReservoirSingleResultHandler;

public:
    explicit KnnFlatL2Reservoir(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Reservoir, elem_data_type, query_count, dimension, topk), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(topk * this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * topk * this->query_count_);

        reservoir_result_handler_ = MakeUnique<ReservoirResultHandler>(query_count, distance_array_.get(), id_array_->data(), topk);
        single_reservoir_result_handler_ = MakeUnique<ReservoirSingleResultHandler>(*reservoir_result_handler_);
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        for (SizeT i = 0; i < this->query_count_; ++i) {
            single_reservoir_result_handler_->begin(i);
        }

        begin_ = true;
    }

    void Search(const DistType *base, i16 base_count, i32 segment_id, i16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2Reservoir isn't begin", __FILE_NAME__, __LINE__);
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        for (i64 i = 0; i < this->query_count_; i++) {
            const DistType *x_i = queries_ + i * this->dimension_;
            const DistType *y_j = base;

            for (u32 j = 0; j < base_count; j++, y_j += this->dimension_) {
                DistType ip = fvec_L2sqr(x_i, y_j, this->dimension_);
                single_reservoir_result_handler_->add_result(ip, RowID{segment_id, block_id, j}, i);
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        for (i32 i = 0; i < this->query_count_; ++i) {
            single_reservoir_result_handler_->end(i);
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

template class KnnFlatL2Reservoir<f32>;

} // namespace infinity
