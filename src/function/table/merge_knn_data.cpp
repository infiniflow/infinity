#include "merge_knn_data.h"
#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"
#include "storage/knnindex/merge_knn.h"

namespace infinity {

MergeKnnFunctionData::MergeKnnFunctionData(i64 total_parallel_n,
                                           i64 query_count,
                                           i64 topk,
                                           EmbeddingDataType elem_type,
                                           KnnDistanceType knn_distance_type,
                                           SharedPtr<BaseTableRef> table_ref)
    : total_parallel_n_(total_parallel_n), current_parallel_idx_(0), query_count_(query_count), topk_(topk), elem_type_(elem_type),
      table_ref_(table_ref) {
    switch (elem_type) {
        case kElemInvalid: {
            ExecutorError("Invalid element type");
        }
        case kElemFloat: {
            MergeKnnFunctionData::InitMergeKnn<f32>(query_count, topk, knn_distance_type);
            break;
        }
        default: {
            NotImplementError("Not implemented");
        }
    }
}

template <typename DataType>
void MergeKnnFunctionData::InitMergeKnn(i64 query_count, i64 topk, KnnDistanceType knn_distance_type) {
    switch (knn_distance_type) {
        case KnnDistanceType::kInvalid: {
            ExecutorError("Invalid knn distance type");
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeShared<MergeKnn<f32, faiss::CMax>>(query_count, topk);
            merge_knn_max->Begin();
            merge_knn_base_ = std::move(merge_knn_max);
            heap_type_ = MergeKnnHeapType::kMaxHeap;
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeShared<MergeKnn<f32, faiss::CMin>>(query_count, topk);
            merge_knn_min->Begin();
            merge_knn_base_ = std::move(merge_knn_min);
            heap_type_ = MergeKnnHeapType::kMinHeap;
            break;
        }
    }
}
} // namespace infinity