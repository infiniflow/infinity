//
// Created by jinhai on 23-9-27.
//

module;

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

export module knn_flat_l2_top1;

namespace infinity {

export template <typename DistType>
class KnnFlatL2Top1 final : public KnnDistance<DistType> {

    using SingleBestResultHandler = SingleBestResultHandler<FaissCMax<DistType, RowID>>;
    using SingleResultHandler = SingleBestResultHandler::SingleResultHandler;

public:
    explicit KnnFlatL2Top1(const DistType *queries, i64 query_count, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Top1, elem_data_type, query_count, dimension, 1), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * this->query_count_);

        single_best_result_handler_ = MakeUnique<SingleBestResultHandler>(query_count, distance_array_.get(), id_array_->data());
        single_result_handler_ = MakeUnique<SingleResultHandler>(*single_best_result_handler_);
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        for (SizeT i = 0; i < this->query_count_; ++i) {
            single_result_handler_->begin(i);
        }

        begin_ = true;
    }

    void Search(const DistType *base, i16 base_count, i32 segment_id, i16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2Top1 isn't begin", __FILE_NAME__, __LINE__);
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        for (i64 i = 0; i < this->query_count_; i++) {
            const DistType *x_i = queries_ + i * this->dimension_;
            const DistType *y_j = base;

            for (u32 j = 0; j < base_count; j++, y_j += this->dimension_) {

                DistType l2_distance = fvec_L2sqr(x_i, y_j, this->dimension_);
                single_result_handler_->add_result(l2_distance, RowID{segment_id, block_id, j}, i);
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        for (i32 i = 0; i < this->query_count_; ++i) {
            single_result_handler_->end(i);
        }

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return single_best_result_handler_->dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return single_best_result_handler_->ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return single_best_result_handler_->dis_tab + idx * 1;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return single_best_result_handler_->ids_tab + idx * 1;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<SingleBestResultHandler> single_best_result_handler_{};
    UniquePtr<SingleResultHandler> single_result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

template class KnnFlatL2Top1<f32>;

} // namespace infinity
