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

export module physical_merge_match_sparse;

import stl;
import query_context;
import operator_state;
import physical_operator;
import table_entry;
import match_sparse_expression;
import base_table_ref;
import data_type;
import physical_scan_base;
import match_sparse_expr;
import base_expression;

namespace infinity {
struct LoadMeta;

export class PhysicalMergeMatchSparse final : public PhysicalScanBase {
public:
    PhysicalMergeMatchSparse(u64 id,
                             UniquePtr<PhysicalOperator> left,
                             u64 table_index,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchSparseExpression> match_sparse_expr,
                             SharedPtr<BaseExpression> filter_expression,
                             SharedPtr<Vector<LoadMeta>> load_metas,
                             bool cache_result);

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

    SizeT TaskletCount() override;

    const SharedPtr<MatchSparseExpression> &match_sparse_expr() const { return match_sparse_expr_; }

    const SharedPtr<BaseExpression> &filter_expression() const { return filter_expression_; }

    i64 GetTopN() const { return match_sparse_expr_->topn_; }

private:
    template <template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state);

    template <typename ResultType, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state);

private:
    SharedPtr<MatchSparseExpression> match_sparse_expr_;
    SharedPtr<BaseExpression> filter_expression_{};
};

} // namespace infinity
