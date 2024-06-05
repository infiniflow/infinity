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

export module physical_match_sparse_scan;

import stl;
import query_context;
import operator_state;
import physical_operator;
import table_entry;
import match_sparse_expression;
import base_table_ref;
import data_type;
import common_query_filter;
import physical_scan_base;
import sparse_info;
import match_sparse_expr;
import match_sparse_scan_function_data;

namespace infinity {
struct LoadMeta;
struct BlockIndex;
class ColumnVector;

export class PhysicalMatchSparseScan final : public PhysicalScanBase {
public:
    explicit PhysicalMatchSparseScan(u64 id,
                                     u64 table_index,
                                     SharedPtr<BaseTableRef> base_table_ref,
                                     SharedPtr<MatchSparseExpression> match_sparse_expression,
                                     const SharedPtr<CommonQueryFilter> &common_query_filter,
                                     SharedPtr<Vector<LoadMeta>> load_metas);

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;

private:
    template <typename DataType>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *operator_state, const SparseInfo *sparse_info, const SparseMetricType &metric_type);

    template <typename DataType, typename IdxType>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *operator_state, const SparseMetricType &metric_type);

    template <typename DataType, typename IdxType, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *operator_state);

    template <typename DataType, typename IdxType, template <typename, typename> typename C>
    void CalculateOnColumnVector(const ColumnVector &column_vector, SegmentID segment_id, BlockID block_id, BlockOffset row_cnt, MatchSparseScanFunctionData &function_data);

private:
    u64 table_index_ = 0;
    SharedPtr<MatchSparseExpression> match_sparse_expr_;

    // for filter
    SharedPtr<CommonQueryFilter> common_query_filter_;

    // column to search
    ColumnID search_column_id_ = 0;
};

} // namespace infinity
