//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "storage/knnindex/knn_distance.h"
#include "storage/knnindex/knn_flat/alias.h"

namespace infinity {

template<typename DistType>
class KnnFlatL2Top1Blas final : public KnnDistance<DistType> {

    using SingleBestResultHandler = SingleBestResultHandler<faiss::CMax<DistType, CompoundID>>;
    using SingleResultHandler = SingleBestResultHandler::SingleResultHandler;

public:
    explicit
    KnnFlatL2Top1Blas(const DistType* queries,
                      i64 query_count,
                      i64 dimension,
                      EmbeddingDataType elem_data_type)
            : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Top1Blas, elem_data_type),
              queries_(queries),
              query_count_(query_count),
              dimension_(dimension) {

        id_array_ = MakeUnique<Vector<CompoundID>>(query_count_, CompoundID(-1, -1));
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * query_count_);

        single_best_result_handler_ = MakeUnique<SingleBestResultHandler>(query_count,
                                                                          distance_array_.get(),
                                                                          id_array_->data());
//        single_result_handler_ = MakeUnique<SingleResultHandler>(*single_best_result_handler_);

    }

    void
    Begin() final;

    void
    Search(const DistType* base,
           i64 base_count,
           i32 segment_id,
           i16 block_id) final;

    void
    End() final;

    [[nodiscard]] inline DistType*
    GetDistances() const final {
        return single_best_result_handler_->dis_tab;
    }

    [[nodiscard]] inline CompoundID*
    GetIDs() const final {
        return single_best_result_handler_->ids_tab;
    }

    [[nodiscard]] inline DistType*
    GetDistanceByIdx(i64 idx) const final {
        if(idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return single_best_result_handler_->dis_tab + idx * 1;
    }

    [[nodiscard]] inline CompoundID*
    GetIDByIdx(i64 idx) const final {
        if(idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return single_best_result_handler_->ids_tab + idx * 1;
    }

private:
    UniquePtr<Vector<CompoundID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<SingleBestResultHandler> single_best_result_handler_{};
//    UniquePtr<SingleResultHandler> single_result_handler_{};
    const DistType* queries_{};
    i64 query_count_{};
    i64 dimension_{};
    bool begin_{false};

    UniquePtr<DistType[]> ip_block_{};
    UniquePtr<DistType[]> x_norms_{};
    UniquePtr<DistType[]> y_norms_{};
};

template
class KnnFlatL2Top1Blas<f32>;

}
