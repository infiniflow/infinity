//
// Created by jinhai on 23-9-17.
//

module;

import stl;
import table_function;
import parser;
import global_block_id;

export module knn_scan_data;

namespace infinity {

class BlockIndex;

class KnnScanFunctionData : public TableFunctionData {
public:
    KnnScanFunctionData(BlockIndex *block_index,
                        const SharedPtr<Vector<GlobalBlockID>> &global_block_ids,
                        const Vector<SizeT> &non_knn_column_ids,
                        Vector<SizeT> knn_column_ids,
                        i64 topk,
                        i64 dimension,
                        i64 query_embedding_count,
                        void *query_embedding,
                        EmbeddingDataType elem_type,
                        KnnDistanceType knn_distance_type)
        : block_index_(block_index), global_block_ids_(global_block_ids), non_knn_column_ids_(non_knn_column_ids),
          knn_column_ids_(Move(knn_column_ids)), topk_(topk), dimension_(dimension), query_embedding_count_(query_embedding_count),
          query_embedding_(query_embedding), elem_type_(elem_type), knn_distance_type_(knn_distance_type) {}

    BlockIndex *block_index_{};
    const SharedPtr<Vector<GlobalBlockID>> &global_block_ids_{};
    const Vector<SizeT> &non_knn_column_ids_{};
    Vector<SizeT> knn_column_ids_{};
    i64 topk_{0};
    i64 dimension_{0};
    i64 query_embedding_count_{0};
    void *query_embedding_{nullptr};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType knn_distance_type_{KnnDistanceType::kInvalid};

    i64 current_block_ids_idx_{0};
#if 0
    UniquePtr<KnnDistanceBase> knn_distance_{nullptr};
#endif
public:
    void Init();
};

} // namespace infinity
