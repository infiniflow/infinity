//
// Created by jinhai on 23-9-25.
//

#pragma once

#include "storage/knnindex/knn_distance.h"
#include "storage/knnindex/knn_flat/alias.h"

namespace infinity {

template <typename DistType>
class KnnFlatIP final : public KnnDistance<DistType> {

    using HeapResultHandler = NewHeapResultHandler<faiss::CMin<DistType, RowID>>;
    using HeapSingleHandler = HeapResultHandler::HeapSingleResultHandler;

public:
    explicit KnnFlatIP(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIp, elem_data_type), queries_(queries), query_count_(query_count), dimension_(dimension),
          top_k_(topk) {
        id_array_ = MakeUnique<Vector<RowID>>(topk * query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * topk * query_count_);

        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, distance_array_.get(), id_array_->data(), topk);
        single_heap_result_handler_ = MakeUnique<HeapSingleHandler>(*heap_result_handler_, query_count);
    }

    void Begin() final;

    void Search(const DistType *base, i16 base_count, i32 segment_id, i16 block_id) final;

    void End() final;

    [[nodiscard]] inline DistType *GetDistances() const final { return heap_result_handler_->heap_dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return heap_result_handler_->heap_ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(i64 idx) const final {
        if (idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return heap_result_handler_->heap_dis_tab + idx * top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(i64 idx) const final {
        if (idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return heap_result_handler_->heap_ids_tab + idx * top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<HeapResultHandler> heap_result_handler_{};
    UniquePtr<HeapSingleHandler> single_heap_result_handler_{};
    const DistType *queries_{};
    i64 query_count_{};
    i64 dimension_{};
    i64 top_k_{};
    bool begin_{false};
};

template class KnnFlatIP<f32>;

} // namespace infinity
