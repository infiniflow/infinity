//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "storage/knnindex/knn_distance.h"
#include "storage/knnindex/knn_flat/alias.h"

namespace infinity {

template <typename DistType>
class KnnFlatL2Top1Blas final : public KnnDistance<DistType> {

    using SingleBestResultHandler = SingleBestResultHandler<faiss::CMax<DistType, RowID>>;
    using SingleResultHandler = SingleBestResultHandler::SingleResultHandler;

public:
    explicit KnnFlatL2Top1Blas(const DistType *queries, i64 query_count, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Top1Blas, elem_data_type, query_count, dimension, 1), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * this->query_count_);

        single_best_result_handler_ = MakeUnique<SingleBestResultHandler>(query_count, distance_array_.get(), id_array_->data());
        //        single_result_handler_ = MakeUnique<SingleResultHandler>(*single_best_result_handler_);
    }

    void Begin() final;

    void Search(const DistType *base, i16 base_count, i32 segment_id, i16 block_id) final;

    void End() final;

    [[nodiscard]] inline DistType *GetDistances() const final { return single_best_result_handler_->dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return single_best_result_handler_->ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return single_best_result_handler_->dis_tab + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            ExecutorError("Query index exceeds the limit")
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
