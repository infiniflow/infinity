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

export module infinity_core:physical_merge_match_sparse;

import :query_context;
import :operator_state;
import :physical_operator;
import :match_sparse_expression;
import :base_table_ref;
import :physical_scan_base;
import :base_expression;

import data_type;
import match_sparse_expr;

namespace infinity {
struct LoadMeta;

export class PhysicalMergeMatchSparse final : public PhysicalScanBase {
public:
    PhysicalMergeMatchSparse(u64 id,
                             std::unique_ptr<PhysicalOperator> left,
                             u64 table_index,
                             std::shared_ptr<BaseTableRef> base_table_ref,
                             std::shared_ptr<MatchSparseExpression> match_sparse_expr,
                             std::shared_ptr<BaseExpression> filter_expression,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas,
                             bool cache_result);

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

    size_t TaskletCount() override;

    const std::shared_ptr<MatchSparseExpression> &match_sparse_expr() const { return match_sparse_expr_; }

    const std::shared_ptr<BaseExpression> &filter_expression() const { return filter_expression_; }

    i64 GetTopN() const { return match_sparse_expr_->topn_; }

private:
    template <template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state);

    template <typename ResultType, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state);

private:
    std::shared_ptr<MatchSparseExpression> match_sparse_expr_;
    std::shared_ptr<BaseExpression> filter_expression_{};
};

} // namespace infinity
