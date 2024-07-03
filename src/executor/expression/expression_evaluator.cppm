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

export module expression_evaluator;

import stl;
import base_expression;
import aggregate_expression;
import case_expression;
import cast_expression;
import column_expression;
import function_expression;
import reference_expression;
import value_expression;
import in_expression;
import data_block;
import column_vector;
import expression_state;

namespace infinity {

export class ExpressionEvaluator {
public:
    void Init(const DataBlock *input_data_block);

    void Execute(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<AggregateExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CastExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ColumnExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<FunctionExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ValueExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ReferenceExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<InExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

private:
    const DataBlock *input_data_block_{};
    bool in_aggregate_{false};
};

} // namespace infinity
