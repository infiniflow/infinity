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
import sparse_util;

namespace infinity {

export class SparseDistanceBase {};

export template <typename DataType, typename IndexType, typename ResultType = DataType>
class SparseDistance : public SparseDistanceBase {
public:
    using IndexT = IndexType;
    using DataT = DataType;

    SparseDistance(SparseMetricType metric_type) {
        switch (metric_type) {
            case SparseMetricType::kInnerProduct: {
                dist_func_ = SparseIPDistance<DataType, IndexType, ResultType>;
                break;
            }
            default: {
                UnrecoverableError(fmt::format("SparseMetricType: {} is not supported.", (i8)metric_type));
            }
        }
    }

    ResultType Calculate(const SparseVecRef<DataType, IndexType> &vec1, const SparseVecRef<DataType, IndexType> &vec2) {
        return Calculate(vec1.data_, vec1.indices_, vec1.nnz_, vec2.data_, vec2.indices_, vec2.nnz_);
    }

    ResultType Calculate(const DataType *data, const IndexType *index, SizeT nnz, const DataType *data2, const IndexType *index2, SizeT nnz2) {
        return dist_func_(data, index, nnz, data2, index2, nnz2);
    }

public:
    using DistFunc =
        ResultType (*)(const DataType *data, const IndexType *index, SizeT nnz, const DataType *data2, const IndexType *index2, SizeT nnz2);

    DistFunc dist_func_{};
};

export template <typename IndexType, typename ResultType = IndexType>
class SparseBitDistance : public SparseDistanceBase {
public:
    using IndexT = IndexType;
    using DataT = bool; // not used

    SparseBitDistance(SparseMetricType metric_type) {
        switch (metric_type) {
            case SparseMetricType::kInnerProduct: {
                dist_func_ = SparseBitIPDistance<IndexType, ResultType>;
                break;
            }
            default: {
                UnrecoverableError(fmt::format("SparseMetricType: {} is not supported.", (i8)metric_type));
            }
        }
    }

    ResultType Calculate(const SparseVecRef<DataT, IndexType> &vec1, const SparseVecRef<DataT, IndexType> &vec2) {
        return Calculate(vec1.indices_, vec1.nnz_, vec2.indices_, vec2.nnz_);
    }

    ResultType Calculate(const IndexType *index1, SizeT nnz1, const IndexType *index2, SizeT nnz2) { return dist_func_(index1, nnz1, index2, nnz2); }

public:
    using DistFunc = ResultType (*)(const IndexType *raw1, SizeT nnz1, const IndexType *raw2, SizeT nnz2);

    DistFunc dist_func_{};
};

export class MatchSparseScanFunctionData : public TableFunctionData {
public:
    MatchSparseScanFunctionData() = default;

    MatchSparseScanFunctionData(const SharedPtr<Vector<GlobalBlockID>> &global_block_ids, const SharedPtr<Vector<SegmentID>> &segment_ids)
        : global_block_ids_(global_block_ids), segment_ids_(segment_ids), query_data_(DataBlock::Make()) {}

public:
    SharedPtr<Vector<GlobalBlockID>> global_block_ids_;
    SharedPtr<Vector<SegmentID>> segment_ids_;

    bool evaluated_ = false;
    SharedPtr<DataBlock> query_data_{};

    u32 current_block_ids_idx_ = 0;
    u32 current_segment_ids_idx_ = 0;
    UniquePtr<MergeKnnBase> merge_knn_base_{};
    UniquePtr<SparseDistanceBase> sparse_distance_{};
};

export class MergeSparseFunctionData : public TableFunctionData {
public:
    UniquePtr<MergeKnnBase> merge_knn_base_{};
};

} // namespace infinity
