//
// Created by jinhai on 23-9-17.
//


#pragma once

#include "common/types/internal_types.h"
#include "function/table_function.h"
#include "function/function_data.h"
#include "storage/txn/constants.h"
#include "parser/parsed_expr/knn_expr.h"

namespace infinity {

class KnnScanFunctionData: public TableFunctionData {
public:
    KnnScanFunctionData(const Vector<SegmentEntry*>* segment_entries_ptr,
                        const SharedPtr<Vector<u64>>& segment_indexes,
                        const Vector<SizeT>& non_knn_column_ids,
                        Vector<SizeT> knn_column_ids,
                        i64 topk,
                        i64 dimension,
                        i64 query_embedding_count,
                        void* query_embedding,
                        EmbeddingDataType elem_type,
                        KnnDistanceType knn_distance_type)
            : segment_entries_ptr_(segment_entries_ptr),
              segment_indexes_(segment_indexes),
              non_knn_column_ids_(non_knn_column_ids),
              knn_column_ids_(std::move(knn_column_ids)),
              topk_(topk),
              dimension_(dimension),
              query_embedding_count_(query_embedding_count),
              query_embedding_(query_embedding),
              elem_type_(elem_type),
              knn_distance_type_(knn_distance_type)
    {}

    const Vector<SegmentEntry*>* segment_entries_ptr_{};
    const SharedPtr<Vector<u64>>& segment_indexes_{};
    const Vector<SizeT>& non_knn_column_ids_{};
    Vector<SizeT> knn_column_ids_{};
    i64 topk_{0};
    i64 dimension_{0};
    i64 query_embedding_count_{0};
    void* query_embedding_{nullptr};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType knn_distance_type_{KnnDistanceType::kInvalid};

    i64 current_segment_idx_{0};

    // TODO: TopK to be implemented, for the whole task

};

}
