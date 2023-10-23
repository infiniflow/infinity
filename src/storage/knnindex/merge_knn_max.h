#pragma once

#include "faiss/utils/ordered_key_value.h"
#include "merge_knn.h"
#include "storage/knnindex/knn_flat/new_result_handler.h"

namespace infinity {

template <typename DistType>
class MergeKnnMax : public MergeKnn<DistType> {
    using HeapResultHandler = NewHeapResultHandler<faiss::CMax<DistType, RowID>>;
    using SingleResultHandler = HeapResultHandler::HeapSingleResultHandler;

public:
    explicit MergeKnnMax(i64 query_count, i64 topk) : MergeKnn<DistType>(query_count, topk) {
        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, this->distance_array_.get(), this->idx_array_.get(), topk);
        single_result_handler_ = MakeUnique<SingleResultHandler>(*heap_result_handler_, query_count);
    }

public:
    void Search(const DistType *dist, const RowID *row_ids, int count) override;

    void Begin() override;

    void End() override;

    DistType *GetDistances() const override;

    RowID *GetIDs() const override;

    DistType *GetDistancesByIdx(i64 idx) const override;

    RowID *GetIDsByIdx(i64 idx) const override;

public:
    UniquePtr<HeapResultHandler> heap_result_handler_{};
    UniquePtr<SingleResultHandler> single_result_handler_{};
};

template class MergeKnnMax<f32>;
} // namespace infinity