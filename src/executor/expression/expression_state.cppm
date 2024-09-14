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

export module expression_state;

import stl;
import base_expression;
import aggregate_expression;
import case_expression;
import cast_expression;
import reference_expression;
import function_expression;
import value_expression;
import in_expression;
import filter_fulltext_expression;
import column_vector;

namespace infinity {

export enum class AggregateFlag : i8 {
    kUninitialized = 0,
    kRunning = 1,
    kFinish = 2,
    kRunAndFinish = 3,
};

export class ExpressionState {
public:
    // Static functions
    static SharedPtr<ExpressionState> CreateState(const SharedPtr<BaseExpression> &expression, char * = nullptr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<AggregateExpression> &agg_expr, char *agg_state, const AggregateFlag agg_flag);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CaseExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CastExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ReferenceExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<FunctionExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ValueExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<InExpression> &in_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<FilterFulltextExpression> &filter_fulltext_expr);

public:
    void AddChild(const SharedPtr<BaseExpression> &expression);

    Vector<SharedPtr<ExpressionState>> &Children() { return children_; }

    SharedPtr<ColumnVector> &OutputColumnVector() { return column_vector_; }

    char *agg_state_{};

    AggregateFlag agg_flag_{AggregateFlag::kUninitialized};

private:
    Vector<SharedPtr<ExpressionState>> children_;
    String name_;

    // output blocks and each block have one output column;
    SharedPtr<ColumnVector> column_vector_{nullptr};
};

} // namespace infinity
