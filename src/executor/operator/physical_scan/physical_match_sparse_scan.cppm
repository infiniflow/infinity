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

export module infinity_core:physical_match_sparse_scan;

import :stl;
import :query_context;
import :operator_state;
import :physical_operator;
import :match_sparse_expression;
import :base_table_ref;
import :common_query_filter;
import :physical_scan_base;
import :physical_filter_scan_base;

import sparse_info;
import match_sparse_expr;
import data_type;

namespace infinity {
struct LoadMeta;
struct BlockIndex;
struct ColumnVector;

export class PhysicalMatchSparseScan final : public PhysicalFilterScanBase {
public:
    PhysicalMatchSparseScan(u64 id,
                            u64 table_index,
                            std::shared_ptr<BaseTableRef> base_table_ref,
                            std::shared_ptr<MatchSparseExpression> match_sparse_expression,
                            const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas);

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override;

    size_t TaskletCount() override;

    void PlanWithIndex(QueryContext *query_context);

    std::vector<std::shared_ptr<std::vector<SegmentID>>>
    PlanWithIndex(std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> &block_groups, i64 parallel_count, QueryContext *query_context);

    std::shared_ptr<MatchSparseExpression> match_sparse_expr() const { return match_sparse_expr_; }

private:
    template <typename DataType>
    void ExecuteInner(QueryContext *query_context,
                      MatchSparseScanOperatorState *operator_state,
                      const SparseInfo *sparse_info,
                      const SparseMetricType &metric_type);

    template <typename DataType, typename IdxType>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *operator_state, const SparseMetricType &metric_type);

    template <typename DataType, typename IdxType, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *operator_state);

    template <typename DataT, typename IdxType, typename ResultType, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state);

    template <typename DistFunc, typename MergeHeap, typename DataType>
    void
    ExecuteInnerT(DistFunc *dist_func, MergeHeap *merge_heap, QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state);

private:
    std::shared_ptr<MatchSparseExpression> match_sparse_expr_;

    // column to search
    ColumnID search_column_id_ = 0;
};

} // namespace infinity
