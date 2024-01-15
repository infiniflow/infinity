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

export module physical_merge_top;

import stl;
import parser;
import query_context;
import operator_state;
import expression_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import load_meta;
import infinity_exception;
import base_table_ref;
import physical_top;

namespace infinity {

export class PhysicalMergeTop final : public PhysicalOperator {
public:
    explicit PhysicalMergeTop(u64 id,
                              SharedPtr<BaseTableRef> base_table_ref,
                              UniquePtr<PhysicalOperator> left,
                              u32 limit,
                              u32 offset,
                              Vector<SharedPtr<BaseExpression>> sort_expressions,
                              Vector<OrderType> order_by_types,
                              SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kMergeTop, std::move(left), nullptr, id, load_metas), base_table_ref_(std::move(base_table_ref)),
          limit_(limit), offset_(offset), order_by_types_(std::move(order_by_types)), sort_expressions_(std::move(sort_expressions)) {}

    ~PhysicalMergeTop() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    SizeT TaskletCount() override { return left_->TaskletCount(); }

    // for OperatorState
    inline auto const &GetSortExpressions() const { return sort_expressions_; }

    // for InputLoad
    // necessary because MergeTop may be the first operator in a pipeline
    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

private:
    SharedPtr<BaseTableRef> base_table_ref_;             // necessary for InputLoad
    u32 limit_{};                                        // limit value
    u32 offset_{};                                       // offset value
    u32 sort_expr_count_{};                              // number of expressions to sort
    Vector<OrderType> order_by_types_;                   // ASC or DESC
    Vector<SharedPtr<BaseExpression>> sort_expressions_; // expressions to sort
    CompareTwoRowAndPreferLeft prefer_left_function_;    // compare function
};

} // namespace infinity
