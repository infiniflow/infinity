
module;

import stl;
import table_function;
import parser;
import base_table_ref;
import merge_knn;

export module merge_knn_data;

namespace infinity {

//class MergeKnnBase;
//class ResultHandler;
//class SingleResultHandler;

export enum class MergeKnnHeapType {
    kMaxHeap,
    kMinHeap,
    kInvalid,
};

export class MergeKnnFunctionData : public TableFunctionData {

public:
    explicit MergeKnnFunctionData(i64 total_parallel_n,
                                  i64 query_count,
                                  i64 topk,
                                  EmbeddingDataType elem_type,
                                  KnnDistanceType knn_distance_type,
                                  SharedPtr<BaseTableRef> table_ref);

private:
    template <typename DistType>
    void InitMergeKnn(i64 query_count, i64 topk, KnnDistanceType knn_distance_type);

public:
    i64 total_parallel_n_{};
    i64 current_parallel_idx_{};
    i64 query_count_{};
    i64 topk_{};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
    MergeKnnHeapType heap_type_{MergeKnnHeapType::kInvalid};
    SharedPtr<BaseTableRef> table_ref_{};

    SharedPtr<MergeKnnBase> merge_knn_base_{};
};
} // namespace infinity