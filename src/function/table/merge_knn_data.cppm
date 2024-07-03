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

export module merge_knn_data;

import stl;
import table_function;

import base_table_ref;
import merge_knn;
import knn_expr;
import internal_types;

namespace infinity {

export enum class MergeKnnHeapType {
    kMaxHeap,
    kMinHeap,
    kInvalid,
};

export class MergeKnnFunctionData : public TableFunctionData {

public:
    explicit MergeKnnFunctionData(i64 query_count,
                                  i64 topk,
                                  EmbeddingDataType elem_type,
                                  KnnDistanceType knn_distance_type,
                                  SharedPtr<BaseTableRef> table_ref);

private:
    template <typename DistType>
    void InitMergeKnn(KnnDistanceType knn_distance_type);

public:
    i64 query_count_{};
    i64 topk_{};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
    MergeKnnHeapType heap_type_{MergeKnnHeapType::kInvalid};
    SharedPtr<BaseTableRef> table_ref_{};

    SharedPtr<MergeKnnBase> merge_knn_base_{};
};
} // namespace infinity