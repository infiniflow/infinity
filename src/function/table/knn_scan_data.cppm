// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import table_function;
import parser;
import global_block_id;
import knn_distance;
import block_index;

export module knn_scan_data;

namespace infinity {

export class KnnScanFunctionData : public TableFunctionData {
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
    UniquePtr<KnnDistanceBase> knn_distance_{nullptr};
public:
    void Init();
};

} // namespace infinity
