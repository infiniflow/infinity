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
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import data_table;
import expression_evaluator;
import expression_selector;

export module physical_filter;

namespace infinity {

export class PhysicalFilter : public PhysicalOperator {
public:
    explicit PhysicalFilter(u64 id, SharedPtr<PhysicalOperator> left, SharedPtr<BaseExpression> condition)
        : PhysicalOperator(PhysicalOperatorType::kFilter, Move(left), nullptr, id), condition_(Move(condition)) {}

    ~PhysicalFilter() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    inline const SharedPtr<BaseExpression> &condition() const { return condition_; }

private:
    SharedPtr<BaseExpression> condition_;

    ExpressionEvaluator executor_;
    ExpressionSelector selector_;

    SharedPtr<DataTable> input_table_{};
};

} // namespace infinity
