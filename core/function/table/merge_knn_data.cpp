
module;

import stl;
import parser;
import base_table_ref;
import infinity_assert;
import infinity_exception;
import merge_knn;
import faiss;

module merge_knn_data;

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
            Error<ExecutorException>("Invalid element type", __FILE_NAME__, __LINE__);
        }
        case kElemFloat: {
            MergeKnnFunctionData::InitMergeKnn<f32>(query_count, topk, knn_distance_type);
            break;
        }
        default: {
            Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
        }
    }
}

template <typename DataType>
void MergeKnnFunctionData::InitMergeKnn(i64 query_count, i64 topk, KnnDistanceType knn_distance_type) {
    switch (knn_distance_type) {
        case KnnDistanceType::kInvalid: {
            Error<ExecutorException>("Invalid knn distance type", __FILE_NAME__, __LINE__);
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeShared<MergeKnn<f32, FaissCMax>>(query_count, topk);
            merge_knn_max->Begin();
            merge_knn_base_ = Move(merge_knn_max);
            heap_type_ = MergeKnnHeapType::kMaxHeap;
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeShared<MergeKnn<f32, FaissCMin>>(query_count, topk);
            merge_knn_min->Begin();
            merge_knn_base_ = Move(merge_knn_min);
            heap_type_ = MergeKnnHeapType::kMinHeap;
            break;
        }
    }
}
} // namespace infinity