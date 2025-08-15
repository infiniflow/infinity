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

export module infinity_core:physical_merge_top;

import :stl;
import :query_context;
import :operator_state;
import :expression_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :base_table_ref;
import :physical_top;

import internal_types;
import select_statement;
import data_type;

namespace infinity {

export class PhysicalMergeTop final : public PhysicalOperator {
public:
    explicit PhysicalMergeTop(u64 id,
                              std::shared_ptr<BaseTableRef> base_table_ref,
                              std::unique_ptr<PhysicalOperator> left,
                              u32 limit,
                              u32 offset,
                              std::vector<std::shared_ptr<BaseExpression>> sort_expressions,
                              std::vector<OrderType> order_by_types,
                              std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kMergeTop, std::move(left), nullptr, id, load_metas), base_table_ref_(std::move(base_table_ref)),
          limit_(limit), offset_(offset), order_by_types_(std::move(order_by_types)), sort_expressions_(std::move(sort_expressions)) {}

    ~PhysicalMergeTop() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

    size_t TaskletCount() override { return left_->TaskletCount(); }

    // for OperatorState and Explain
    inline auto const &GetSortExpressions() const { return sort_expressions_; }

    // for Explain
    inline auto const &GetOrderbyTypes() const { return order_by_types_; }

    // for Explain
    inline auto GetLimit() const { return limit_; }

    // for Explain
    inline auto GetOffset() const { return offset_; }

    // for InputLoad
    // necessary because MergeTop may be the first operator in a pipeline
    void FillingTableRefs(std::unordered_map<size_t, std::shared_ptr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

private:
    std::shared_ptr<BaseTableRef> base_table_ref_;             // necessary for InputLoad
    u32 limit_{};                                        // limit value
    u32 offset_{};                                       // offset value
    u32 sort_expr_count_{};                              // number of expressions to sort
    std::vector<OrderType> order_by_types_;                   // ASC or DESC
    std::vector<std::shared_ptr<BaseExpression>> sort_expressions_; // expressions to sort
    CompareTwoRowAndPreferLeft prefer_left_function_;    // compare function
};

} // namespace infinity
