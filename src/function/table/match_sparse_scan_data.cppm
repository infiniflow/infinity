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

export module match_sparse_scan_function_data;

import stl;
import table_function;
import global_block_id;
import block_index;
import merge_knn;
import data_block;
import match_sparse_expr;
import match_sparse_expression;
import infinity_exception;
import third_party;
import knn_result_handler;
import sparse_vector_distance;

namespace infinity {

export class SparseDistanceBase {};

export template <typename DataType, typename IndexType>
class SparseDistance : public SparseDistanceBase {
public:
    SparseDistance(SparseMetricType metric_type) {
        switch (metric_type) {
            case SparseMetricType::kInnerProduct: {
                dist_func_ = SparseIPDistance<DataType, IndexType>;
                break;
            }
            default: {
                UnrecoverableError(fmt::format("SparseMetricType: {} is not supported.", (i8)metric_type));
            }
        }
    }

    DataType Calculate(const char *raw1, SizeT nnz1, const char *raw2, SizeT nnz2) {
        const IndexType *index1 = reinterpret_cast<const IndexType *>(raw1);
        const DataType *data1 = reinterpret_cast<const DataType *>(raw1 + nnz1 * sizeof(IndexType));
        const IndexType *index2 = reinterpret_cast<const IndexType *>(raw2);
        const DataType *data2 = reinterpret_cast<const DataType *>(raw2 + nnz2 * sizeof(IndexType));
        return Calculate(data1, index1, nnz1, data2, index2, nnz2);
    }

    DataType Calculate(const DataType *data, const IndexType *index, SizeT nnz, const DataType *data2, const IndexType *index2, SizeT nnz2) {
        return dist_func_(data, index, nnz, data2, index2, nnz2);
    }

public:
    using DistFunc =
        DataType (*)(const DataType *data, const IndexType *index, SizeT nnz, const DataType *data2, const IndexType *index2, SizeT nnz2);

    DistFunc dist_func_{};
};

export class MatchSparseScanFunctionData : public TableFunctionData {
public:
    MatchSparseScanFunctionData() = default;

    MatchSparseScanFunctionData(const BlockIndex *block_index, const SharedPtr<Vector<GlobalBlockID>> &global_block_ids)
        : block_index_(block_index), global_block_ids_(global_block_ids), query_data_(DataBlock::Make()) {}

public:
    const BlockIndex *block_index_{};
    SharedPtr<Vector<GlobalBlockID>> global_block_ids_;

    bool evaluated_ = false;
    SharedPtr<DataBlock> query_data_{};

    u32 current_block_ids_idx_ = 0;
    UniquePtr<MergeKnnBase> merge_knn_base_{};
    UniquePtr<SparseDistanceBase> sparse_distance_{};
};

export class MergeSparseFunctionData : public TableFunctionData {
public:
    UniquePtr<MergeKnnBase> merge_knn_base_{};
};

} // namespace infinity
